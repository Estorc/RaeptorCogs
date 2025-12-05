/** ********************************************************************************
 * @section GAPI_Common_Core_Buffer_Overview Overview
 * @file Buffer.hpp
 * @brief Buffer interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for different buffer types
 * *********************************************************************************
 * @section GAPI_Common_Core_Buffer_Header Header
 * <RaeptorCogs/GAPI/Common/Resources/Buffer.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_Buffer_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Buffer interface.
 * 
 * Provides a common interface for different buffer types.
 * 
 * @note This is an abstract base class and should be inherited by specific buffer implementations.
 */
class Buffer : public ObjectData {
    //
};

// -------------------------------------------------------------
//                         Framebuffer
// -------------------------------------------------------------

/**
 * @brief Framebuffer class.
 * 
 * Represents a framebuffer object and provides utilities for handling it.
 * 
 * @note This is a derived class from Buffer. This is an abstract representation; 
 *       specific implementations should provide concrete functionality.
 */
class Framebuffer : public Buffer {
    //
};

/**
 * @brief Type aliases for common buffer types.
 * 
 * These aliases provide convenient names for commonly used buffer types.
 */
using FBO = Framebuffer;

// -------------------------------------------------------------
//                         Renderbuffer
// -------------------------------------------------------------

/**
 * @brief Renderbuffer class.
 * 
 * Represents a renderbuffer object and provides utilities for handling it.
 * 
 * @note This is a derived class from Buffer. This is an abstract representation; 
 *       specific implementations should provide concrete functionality.
 */
class Renderbuffer : public Buffer {
    //
};

/**
 * @brief Type alias for Renderbuffer.
 * 
 * Provides a convenient name for the Renderbuffer type.
 */
using RBO = Renderbuffer;

// -------------------------------------------------------------
//                        Vertexbuffer
// -------------------------------------------------------------

/**
 * @brief Vertexbuffer class.
 * 
 * Represents a vertex buffer object and provides utilities for handling it.
 * 
 * @note This is a derived class from Buffer. This is an abstract representation; 
 *       specific implementations should provide concrete functionality.
 */
class Vertexbuffer : public Buffer {
    //
};

/**
 * @brief Type alias for Vertexbuffer.
 * 
 * Provides a convenient name for the Vertexbuffer type.
 */
using VBO = Vertexbuffer;

// -------------------------------------------------------------
//                        Elementbuffer
// -------------------------------------------------------------

/**
 * @brief Elementbuffer class.
 * 
 * Represents an element buffer object and provides utilities for handling it.
 * 
 * @note This is a derived class from Buffer. This is an abstract representation; 
 *       specific implementations should provide concrete functionality.
 */
class Elementbuffer : public Buffer {
    //
};

/**
 * @brief Type alias for Elementbuffer.
 * 
 * Provides a convenient name for the Elementbuffer type.
 */
using EBO = Elementbuffer;

// -------------------------------------------------------------
//                      ShaderStorageBuffer
// -------------------------------------------------------------

/**
 * @brief ShaderStorageBuffer class.
 * 
 * Represents a shader storage buffer object and provides utilities for handling it.
 * 
 * @note This is a derived class from Buffer. This is an abstract representation; 
 *       specific implementations should provide concrete functionality.
 */
class ShaderStorageBuffer : public Buffer {
    //
};

/**
 * @brief Type alias for ShaderStorageBuffer.
 * 
 * Provides a convenient name for the ShaderStorageBuffer type.
 */
using SSBO = ShaderStorageBuffer;

}