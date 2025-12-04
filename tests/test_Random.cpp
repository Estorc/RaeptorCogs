#include <gtest/gtest.h>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <set>
#include <cmath>

using namespace RaeptorCogs;

TEST(RandomTest, GetIntInRange) {
    const int min = 1;
    const int max = 10;
    const int iterations = 1000;
    
    for (int i = 0; i < iterations; ++i) {
        int value = Random().getInt(min, max);
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
    }
}

TEST(RandomTest, GetIntSingleValue) {
    const int value = 5;
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(Random().getInt(value, value), value);
    }
}

TEST(RandomTest, GetIntNegativeRange) {
    const int min = -10;
    const int max = -1;
    
    for (int i = 0; i < 100; ++i) {
        int value = Random().getInt(min, max);
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
    }
}

TEST(RandomTest, GetIntCrossZero) {
    const int min = -5;
    const int max = 5;
    
    for (int i = 0; i < 100; ++i) {
        int value = Random().getInt(min, max);
        EXPECT_GE(value, min);
        EXPECT_LE(value, max);
    }
}

TEST(RandomTest, GetIntDistribution) {
    const int min = 1;
    const int max = 5;
    const int iterations = 10000;
    std::set<int> uniqueValues;
    
    for (int i = 0; i < iterations; ++i) {
        int value = Random().getInt(min, max);
        uniqueValues.insert(value);
    }
    
    // Should generate all possible values in the range
    EXPECT_EQ(uniqueValues.size(), max - min + 1);
    for (int i = min; i <= max; ++i) {
        EXPECT_TRUE(uniqueValues.count(i) > 0);
    }
}

TEST(RandomTest, GetFloatInRange) {
    const float min = 0.0f;
    const float max = 1.0f;
    const int iterations = 1000;
    
    for (int i = 0; i < iterations; ++i) {
        float value = Random().getFloat(min, max);
        EXPECT_GE(value, min);
        EXPECT_LT(value, max);
    }
}

TEST(RandomTest, GetFloatLargeRange) {
    const float min = -100.0f;
    const float max = 100.0f;
    
    for (int i = 0; i < 100; ++i) {
        float value = Random().getFloat(min, max);
        EXPECT_GE(value, min);
        EXPECT_LT(value, max);
    }
}

TEST(RandomTest, GetFloatNarrowRange) {
    const float min = 0.1f;
    const float max = 1.0f;
    
    for (int i = 0; i < 100; ++i) {
        float value = Random().getFloat(min, max);
        EXPECT_GE(value, min);
        EXPECT_LT(value, max);
    }
}

TEST(RandomTest, GetFloatSameMinMax) {
    const float value = 5.5f;
    // When min == max, should return min (or close to it)
    float result = Random().getFloat(value, value);
    EXPECT_FLOAT_EQ(result, value);
}

TEST(RandomTest, GetFloatDistribution) {
    const float min = 0.0f;
    const float max = 1.0f;
    const int iterations = 10000;
    float sum = 0.0f;
    
    for (int i = 0; i < iterations; ++i) {
        sum += Random().getFloat(min, max);
    }
    
    float average = sum / iterations;
    // Average should be close to midpoint of range (0.5)
    // Allow 5% tolerance
    EXPECT_NEAR(average, 0.5f, 0.05f);
}

TEST(RandomTest, SingletonBehavior) {
    // Test that Random() returns the same singleton instance
    auto& r1 = Random();
    auto& r2 = Random();
    
    // They should be the same object
    EXPECT_EQ(&r1, &r2);
}

TEST(RandomTest, GetFloatNegativeRange) {
    const float min = -10.0f;
    const float max = -1.0f;
    
    for (int i = 0; i < 100; ++i) {
        float value = Random().getFloat(min, max);
        EXPECT_GE(value, min);
        EXPECT_LT(value, max);
    }
}

TEST(RandomTest, GetFloatCrossZero) {
    const float min = -5.0f;
    const float max = 5.0f;
    
    bool hasNegative = false;
    bool hasPositive = false;
    
    for (int i = 0; i < 1000; ++i) {
        float value = Random().getFloat(min, max);
        EXPECT_GE(value, min);
        EXPECT_LT(value, max);
        
        if (value < 0.0f) hasNegative = true;
        if (value > 0.0f) hasPositive = true;
    }
    
    // Over 1000 iterations, should have both negative and positive values
    EXPECT_TRUE(hasNegative);
    EXPECT_TRUE(hasPositive);
}
