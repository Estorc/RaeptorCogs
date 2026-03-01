/** ********************************************************************************
 * @section GAPI_WebGL_GraphicCore_Overview Overview
 * @file GraphicCore.hpp
 * @brief Graphic core interface.
 * @details
 * Typical use cases:
 * - Providing WebGL-specific core graphics functionality and resource management
 * *********************************************************************************
 * @section GAPI_WebGL_GraphicCore_Header Header
 * <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/GraphicCore.hpp>
 ***********************************************************************************
 * @section GAPI_WebGL_GraphicCore_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Buffer.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Shader.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/TextureData.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/VertexArray.hpp>

namespace RaeptorCogs::GAPI::WebGL {
#include <RaeptorCogsShaders/common/constants.glsl>

/**
 * @brief Height of the instance data texture.
 */
constexpr int IDATATEX_HEIGHT =
    (Common::MAX_SPRITES * Common::INSTANCE_SIZE + 16 * IDATATEX_WIDTH - 1) / (16 * IDATATEX_WIDTH);

class GraphicCore : public GL::GraphicCore {
  public:
    // ============================================================================
    //                             PUBLIC METHODS
    // ============================================================================

    /**
     * @brief Constructor for GraphicCore.
     *
     * Initializes the GraphicCore with a reference to the renderer backend.
     *
     * @param renderer Reference to the renderer backend.
     */
    GraphicCore(Common::RendererBackend &renderer) : GL::GraphicCore(renderer) {}

    /**
     * @brief Initialize graphics resources.
     *
     * Sets up graphics resources such as shaders, buffers, and framebuffers.
     *
     * @note Called during renderer backend initialization.
     */
    void initGraphics() override;

    /**
     * @see GL::GraphicCore::buildShaders
     */
    void buildShaders() override;

    // ------------------------------------------------------------------------
    //                      State machine methods
    // ------------------------------------------------------------------------

    /**
     * @see Common::GraphicCore::bindMaskTexture
     */
    void bindMaskTexture() override;

    /**
     * @see Common::GraphicCore::setTextureUniform
     */
    void setTextureUniform(ObjectHandler<Common::Shader> shader) override;

    /**
     * @see Common::GraphicCore::setMaskTextureUniform
     */
    void setMaskTextureUniform(ObjectHandler<Common::Shader> shader) override;

    /**
     * @see Common::GraphicCore::drawElementsInstancedBaseVertexBaseInstance
     */
    void drawElementsInstancedBaseVertexBaseInstance(
        size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) override;

    /**
     * @see Common::GraphicCore::bindGraphicTexture
     */
    void bindGraphicTexture(Graphic2D &graphic) override;

    /**
     * @see Common::GraphicCore::uploadChunkMaxSize
     */
    size_t uploadChunkMaxSize() override {
      return 100;
    }
};

} // namespace RaeptorCogs::GAPI::WebGL