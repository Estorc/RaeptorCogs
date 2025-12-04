#include <gtest/gtest.h>
#include <RaeptorCogs/BitOp.hpp>

using namespace RaeptorCogs;

TEST(BitOpTest, NextPowerOf2_PowerOf2Input) {
    EXPECT_EQ(NextPowerOf2(1), 1);
    EXPECT_EQ(NextPowerOf2(2), 2);
    EXPECT_EQ(NextPowerOf2(4), 4);
    EXPECT_EQ(NextPowerOf2(8), 8);
    EXPECT_EQ(NextPowerOf2(16), 16);
    EXPECT_EQ(NextPowerOf2(256), 256);
    EXPECT_EQ(NextPowerOf2(1024), 1024);
}

TEST(BitOpTest, NextPowerOf2_NonPowerOf2Input) {
    EXPECT_EQ(NextPowerOf2(3), 4);
    EXPECT_EQ(NextPowerOf2(5), 8);
    EXPECT_EQ(NextPowerOf2(6), 8);
    EXPECT_EQ(NextPowerOf2(7), 8);
    EXPECT_EQ(NextPowerOf2(9), 16);
    EXPECT_EQ(NextPowerOf2(15), 16);
    EXPECT_EQ(NextPowerOf2(17), 32);
    EXPECT_EQ(NextPowerOf2(300), 512);
}

TEST(BitOpTest, NextPowerOf2_Zero) {
    EXPECT_EQ(NextPowerOf2(0), 0);
}

TEST(BitOpTest, NextPowerOf2_LargeValues) {
    EXPECT_EQ(NextPowerOf2(1000), 1024);
    EXPECT_EQ(NextPowerOf2(1023), 1024);
    EXPECT_EQ(NextPowerOf2(1025), 2048);
    EXPECT_EQ(NextPowerOf2(100000), 131072);
}

TEST(BitOpTest, NextPowerOf2_EdgeCases) {
    // Test boundary values around powers of 2
    EXPECT_EQ(NextPowerOf2(127), 128);
    EXPECT_EQ(NextPowerOf2(128), 128);
    EXPECT_EQ(NextPowerOf2(129), 256);
    EXPECT_EQ(NextPowerOf2(255), 256);
    EXPECT_EQ(NextPowerOf2(257), 512);
}
