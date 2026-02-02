/** ********************************************************************************
 * @section GAPI_Common_WebGL_Buffer_Overview Overview
 * @file Buffer.hpp
 * @brief Buffer interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for different buffer types
 * *********************************************************************************
 * @section GAPI_Common_WebGL_Buffer_Header Header
 * <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Buffer.hpp>
 ***********************************************************************************
 * @section GAPI_Common_WebGL_Buffer_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/Buffer.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Object.hpp>

namespace RaeptorCogs::GAPI::WebGL {

// -------------------------------------------------------------
//                        Framebuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Framebuffer
 */
class Framebuffer : public GL::Framebuffer {};

/**
 * @see RaeptorCogs::GAPI::Common::FBO
 */
using FBO = Framebuffer;

/** @brief Register Framebuffer with the FactoryRegistry.*/
REGISTER(Common::Framebuffer, Framebuffer);

// -------------------------------------------------------------
//                        Renderbuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Renderbuffer
 */
class Renderbuffer : public GL::Renderbuffer {};

/**
 * @see RaeptorCogs::GAPI::Common::RBO
 */
using RBO = Renderbuffer;

/** @brief Register Renderbuffer with the FactoryRegistry.*/
REGISTER(Common::Renderbuffer, Renderbuffer);

// -------------------------------------------------------------
//                        Vertexbuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Vertexbuffer
 */
class Vertexbuffer : public GL::Vertexbuffer {};

/**
 * @see RaeptorCogs::GAPI::Common::VBO
 */
using VBO = Vertexbuffer;

/** @brief Register Vertexbuffer with the FactoryRegistry.*/
REGISTER(Common::Vertexbuffer, Vertexbuffer);

// -------------------------------------------------------------
//                       Elementbuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Elementbuffer
 */
class Elementbuffer : public GL::Elementbuffer {};

/**
 * @see RaeptorCogs::GAPI::Common::EBO
 */
using EBO = Elementbuffer;

/** @brief Register Elementbuffer with the FactoryRegistry.*/
REGISTER(Common::Elementbuffer, Elementbuffer);

// -------------------------------------------------------------
//                     ShaderStorageBuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::ShaderStorageBuffer
 */
class ShaderStorageBuffer : public Common::ShaderStorageBuffer {
    private:
        std::shared_ptr<GLuint> doubleBuffer = 0;
        size_t bindingPoint = 0;
        size_t height = 0;
    public:
        /**
         * @brief Initialize the shader storage buffer.
         * 
         * Initializes the OpenGL shader storage buffer object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        virtual void initialize() override;

        /**
         * @brief Bind the shader storage buffer for use.
         * 
         * Binds the OpenGL shader storage buffer object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void bind() override;

        /**
         * @brief Unbind the shader storage buffer.
         * 
         * Unbinds the OpenGL shader storage buffer object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void unbind() const override;

        /**
         * @see RaeptorCogs::GAPI::Common::ShaderStorageBuffer::allocate
         */
        void allocate(size_t size) override;

        /**
         * @see RaeptorCogs::GAPI::Common::ShaderStorageBuffer::setBindingPoint
         */
        void setBindingPoint(unsigned int bindingPoint) override;

        /**
         * @see RaeptorCogs::GAPI::Common::ShaderStorageBuffer::upload
         */
        void upload(const void* data, size_t size, size_t offset = 0) override;
};

/**
 * @see RaeptorCogs::GAPI::Common::SSBO
 */
using SSBO = ShaderStorageBuffer;

/** @brief Register ShaderStorageBuffer with the FactoryRegistry.*/
REGISTER(Common::ShaderStorageBuffer, ShaderStorageBuffer);

}