/** ********************************************************************************
 * @section GAPI_Common_Vulkan_Buffer_Overview Overview
 * @file Buffer.hpp
 * @brief Buffer interface.
 * @details
 * Typical use cases:
 * - Defining Vulkan-specific buffer implementations
 * *********************************************************************************
 * @section GAPI_Common_Vulkan_Buffer_Header Header
 * <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Buffer.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Vulkan_Buffer_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Buffer.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Object.hpp>


namespace RaeptorCogs::GAPI::Vulkan {

class Buffer {
  private:
    /**
     * @brief Vulkan buffer handle.
     */
    VkBuffer buffer = nullptr;

    /**
     * @brief Vulkan device memory handle.
     */
    VkDeviceMemory memory = nullptr;

  protected:
    /**
     * @brief Get the Vulkan buffer handle.
     *
     * @return VkBuffer& Handle to the Vulkan buffer.
     */
    VkBuffer &getBuffer();

    /**
     * @brief Get the Vulkan device memory handle.
     *
     * @return VkDeviceMemory& Handle to the Vulkan device memory.
     */
    VkDeviceMemory &getMemory();

  public:
    /**
     * @brief Constructor for Buffer.
     */
    Buffer() = default;

    /**
     * @brief Destructor for Buffer.
     */
    ~Buffer();

    /**
     * @brief Build the buffer with given data, size, and usage.
     * @param data Pointer to the data to be copied into the buffer.
     * @param size Size of the data in bytes.
     * @param usage Vulkan buffer usage flags.
     *
     * @note This is a helper function for buffer creation.
     */
    void vkBuild(const void *data, size_t size, VkBufferUsageFlags usage);
};

// -------------------------------------------------------------
//                        Framebuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Framebuffer
 */
class Framebuffer : public Common::Framebuffer, Buffer {
  public:
    /**
     * @brief Constructor for Framebuffer.
     */
    Framebuffer();

    /**
     * @brief Bind the framebuffer for use.
     *
     * Binds the Vulkan framebuffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void bind() override;

    /**
     * @brief Unbind the framebuffer.
     *
     * Unbinds the Vulkan framebuffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void unbind() const override;
};

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
class Renderbuffer : public Common::Renderbuffer, Buffer {
  public:
    /**
     * @brief Constructor for Renderbuffer.
     */
    Renderbuffer();

    /**
     * @brief Bind the renderbuffer for use.
     *
     * Binds the Vulkan renderbuffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void bind() override;

    /**
     * @brief Unbind the renderbuffer.
     *
     * Unbinds the Vulkan renderbuffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void unbind() const override;
};

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
class Vertexbuffer : public Common::Vertexbuffer, Buffer {
  public:
    /**
     * @brief Constructor for Vertexbuffer.
     */
    Vertexbuffer();

    /**
     * @brief Bind the vertex buffer for use.
     *
     * Binds the Vulkan vertex buffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void bind() override;

    /**
     * @brief Unbind the vertex buffer.
     *
     * Unbinds the Vulkan vertex buffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void unbind() const override;

    /**
     * @see RaeptorCogs::GAPI::Common::Vertexbuffer::build
     */
    void build(const void *data, size_t size) override;
};

/**
 * @see RaeptorCogs::GAPI::Common::VBO
 */
using VBO = Vertexbuffer;

/** @brief Register Vertexbuffer with the FactoryRegistry.*/
REGISTER(Common::Vertexbuffer, Vertexbuffer);

// -------------------------------------------------------------
//                     Elementbuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::Elementbuffer
 */
class Elementbuffer : public Common::Elementbuffer, Buffer {
  public:
    /**
     * @brief Constructor for Elementbuffer.
     */
    Elementbuffer();

    /**
     * @brief Bind the element buffer for use.
     *
     * Binds the Vulkan element buffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void bind() override;

    /**
     * @brief Unbind the element buffer.
     *
     * Unbinds the Vulkan element buffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void unbind() const override;

    /**
     * @see RaeptorCogs::GAPI::Common::Elementbuffer::build
     */
    void build(const void *data, size_t size) override;
};

/**
 * @see RaeptorCogs::GAPI::Common::EBO
 */
using EBO = Elementbuffer;

/** @brief Register Elementbuffer with the FactoryRegistry.*/
REGISTER(Common::Elementbuffer, Elementbuffer);

// -------------------------------------------------------------
//                   ShaderStorageBuffer
// -------------------------------------------------------------

/**
 * @see RaeptorCogs::GAPI::Common::ShaderStorageBuffer
 */
class ShaderStorageBuffer : public Common::ShaderStorageBuffer, Buffer {
  public:
    /**
     * @brief Constructor for ShaderStorageBuffer.
     */
    ShaderStorageBuffer();

    /**
     * @brief Bind the shader storage buffer for use.
     *
     * Binds the Vulkan shader storage buffer object.
     *
     * @note Overrides the pure virtual method from the base class.
     */
    void bind() override;

    /**
     * @brief Unbind the shader storage buffer.
     *
     * Unbinds the Vulkan shader storage buffer object.
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
    void upload(const void *data, size_t size, size_t offset = 0) override;
};

/**
 * @see RaeptorCogs::GAPI::Common::SSBO
 */
using SSBO = ShaderStorageBuffer;

/** @brief Register ShaderStorageBuffer with the FactoryRegistry.*/
REGISTER(Common::ShaderStorageBuffer, ShaderStorageBuffer);

} // namespace RaeptorCogs::GAPI::Vulkan