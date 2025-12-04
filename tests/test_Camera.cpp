#include <gtest/gtest.h>
#include <RaeptorCogs/Camera.hpp>
#include <glm/glm.hpp>

using namespace RaeptorCogs;

TEST(CameraFlagsTest, EnumValues) {
    EXPECT_EQ(static_cast<uint32_t>(CameraFlags::NONE), 0);
    EXPECT_EQ(static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_VIEW_MATRIX), 1);
    EXPECT_EQ(static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX), 2);
}

TEST(CameraFlagsTest, BitwiseOperations) {
    CameraFlags combined = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX | 
                           CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
    EXPECT_EQ(static_cast<uint32_t>(combined), 3); // 1 | 2 = 3
}

TEST(CameraFlagsTest, NoneFlag) {
    CameraFlags none = CameraFlags::NONE;
    EXPECT_EQ(static_cast<uint32_t>(none), 0);
}

TEST(CameraFlagsTest, IndividualFlags) {
    CameraFlags view = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX;
    CameraFlags projection = CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
    
    EXPECT_NE(static_cast<uint32_t>(view), static_cast<uint32_t>(projection));
}

TEST(CameraFlagsTest, CombinedFlags) {
    CameraFlags both = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX | 
                       CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
    
    // Should be able to test individual flags in combination
    uint32_t value = static_cast<uint32_t>(both);
    EXPECT_TRUE((value & static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_VIEW_MATRIX)) != 0);
    EXPECT_TRUE((value & static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX)) != 0);
}

TEST(CameraFlagsTest, FlagOperations) {
    CameraFlags flag = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX;
    
    // Test bitwise AND with itself
    CameraFlags result = flag & flag;
    EXPECT_EQ(static_cast<uint32_t>(result), static_cast<uint32_t>(flag));
}

TEST(CameraFlagsTest, FlagXOR) {
    CameraFlags flag1 = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX;
    CameraFlags flag2 = CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
    
    CameraFlags result = flag1 ^ flag2;
    EXPECT_EQ(static_cast<uint32_t>(result), 3); // 1 ^ 2 = 3
}

TEST(CameraFlagsTest, PowerOfTwo) {
    // All flags should be powers of 2
    uint32_t view = static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_VIEW_MATRIX);
    uint32_t proj = static_cast<uint32_t>(CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX);
    
    // Check if power of 2 (only one bit set)
    EXPECT_TRUE((view & (view - 1)) == 0 && view != 0);
    EXPECT_TRUE((proj & (proj - 1)) == 0 && proj != 0);
}

TEST(CameraFlagsTest, FlagComparison) {
    EXPECT_NE(CameraFlags::NONE, CameraFlags::NEEDS_REBUILD_VIEW_MATRIX);
    EXPECT_NE(CameraFlags::NONE, CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX);
    EXPECT_NE(CameraFlags::NEEDS_REBUILD_VIEW_MATRIX, CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX);
}

// Tests for matrix identity
TEST(CameraMatrixTest, IdentityMatrix) {
    glm::mat4 identity = glm::mat4(1.0f);
    
    EXPECT_FLOAT_EQ(identity[0][0], 1.0f);
    EXPECT_FLOAT_EQ(identity[1][1], 1.0f);
    EXPECT_FLOAT_EQ(identity[2][2], 1.0f);
    EXPECT_FLOAT_EQ(identity[3][3], 1.0f);
    
    EXPECT_FLOAT_EQ(identity[0][1], 0.0f);
    EXPECT_FLOAT_EQ(identity[1][0], 0.0f);
}

TEST(CameraMatrixTest, MatrixMultiplication) {
    glm::mat4 m1 = glm::mat4(1.0f);
    glm::mat4 m2 = glm::mat4(2.0f);
    
    glm::mat4 result = m1 * m2;
    
    // Identity * 2 * Identity should give diagonal 2s
    EXPECT_NE(result, m1);
}

TEST(CameraMatrixTest, MatrixEquality) {
    glm::mat4 m1 = glm::mat4(1.0f);
    glm::mat4 m2 = glm::mat4(1.0f);
    
    EXPECT_EQ(m1, m2);
}

TEST(Camera2DVectorTest, Vec2Construction) {
    glm::vec2 pos(10.0f, 20.0f);
    
    EXPECT_FLOAT_EQ(pos.x, 10.0f);
    EXPECT_FLOAT_EQ(pos.y, 20.0f);
}

TEST(Camera2DVectorTest, Vec2Operations) {
    glm::vec2 v1(1.0f, 2.0f);
    glm::vec2 v2(3.0f, 4.0f);
    
    glm::vec2 sum = v1 + v2;
    EXPECT_FLOAT_EQ(sum.x, 4.0f);
    EXPECT_FLOAT_EQ(sum.y, 6.0f);
    
    glm::vec2 diff = v2 - v1;
    EXPECT_FLOAT_EQ(diff.x, 2.0f);
    EXPECT_FLOAT_EQ(diff.y, 2.0f);
}

TEST(Camera2DVectorTest, Vec2Scaling) {
    glm::vec2 v(2.0f, 3.0f);
    glm::vec2 scaled = v * 2.0f;
    
    EXPECT_FLOAT_EQ(scaled.x, 4.0f);
    EXPECT_FLOAT_EQ(scaled.y, 6.0f);
}

TEST(Camera2DVectorTest, Vec2Zero) {
    glm::vec2 zero(0.0f, 0.0f);
    
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);
}
