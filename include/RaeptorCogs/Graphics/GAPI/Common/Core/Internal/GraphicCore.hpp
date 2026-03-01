/** ********************************************************************************
 * @section GAPI_Common_GraphicCore_Overview Overview
 * @file GraphicCore.hpp
 * @brief Graphic core interface.
 * @details
 * Typical use cases:
 * - Providing core graphics functionality and resource management
 * *********************************************************************************
 * @section GAPI_Common_GraphicCore_Header Header
 * <RaeptorCogs/Graphics/GAPI/Common/Core/Internal/GraphicCore.hpp>
 ***********************************************************************************
 * @section GAPI_Common_GraphicCore_Metadata Metadata
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
#include <RaeptorCogs/Core/Region.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Core/InstanceData.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Shader.hpp>
#include <RaeptorCogs/Graphics/Window.hpp>
#include <RaeptorCogs/IO/Texture.hpp>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Maximum number of sprites supported.
 */
constexpr int MAX_SPRITES = 2000000;

/**
 * @brief Width of the instance data texture.
 */
constexpr int INSTANCE_SIZE = 32;

/**
 * @brief Default size of the dynamic instance data buffer.
 */
constexpr int DEFAULT_DYNAMIC_DATA_BUFFER_SIZE = (MAX_SPRITES * INSTANCE_SIZE);

/**
 * @brief Size of the static instance data buffer.
 */
constexpr int STATIC_INSTANCE_DATA_SIZE = (MAX_SPRITES * sizeof(Common::StaticInstanceData));

class RendererBackend;
/**
 * @brief Graphic core interface.
 *
 * Provides core graphics functionality and resource management.
 */
class GraphicCore {
  private:
    // ============================================================================
    //                             PRIVATE MEMBERS
    // ============================================================================

    /**
     * @brief Reference to the renderer backend.
     *
     * Holds a reference to the renderer backend singleton.
     */
    RendererBackend &renderer;

    // --------------------------------------------
    //              Graphic resources
    // --------------------------------------------

    /**
     * @brief Main shader program.
     *
     * Holds the main shader for rendering.
     */
    ObjectHandler<Common::Shader> mainShader;

    /**
     * @brief Mask shader program.
     *
     * Holds the shader for rendering masks.
     */
    ObjectHandler<Common::Shader> maskShader;

    /**
     * @brief SSBO for static instance data.
     *
     * Holds static instance data for rendering.
     */
    ObjectHandler<SSBO> staticInstanceDataSSBO;

    /**
     * @brief SSBO for dynamic instance data.
     *
     * Holds dynamic instance data for rendering.
     */
    ObjectHandler<SSBO> dynamicInstanceDataSSBO;

    /**
     * @brief Quad Vertex Buffer Object (VBO).
     *
     * Holds the VBO for rendering a quad.
     */
    ObjectHandler<VBO> quadVBO;

    /**
     * @brief Quad Element Buffer Object (EBO).
     *
     * Holds the EBO for rendering a quad.
     */
    ObjectHandler<EBO> quadEBO;

    // --------------------------------------------
    //           Instance data buffers
    // --------------------------------------------

    /**
     * @brief Instance data structure.
     *
     * Holds static and dynamic instance data for rendering.
     */
    InstanceData instanceData;

    /**
     * @brief Instance allocator.
     *
     * Manages allocation of instance data.
     */
    InstanceAllocator instanceAllocator { instanceData };

    /**
     * @brief Instance uploader.
     *
     * Handles uploading of instance data to the GPU.
     */
    InstanceUploader instanceUploader { instanceData };

  protected:
    // ============================================================================
    //                             PROTECTED METHODS
    // ============================================================================

    /**
     * @brief Get the renderer pointer.
     *
     * @return Reference to the renderer backend singleton.
     */
    RendererBackend &getRenderer() const {
      return this->renderer;
    }

  public:
    // ============================================================================
    //                             PUBLIC METHODS
    // ============================================================================

    /**
     * @brief Default constructor.
     *
     * Initializes the GraphicCore.
     */
    GraphicCore(RendererBackend &renderer) : renderer(renderer) {}

    /**
     * @brief Virtual destructor.
     *
     * Cleans up resources used by the GraphicCore.
     */
    virtual ~GraphicCore() = default;

    /**
     * @brief Update the GPU data for graphics.
     *
     * @note Uploads instance data and order indices to the GPU.
     */
    void updateGraphicGPUData();

    /**
     * @brief Get the instance data.
     *
     * @return Reference to the instance data.
     */
    InstanceData &getInstanceData() {
      return this->instanceData;
    }

    /**
     * @brief Get the instance allocator.
     *
     * @return Reference to the instance allocator.
     */
    InstanceAllocator &getInstanceAllocator() {
      return this->instanceAllocator;
    }

    /**
     * @brief Get the instance uploader.
     *
     * @return Reference to the instance uploader.
     */
    InstanceUploader &getInstanceUploader() {
      return this->instanceUploader;
    }

    /**
     * @brief Get the main shader program.
     */
    ObjectHandler<Common::Shader> &getMainShader() {
      return this->mainShader;
    }

    /**
     * @brief Get the mask shader program.
     */
    ObjectHandler<Common::Shader> &getMaskShader() {
      return this->maskShader;
    }

    // ------------------------------------------------------------------------
    //                   Instance data SSBO accessors
    // ------------------------------------------------------------------------

    /**
     * @brief Get the static instance data SSBO.
     *
     * @return Reference to the static instance data SSBO.
     */
    ObjectHandler<SSBO> &getStaticInstanceDataSSBO();

    /**
     * @brief Get the dynamic instance data SSBO.
     *
     * @return Reference to the dynamic instance data SSBO.
     */
    ObjectHandler<SSBO> &getDynamicInstanceDataSSBO();

    // ------------------------------------------------------------------------
    //                         Buffers accessors
    // ------------------------------------------------------------------------

    /**
     * @brief Get the quad VBO.
     *
     * @return Reference to the quad VBO.
     */
    ObjectHandler<VBO> &getQuadVBO();

    /**
     * @brief Get the quad EBO.
     *
     * @return Reference to the quad EBO.
     */
    ObjectHandler<EBO> &getQuadEBO();

    // ------------------------------------------------------------------------
    //                      State machine methods
    // ------------------------------------------------------------------------

    /**
     * @brief Set the viewport dimensions.
     *
     * @param x X position of the viewport.
     * @param y Y position of the viewport.
     * @param width Width of the viewport.
     * @param height Height of the viewport.
     *
     * @note Configures the rendering viewport.
     */
    virtual void setViewport(int x, int y, int width, int height) = 0;

    /**
     * @brief Clear the swapchain buffers.
     *
     * @note Clears color, depth, and stencil buffers.
     */
    virtual void clearSwapchainBuffers() = 0;

    /**
     * @brief Use blending for rendering.
     *
     * @note Enables blending state in the graphics pipeline.
     */
    virtual void useBlend() = 0;

    /**
     * @brief Bind the mask texture.
     *
     * @note Binds the mask texture to the appropriate texture unit.
     */
    virtual void bindMaskTexture() = 0;

    /**
     * @brief Set the texture uniform in shaders.
     *
     * @note Updates the shader uniform for the main texture.
     */
    virtual void setTextureUniform(ObjectHandler<Common::Shader> shader) = 0;

    /**
     * @brief Set the mask texture uniform in shaders.
     *
     * @note Updates the shader uniform for the mask texture.
     */
    virtual void setMaskTextureUniform(ObjectHandler<Common::Shader> shader) = 0;

    /**
     * @brief Draw elements with instancing, base vertex, and base instance.
     *
     * @param count Number of elements to draw.
     * @param instanceCount Number of instances to draw.
     * @param first Index of the first element to draw.
     * @param baseVertex Base vertex offset.
     * @param baseInstance Base instance offset.
     *
     * @note Issues a draw call with advanced parameters.
     */
    virtual void drawElementsInstancedBaseVertexBaseInstance(
        size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) = 0;

    /**
     * @brief Bind a graphic texture for rendering.
     *
     * @param graphic Graphic to bind.
     *
     * @note Binds the specified texture for use in rendering.
     */
    virtual void bindGraphicTexture(Graphic2D &graphic) = 0;

    /**
     * @brief Enable stencil guarding.
     *
     * @note Activates stencil buffer operations for guarding.
     */
    virtual void enableStencilGuarding() = 0;

    /**
     * @brief Get the maximum upload chunk size.
     *
     * @return Maximum size of upload chunks.
     *
     * @note Useful for managing data uploads to the GPU in WebGL.
     */
    virtual size_t uploadChunkMaxSize() {
      return 0;
    }

    /**
     * @brief Set the render target.
     *
     * @note Configures the render target for offscreen rendering.
     */
    virtual void setRenderTarget(Window *window, int x, int y, int width, int height) = 0;

    /**
     * @brief Reset to the default render target.
     *
     * @note Switches back to the default framebuffer for rendering.
     */
    virtual void setRenderTarget(Texture texture, int x, int y, int width, int height) = 0;

    /**
     * @brief Clear the color buffer with specified color.
     * @param r Red component (0.0 to 1.0).
     * @param g Green component (0.0 to 1.0).
     * @param b Blue component (0.0 to 1.0).
     * @param a Alpha component (0.0 to 1.0).
     *
     * @note Clears the color buffer to the specified RGBA color.
     */
    virtual void clearColorBuffer(float r, float g, float b, float a) = 0;
};

} // namespace RaeptorCogs::GAPI::Common