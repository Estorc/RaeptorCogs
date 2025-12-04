#include <gtest/gtest.h>
#include <RaeptorCogs/Vertex.hpp>
#include <glm/gtc/epsilon.hpp>

using namespace RaeptorCogs;

TEST(Vertex2DTest, DefaultConstruction) {
    Vertex2D vertex{};
    EXPECT_EQ(vertex.position.x, 0.0f);
    EXPECT_EQ(vertex.position.y, 0.0f);
    EXPECT_EQ(vertex.uv.x, 0.0f);
    EXPECT_EQ(vertex.uv.y, 0.0f);
}

TEST(Vertex2DTest, ConstructionWithValues) {
    Vertex2D vertex{
        glm::vec2(10.5f, 20.5f),
        glm::vec2(0.5f, 0.75f)
    };
    
    EXPECT_FLOAT_EQ(vertex.position.x, 10.5f);
    EXPECT_FLOAT_EQ(vertex.position.y, 20.5f);
    EXPECT_FLOAT_EQ(vertex.uv.x, 0.5f);
    EXPECT_FLOAT_EQ(vertex.uv.y, 0.75f);
}

TEST(Vertex2DTest, PositionManipulation) {
    Vertex2D vertex;
    vertex.position = glm::vec2(5.0f, 15.0f);
    
    EXPECT_FLOAT_EQ(vertex.position.x, 5.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, 15.0f);
}

TEST(Vertex2DTest, UVManipulation) {
    Vertex2D vertex;
    vertex.uv = glm::vec2(0.25f, 0.75f);
    
    EXPECT_FLOAT_EQ(vertex.uv.x, 0.25f);
    EXPECT_FLOAT_EQ(vertex.uv.y, 0.75f);
}

TEST(Vertex2DTest, NegativeCoordinates) {
    Vertex2D vertex{
        glm::vec2(-10.0f, -20.0f),
        glm::vec2(-0.5f, -0.25f)
    };
    
    EXPECT_FLOAT_EQ(vertex.position.x, -10.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, -20.0f);
    EXPECT_FLOAT_EQ(vertex.uv.x, -0.5f);
    EXPECT_FLOAT_EQ(vertex.uv.y, -0.25f);
}

TEST(Vertex3DTest, DefaultConstruction) {
    Vertex3D vertex{};
    EXPECT_EQ(vertex.position.x, 0.0f);
    EXPECT_EQ(vertex.position.y, 0.0f);
    EXPECT_EQ(vertex.position.z, 0.0f);
    EXPECT_EQ(vertex.uv.x, 0.0f);
    EXPECT_EQ(vertex.uv.y, 0.0f);
}

TEST(Vertex3DTest, ConstructionWithValues) {
    Vertex3D vertex{
        glm::vec3(10.5f, 20.5f, 30.5f),
        glm::vec2(0.5f, 0.75f)
    };
    
    EXPECT_FLOAT_EQ(vertex.position.x, 10.5f);
    EXPECT_FLOAT_EQ(vertex.position.y, 20.5f);
    EXPECT_FLOAT_EQ(vertex.position.z, 30.5f);
    EXPECT_FLOAT_EQ(vertex.uv.x, 0.5f);
    EXPECT_FLOAT_EQ(vertex.uv.y, 0.75f);
}

TEST(Vertex3DTest, PositionManipulation) {
    Vertex3D vertex;
    vertex.position = glm::vec3(1.0f, 2.0f, 3.0f);
    
    EXPECT_FLOAT_EQ(vertex.position.x, 1.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, 2.0f);
    EXPECT_FLOAT_EQ(vertex.position.z, 3.0f);
}

TEST(Vertex3DTest, UVManipulation) {
    Vertex3D vertex;
    vertex.uv = glm::vec2(0.1f, 0.9f);
    
    EXPECT_FLOAT_EQ(vertex.uv.x, 0.1f);
    EXPECT_FLOAT_EQ(vertex.uv.y, 0.9f);
}

TEST(Vertex3DTest, NegativeCoordinates) {
    Vertex3D vertex{
        glm::vec3(-5.0f, -10.0f, -15.0f),
        glm::vec2(-0.5f, -0.75f)
    };
    
    EXPECT_FLOAT_EQ(vertex.position.x, -5.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, -10.0f);
    EXPECT_FLOAT_EQ(vertex.position.z, -15.0f);
    EXPECT_FLOAT_EQ(vertex.uv.x, -0.5f);
    EXPECT_FLOAT_EQ(vertex.uv.y, -0.75f);
}

TEST(Vertex3DTest, LargeCoordinates) {
    Vertex3D vertex{
        glm::vec3(1000.0f, 2000.0f, 3000.0f),
        glm::vec2(1.5f, 2.5f)
    };
    
    EXPECT_FLOAT_EQ(vertex.position.x, 1000.0f);
    EXPECT_FLOAT_EQ(vertex.position.y, 2000.0f);
    EXPECT_FLOAT_EQ(vertex.position.z, 3000.0f);
    EXPECT_FLOAT_EQ(vertex.uv.x, 1.5f);
    EXPECT_FLOAT_EQ(vertex.uv.y, 2.5f);
}

TEST(Vertex2D3DComparison, IndependentStructures) {
    Vertex2D v2d{glm::vec2(5.0f, 10.0f), glm::vec2(0.5f, 0.5f)};
    Vertex3D v3d{glm::vec3(5.0f, 10.0f, 0.0f), glm::vec2(0.5f, 0.5f)};
    
    // Both should have same x, y, and u, v
    EXPECT_FLOAT_EQ(v2d.position.x, v3d.position.x);
    EXPECT_FLOAT_EQ(v2d.position.y, v3d.position.y);
    EXPECT_FLOAT_EQ(v2d.uv.x, v3d.uv.x);
    EXPECT_FLOAT_EQ(v2d.uv.y, v3d.uv.y);
    
    // v3d should have z coordinate
    EXPECT_FLOAT_EQ(v3d.position.z, 0.0f);
}
