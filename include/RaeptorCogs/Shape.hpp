/** ********************************************************************************
 * @section Shape_Overview Overview
 * @file Shape.hpp
 * @brief High-level shape handling utilities.
 * @details
 * Typical use cases:
 * - Handling 2D shape definitions and vertex data retrieval.
 * 
 * TODO:
 * - Connect shape to GPU buffers for rendering.
 * *********************************************************************************
 * @section Shape_Header Header
 * <RaeptorCogs/Shape.hpp>
 ***********************************************************************************
 * @section Shape_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <iostream>
#include "../../shaders/constants.glsl"

namespace RaeptorCogs {

/**
 * @brief Base Shape class.
 * 
 * Provides an interface for 2D shapes to retrieve vertex and index data.
 */
class Shape {
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Virtual destructor.
         */
        virtual ~Shape() = default;

        /**
         * @brief Get vertex data as triangles.
         * 
         * @param outTrianglesPos Output vector for triangle positions.
         * @param outTrianglesUV Output vector for triangle UV coordinates.
         * @param triangleCount Output number of triangles.
         * 
         * @note The default implementation converts the shape's vertices and indices into triangles.
         */
        virtual void getVertexData(std::vector<glm::mat3> *outTrianglesPos, std::vector<glm::mat3x2> *outTrianglesUV, size_t *triangleCount) const;

        /**
         * @brief Get raw vertex array.
         * 
         * @return Pointer to the vertex array.
         */
        virtual const float* getVertices() const = 0;

        /**
         * @brief Get raw index array.
         * 
         * @param count Output number of indices.
         * @return Pointer to the index array.
         */
        virtual const unsigned* getIndices(size_t& count) const = 0;
};

/**
 * @brief Quad shape class.
 * 
 * Provides vertex and index data for a simple quad shape.
 * 
 * @code{.cpp}
 * std::shared_ptr<Shape> quad = std::make_shared<Quad>();
 * size_t triangleCount;
 * const float* vertices = quad->getVertices();
 * const unsigned* indices = quad->getIndices(triangleCount);
 * @endcode
 */
class Quad : public Shape {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Vertex data for the quad.
         * 
         * Each vertex consists of position (x, y) and UV coordinates (u, v).
         * 
         * @note The quad is defined in a 1x1 space from (0,0) to (1,1).
         */
        static constexpr float vertices[] = {
            // positions    uv
            0.0f, 0.0f,     0.0f, 0.0f,  // Bottom-left
            1.0f, 0.0f,     1.0f, 0.0f,  // Bottom-right
            1.0f, 1.0f,     1.0f, 1.0f,  // Top-right
            0.0f, 1.0f,     0.0f, 1.0f   // Top-left
        };

        /**
         * @brief Index data for the quad.
         * 
         * Defines two triangles that make up the quad.
         */
        static constexpr unsigned indices[] = {
            0, 1, 2,
            2, 3, 0
        };
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Destructor for Quad.
         */
        ~Quad() override = default;

        /**
         * @brief Get vertex data.
         * 
         * @return Pointer to the vertex array.
         */
        const float* getVertices() const override {
            return vertices;
        }

        /**
         * @brief Get index data.
         * 
         * @param count Output number of indices.
         * @return Pointer to the index array.
         */
        const unsigned* getIndices(size_t& count) const override {
            count = std::size(indices);
            return indices;
        }
};

/**
 * @brief Regular polygon shape class.
 * 
 * Provides vertex and index data for a regular polygon shape with a specified number of sides.
 */
class RegularPolygon : public Shape {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Number of sides of the polygon.
         * 
         * @note Minimum is 3.
         */
        unsigned int sides;

        /**
         * @brief Vertex data.
         * 
         * Generated based on the number of sides.
         */
        std::vector<float> vertices;

        /**
         * @brief Index data.
         * 
         * Generated based on the number of sides.
         */
        std::vector<unsigned> indices;

        /**
         * @brief Generate geometry for the polygon.
         * 
         * @note Called during construction to populate vertices and indices.
         */
        void generateGeometry() {
            vertices.clear();
            indices.clear();
            if (sides < 3u) sides = 3u; // Minimum of 3 sides

            // Center vertex
            vertices.push_back(0.5f); // x
            vertices.push_back(0.5f); // y
            vertices.push_back(0.5f); // u
            vertices.push_back(0.5f); // v

            float angleStep = 2.0f * static_cast<float>(PI) / static_cast<float>(sides);
            for (unsigned int i = 0; i < sides; ++i) {
                float angle = static_cast<float>(i) * angleStep;
                float x = 0.5f + 0.5f * cos(angle);
                float y = 0.5f + 0.5f * sin(angle);
                float u = x; // Simple UV mapping
                float v = y; // Simple UV mapping
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(u);
                vertices.push_back(v);
            }

            for (unsigned int i = 1; i <= sides; ++i) {
                indices.push_back(0); // Center vertex
                indices.push_back(i);
                indices.push_back(i % sides + 1);
            }
        }

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Destructor for RegularPolygon.
         */
        ~RegularPolygon() override = default;

        /**
         * @brief Constructor for RegularPolygon.
         * 
         * @param sides Number of sides of the polygon.
         */
        RegularPolygon(unsigned int sides) : sides(sides) {
            generateGeometry();
        }

        /**
         * @brief Get vertex data.
         * 
         * @return Pointer to the vertex array.
         */
        const float* getVertices() const override {
            return vertices.data();
        }

        /**
         * @brief Get index data.
         * 
         * @param count Output number of indices.
         * @return Pointer to the index array.
         */
        const unsigned* getIndices(size_t& count) const override {
            count = indices.size();
            return indices.data();
        }

};

};