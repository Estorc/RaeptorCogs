/** ********************************************************************************
 * @section GAPI_GL_Constants_Overview Overview
 * @file Constants.hpp
 * @brief OpenGL constants definitions.
 * @details
 * Typical use cases:
 * - Defining OpenGL constants for use in graphics programming
 * *********************************************************************************
 * @section GAPI_GL_Constants_Header Header
 * <RaeptorCogs/GAPI/GL/Constants.hpp>
 ***********************************************************************************
 * @section GAPI_GL_Constants_Metadata Metadata
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

/**
 * @brief GL enum definition.
 * 
 * Defines GLenum type for OpenGL programming.
 */
using GLenum = unsigned int;

/**
 * @brief GLuint definition.
 * 
 * Defines GLuint type for OpenGL programming.
 */
using GLuint = unsigned int;

/**
 * @brief GLint definition.
 * 
 * Defines GLint type for OpenGL programming.
 */
using GLint = int;

/**
 * @brief GL texture linear filtering mode constant.
 * 
 * Linear filtering mode for texture sampling in OpenGL.
 */
constexpr unsigned int GL_LINEAR               = 0x2601;

/**
 * @brief GL texture mipmap linear filtering mode constant.
 * 
 * Linear mipmap filtering mode for texture sampling in OpenGL.
 */
constexpr unsigned int GL_LINEAR_MIPMAP_LINEAR = 0x2703;

/**
 * @brief GL texture mipmap nearest filtering mode constant.
 * 
 * Nearest mipmap filtering mode for texture sampling in OpenGL.
 */
constexpr unsigned int GL_LINEAR_MIPMAP_NEAREST = 0x2701;