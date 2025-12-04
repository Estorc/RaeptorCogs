#include <gtest/gtest.h>
#include <RaeptorCogs/BitArray.hpp>

using namespace RaeptorCogs;

TEST(BitArrayTest, DefaultConstruction) {
    BitArray bitArray;
    EXPECT_FALSE(bitArray.test(0));
    EXPECT_FALSE(bitArray.test(100));
}

TEST(BitArrayTest, SetAndTest) {
    BitArray bitArray;
    bitArray.set(5);
    
    EXPECT_TRUE(bitArray.test(5));
    EXPECT_FALSE(bitArray.test(4));
    EXPECT_FALSE(bitArray.test(6));
}

TEST(BitArrayTest, SetMultipleBits) {
    BitArray bitArray;
    bitArray.set(0);
    bitArray.set(5);
    bitArray.set(63);
    bitArray.set(64);
    bitArray.set(127);
    
    EXPECT_TRUE(bitArray.test(0));
    EXPECT_TRUE(bitArray.test(5));
    EXPECT_TRUE(bitArray.test(63));
    EXPECT_TRUE(bitArray.test(64));
    EXPECT_TRUE(bitArray.test(127));
    EXPECT_FALSE(bitArray.test(1));
    EXPECT_FALSE(bitArray.test(62));
    EXPECT_FALSE(bitArray.test(65));
}

TEST(BitArrayTest, ChainedSet) {
    BitArray bitArray;
    bitArray.set(1).set(2).set(3);
    
    EXPECT_TRUE(bitArray.test(1));
    EXPECT_TRUE(bitArray.test(2));
    EXPECT_TRUE(bitArray.test(3));
    EXPECT_FALSE(bitArray.test(0));
    EXPECT_FALSE(bitArray.test(4));
}

TEST(BitArrayTest, CrossBoundary) {
    BitArray bitArray;
    // Test across 64-bit boundaries
    bitArray.set(62);
    bitArray.set(63);
    bitArray.set(64);
    bitArray.set(65);
    
    EXPECT_TRUE(bitArray.test(62));
    EXPECT_TRUE(bitArray.test(63));
    EXPECT_TRUE(bitArray.test(64));
    EXPECT_TRUE(bitArray.test(65));
}

TEST(BitArrayTest, Clear) {
    BitArray bitArray;
    bitArray.set(5).set(10).set(100);
    
    EXPECT_TRUE(bitArray.test(5));
    EXPECT_TRUE(bitArray.test(10));
    EXPECT_TRUE(bitArray.test(100));
    
    bitArray.clear();
    
    EXPECT_FALSE(bitArray.test(5));
    EXPECT_FALSE(bitArray.test(10));
    EXPECT_FALSE(bitArray.test(100));
}

TEST(BitArrayTest, LargeIndices) {
    BitArray bitArray;
    bitArray.set(1000);
    bitArray.set(10000);
    
    EXPECT_TRUE(bitArray.test(1000));
    EXPECT_TRUE(bitArray.test(10000));
    EXPECT_FALSE(bitArray.test(999));
    EXPECT_FALSE(bitArray.test(10001));
}

TEST(BitArrayTest, Data) {
    BitArray bitArray;
    bitArray.set(0);
    bitArray.set(1);
    
    auto data = bitArray.data();
    EXPECT_FALSE(data.empty());
    EXPECT_EQ(data[0], 3); // 0b11
}
