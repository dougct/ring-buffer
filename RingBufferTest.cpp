#include <cassert>
#include <thread>

#include "gtest/gtest.h"

#include "RingBuffer.h"

TEST(RingBuffer, SimpleRingBufferTest) {
  uint32_t numItems = 10;
  RingBuffer<int> ring(numItems + 1);
  EXPECT_TRUE(ring.empty());
  EXPECT_TRUE(ring.push(1));
  EXPECT_EQ(*ring.front(), 1);
  ring.pop();
  EXPECT_TRUE(ring.empty());
}

TEST(RingBuffer, PopulateRingBufferTest) {
  uint32_t numItems = 10;
  RingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
  }
  EXPECT_TRUE(ring.full());
  EXPECT_FALSE(ring.push(0));

  for (uint32_t i = 0; i < numItems; i++) {
    ring.pop();
  }
  EXPECT_TRUE(ring.empty());
}

TEST(RingBuffer, EmptyRingBufferTest) {
  uint32_t numItems = 10;
  RingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
    ring.pop();
    EXPECT_TRUE(ring.empty());
  }
}

TEST(RingBuffer, FrontRingBufferTest) {
  uint32_t numItems = 100;
  RingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
    const uint32_t front = *ring.front();
    EXPECT_EQ(front, i);
    ring.pop();
    EXPECT_TRUE(ring.empty());
  }
}

TEST(RingBuffer, ReadRingBufferTest) {
  uint32_t numItems = 100;
  RingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
    int* front = ring.front();
    EXPECT_EQ(static_cast<const uint32_t>(*front), i);
    ring.pop();
    EXPECT_TRUE(ring.empty());
  }
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
