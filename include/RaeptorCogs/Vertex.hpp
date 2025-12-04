/** ********************************************************************************
 * @section Vertex_Overview Overview
 * @file Vertex.hpp
 * @brief Vertex data structures for 2D and 3D graphics.
 * @details
 * Typical use cases:
 * - Defining vertex attributes for rendering.
 * *********************************************************************************
 * @section Vertex_Header Header
 * <RaeptorCogs/Vertex.hpp>
 ***********************************************************************************
 * @section Vertex_Metadata Metadata
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace RaeptorCogs {

/**
 * @brief Vertex structure for 2D graphics.
 * 
 * Defines the layout of vertex data used in rendering.
 */
struct Vertex2D {
    /**
     * @brief Position of the vertex in 2D space.
     * 
     * Uses glm::vec2 to represent the x and y coordinates.
     */
    glm::vec2 position;

    /**
     * @brief Texture coordinates (UV) of the vertex.
     * 
     * Uses glm::vec2 to represent the u and v texture mapping coordinates.
     */
    glm::vec2 uv;
};

/**
 * @brief Vertex structure for 3D graphics.
 * 
 * Defines the layout of a 3D vertex with position and texture coordinates.
 */
struct Vertex3D {
    /**
     * @brief Position of the vertex in 3D space.
     * 
     * Uses glm::vec3 to represent the x, y, and z coordinates.
     */
    glm::vec3 position;

    /**
     * @brief Texture coordinates (UV) of the vertex.
     * 
     * Uses glm::vec2 to represent the u and v texture mapping coordinates.
     */
    glm::vec2 uv;
};

}