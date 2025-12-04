#include <RaeptorCogs/Shape.hpp>

namespace RaeptorCogs {

void Shape::getVertexData(std::vector<glm::mat3> *outTrianglesPos, std::vector<glm::mat3x2> *outTrianglesUV, size_t *triangleCount) const {
    float const* vertices = this->getVertices();
    size_t vertexCount;
    const unsigned* indices = this->getIndices(vertexCount);
    *triangleCount = vertexCount / 3;

    outTrianglesPos->clear();
    outTrianglesUV->clear();
    outTrianglesPos->reserve(*triangleCount);
    outTrianglesUV->reserve(*triangleCount);
    for (size_t i = 0; i < *triangleCount; ++i) {
        glm::mat3 triPos;
        glm::mat3x2 triUV;
        for (size_t j = 0; j < 3; ++j) {
            size_t vertexIndex = indices[i * 3 + j];
            triPos[static_cast<glm::length_t>(j)] = glm::vec3(
                vertices[vertexIndex * 4 + 0],
                vertices[vertexIndex * 4 + 1],
                0.0f
            );
            triUV[static_cast<glm::length_t>(j)] = glm::vec2(
                vertices[vertexIndex * 4 + 2],
                vertices[vertexIndex * 4 + 3]
            );
        }
        outTrianglesPos->push_back(triPos);
        outTrianglesUV->push_back(triUV);
    }
};

};