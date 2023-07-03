#include "gtest/gtest.h"

#include "SingleThreadRingBuffer.h"

TEST(SingleThreadRingBuffer, SingleThreadSimpleTest) {
  uint32_t numItems = 10;
  SingleThreadRingBuffer<int> ring(numItems + 1);
  EXPECT_TRUE(ring.empty());
  EXPECT_TRUE(ring.push(1));
  EXPECT_EQ(*ring.front(), 1);
  ring.pop();
  EXPECT_TRUE(ring.empty());
}

TEST(SingleThreadRingBuffer, SingleThreadPopulateTest) {
  uint32_t numItems = 10;
  SingleThreadRingBuffer<int> ring(numItems + 1);
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

TEST(SingleThreadRingBuffer, SingleThreadEmptyTest) {
  uint32_t numItems = 10;
  SingleThreadRingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
    ring.pop();
    EXPECT_TRUE(ring.empty());
  }
}

TEST(SingleThreadRingBuffer, SingleThreadFrontPtrTest) {
  uint32_t numItems = 100;
  SingleThreadRingBuffer<int> ring(numItems + 1);
  for (uint32_t i = 0; i < numItems; i++) {
    EXPECT_TRUE(ring.push(i));
    const uint32_t front = *ring.front();
    EXPECT_EQ(front, i);
    ring.pop();
    EXPECT_TRUE(ring.empty());
  }
}

TEST(SingleThreadRingBuffer, SingleThreadReadTest) {
  uint32_t numItems = 100;
  SingleThreadRingBuffer<int> ring(numItems + 1);
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
