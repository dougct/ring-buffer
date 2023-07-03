
#pragma once

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <class T>
class RingBuffer {
 private:
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t kCacheLineSize =
      std::hardware_destructive_interference_size;
#else
  static constexpr size_t kCacheLineSize = 64;
#endif

  using AtomicIndex = std::atomic<size_t>;

  char pad0_[kCacheLineSize];
  const uint32_t capacity_;
  T* const records_;

  alignas(kCacheLineSize) AtomicIndex readIndex_;
  alignas(kCacheLineSize) AtomicIndex writeIndex_;

  char pad1_[kCacheLineSize - sizeof(AtomicIndex)];
  typedef T value_type;

 public:
  // Note that the number of usable slots in the queue at any
  // given time is actually (size-1), so if you start with an empty queue,
  // full() will return true after size-1 insertions.
  explicit RingBuffer(uint32_t size)
      : capacity_(size),
        records_(static_cast<T*>(std::malloc(sizeof(T) * size))),
        readIndex_(0),
        writeIndex_(0) {
    assert(size >= 2);
    if (!records_) {
      throw std::bad_alloc();
    }
  }

  // Prevent copying
  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;

  ~RingBuffer() {
    // No synchronization needed: only one thread can be doing this
    if (!std::is_trivially_destructible<T>::value) {
      size_t readIndex = readIndex_;
      const size_t endIndex = writeIndex_;
      while (readIndex != endIndex) {
        records_[readIndex].~T();
        if (++readIndex == capacity_) {
          readIndex = 0;
        }
      }
    }
    std::free(records_);
  }

  template <class... Args>
  bool push(Args&&... recordArgs) {
    auto const currentWrite = writeIndex_.load(std::memory_order_relaxed);
    auto nextRecord = currentWrite + 1;
    if (nextRecord == capacity_) {
      nextRecord = 0;
    }
    if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
      new (&records_[currentWrite]) T(std::forward<Args>(recordArgs)...);
      writeIndex_.store(nextRecord, std::memory_order_release);
      return true;
    }
    // The queue is full
    return false;
  }

  void pop() {
    auto const currentRead = readIndex_.load(std::memory_order_relaxed);
    assert(currentRead != writeIndex_.load(std::memory_order_acquire));
    auto nextRecord = currentRead + 1;
    if (nextRecord == capacity_) {
      nextRecord = 0;
    }
    records_[currentRead].~T();
    readIndex_.store(nextRecord, std::memory_order_release);
  }

  T* front() {
    auto const currentRead = readIndex_.load(std::memory_order_relaxed);
    if (writeIndex_.load(std::memory_order_acquire) == currentRead) {
      return nullptr;  // Queue is empty
    }
    return &records_[currentRead];
  }

  bool empty() const {
    return readIndex_.load(std::memory_order_acquire) ==
           writeIndex_.load(std::memory_order_acquire);
  }

  bool full() const {
    size_t nextRecord = writeIndex_.load(std::memory_order_acquire) + 1;
    if (nextRecord == capacity_) {
      nextRecord = 0;
    }
    if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
      return false;
    }
    // queue is full
    return true;
  }

  // * If called by consumer, then true size may be more (because producer may
  //   be adding items concurrently).
  // * If called by producer, then true size may be less (because consumer may
  //   be removing items concurrently).
  // * It is undefined to call this from any other thread.
  size_t size() const {
    std::ptrdiff_t ret = writeIndex_.load(std::memory_order_acquire) -
                         readIndex_.load(std::memory_order_acquire);
    if (ret < 0) {
      ret += capacity_;
    }
    std::cout << std::endl;
    return ret;
  }
};
