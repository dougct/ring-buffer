#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "RingBuffer.h"

void bench() {
  const size_t iter = 100000;
  RingBuffer<size_t> ring(iter / 1000 + 1);
  std::atomic<bool> flag(false);
  long sum = 0;
  std::thread producer([&] {
    while (!flag) {
      ;
    }

    size_t i = 0;
    while (i < iter) {
      if (ring.push(i)) {
        i++;
      }
    }
  });

  auto start = std::chrono::system_clock::now();
  flag = true;
  for (size_t i = 0; i < iter; ++i) {
    while (!ring.front()) {
      ;
    }
    sum += *ring.front();
    ring.pop();
  }
  auto end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  assert(ring.front() == nullptr);
  assert(sum == iter * (iter - 1) / 2);

  producer.join();
  std::cout << duration.count() / iter << " ns/iteration" << std::endl;
}

int main() {
  bench();
}
