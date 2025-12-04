#include <gtest/gtest.h>
#include <RaeptorCogs/Flags.hpp>

using namespace RaeptorCogs;

// Test enum for flags operations
enum class TestFlags : uint32_t {
    NONE   = 0,
    FLAG_A = 1 << 0,  // 0b00001
    FLAG_B = 1 << 1,  // 0b00010
    FLAG_C = 1 << 2,  // 0b00100
    FLAG_D = 1 << 3,  // 0b01000
    FLAG_E = 1 << 4,  // 0b10000
};

// Plain enum for testing
enum PlainEnum {
    PLAIN_A = 1,
    PLAIN_B = 2,
};

TEST(FlagsTest, IsClassEnum) {
    EXPECT_TRUE(isClassEnum<TestFlags>::value);
    EXPECT_FALSE(isClassEnum<PlainEnum>::value);
    EXPECT_FALSE(isClassEnum<int>::value);
}

TEST(FlagsTest, IsPlainEnum) {
    EXPECT_FALSE(isPlainEnum<TestFlags>::value);
    EXPECT_TRUE(isPlainEnum<PlainEnum>::value);
    EXPECT_FALSE(isPlainEnum<int>::value);
}

TEST(FlagsTest, BitwiseOR) {
    TestFlags combined = TestFlags::FLAG_A | TestFlags::FLAG_B;
    EXPECT_EQ(static_cast<uint32_t>(combined), 0b00011);
    
    TestFlags triple = TestFlags::FLAG_A | TestFlags::FLAG_B | TestFlags::FLAG_C;
    EXPECT_EQ(static_cast<uint32_t>(triple), 0b00111);
}

TEST(FlagsTest, BitwiseAND) {
    TestFlags combined = TestFlags::FLAG_A | TestFlags::FLAG_B;
    TestFlags result = combined & TestFlags::FLAG_A;
    EXPECT_EQ(static_cast<uint32_t>(result), static_cast<uint32_t>(TestFlags::FLAG_A));
    
    TestFlags noMatch = combined & TestFlags::FLAG_C;
    EXPECT_EQ(static_cast<uint32_t>(noMatch), 0);
}

TEST(FlagsTest, BitwiseXOR) {
    TestFlags combined = TestFlags::FLAG_A | TestFlags::FLAG_B;
    TestFlags result = combined ^ TestFlags::FLAG_A;
    EXPECT_EQ(static_cast<uint32_t>(result), static_cast<uint32_t>(TestFlags::FLAG_B));
    
    TestFlags toggle = TestFlags::FLAG_A ^ TestFlags::FLAG_B;
    EXPECT_EQ(static_cast<uint32_t>(toggle), 0b00011);
}

TEST(FlagsTest, BitwiseNOT) {
    TestFlags inverted = ~TestFlags::FLAG_A;
    EXPECT_NE(static_cast<uint32_t>(inverted), static_cast<uint32_t>(TestFlags::FLAG_A));
    EXPECT_EQ(static_cast<uint32_t>(inverted), ~static_cast<uint32_t>(TestFlags::FLAG_A));
}

TEST(FlagsTest, CompoundAssignment_OR) {
    TestFlags flags = TestFlags::FLAG_A;
    flags |= TestFlags::FLAG_B;
    EXPECT_EQ(static_cast<uint32_t>(flags), 0b00011);
    
    flags |= TestFlags::FLAG_C;
    EXPECT_EQ(static_cast<uint32_t>(flags), 0b00111);
}

TEST(FlagsTest, CompoundAssignment_AND) {
    TestFlags flags = TestFlags::FLAG_A | TestFlags::FLAG_B | TestFlags::FLAG_C;
    flags &= (TestFlags::FLAG_A | TestFlags::FLAG_B);
    EXPECT_EQ(static_cast<uint32_t>(flags), 0b00011);
}

TEST(FlagsTest, CompoundAssignment_XOR) {
    TestFlags flags = TestFlags::FLAG_A | TestFlags::FLAG_B;
    flags ^= TestFlags::FLAG_B;
    EXPECT_EQ(static_cast<uint32_t>(flags), static_cast<uint32_t>(TestFlags::FLAG_A));
}

TEST(FlagsTest, ComplexCombinations) {
    TestFlags all = TestFlags::FLAG_A | TestFlags::FLAG_B | TestFlags::FLAG_C | TestFlags::FLAG_D | TestFlags::FLAG_E;
    EXPECT_EQ(static_cast<uint32_t>(all), 0b11111);
    
    TestFlags masked = all & (TestFlags::FLAG_B | TestFlags::FLAG_D);
    EXPECT_EQ(static_cast<uint32_t>(masked), 0b01010);
    
    TestFlags toggled = masked ^ TestFlags::FLAG_B;
    EXPECT_EQ(static_cast<uint32_t>(toggled), static_cast<uint32_t>(TestFlags::FLAG_D));
}

TEST(FlagsTest, NoneFlag) {
    TestFlags none = TestFlags::NONE;
    EXPECT_EQ(static_cast<uint32_t>(none), 0);
    
    TestFlags withNone = TestFlags::FLAG_A | TestFlags::NONE;
    EXPECT_EQ(static_cast<uint32_t>(withNone), static_cast<uint32_t>(TestFlags::FLAG_A));
}
