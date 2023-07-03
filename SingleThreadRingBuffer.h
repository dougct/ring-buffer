#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T>
class SingleThreadRingBuffer {
 private:
  const size_t capacity_;
  T* const records_;
  size_t readIndex_;
  size_t writeIndex_;

 public:
  explicit SingleThreadRingBuffer(size_t numRecords)
      : capacity_(numRecords),
        records_(static_cast<T*>(std::malloc(sizeof(T) * (capacity_ + 1)))),
        readIndex_(0),
        writeIndex_(0) {
    assert(capacity_ >= 2);
    if (!records_) {
      throw std::bad_alloc();
    }
  }

  ~SingleThreadRingBuffer() {
    if (!std::is_trivially_destructible_v<T>) {
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

  bool empty() const { return readIndex_ == writeIndex_; }

  bool full() const {
    auto nextWriteIndex = writeIndex_ + 1;
    if (nextWriteIndex == capacity_) {
      nextWriteIndex = 0;
    }
    if (nextWriteIndex != readIndex_) {
      return false;
    }
    return true;
  }

  size_t size() const {
    auto estimate = writeIndex_ - readIndex_;
    if (estimate < 0) {
      estimate += capacity_;
    }
    return estimate;
  }

  bool push(const T& record) {
    auto nextWriteIndex = writeIndex_ + 1;
    // Wrap around if we reached the end of the queue
    if (nextWriteIndex == capacity_) {
      nextWriteIndex = 0;
    }
    // If writeIndex is one element behind readIndex the queue is full
    if (nextWriteIndex != readIndex_) {
      records_[writeIndex_] = record;
      writeIndex_ = nextWriteIndex;
      return true;
    }

    return false;
  }

  void pop() {
    assert(readIndex_ != writeIndex_);
    records_[readIndex_].~T();
    auto nextRecord = readIndex_ + 1;
    // Wrap around if queue is full
    if (nextRecord == capacity_) {
      nextRecord = 0;
    }
    readIndex_ = nextRecord;
  }

  T* front() {
    if (readIndex_ == writeIndex_) {
      return nullptr;
    }
    return &records_[readIndex_];
  }
};
