/** ********************************************************************************
 * @section GAPI_GL_GraphicCore_Overview Overview
 * @file GraphicCore.hpp
 * @brief Graphic core interface.
 * @details
 * Typical use cases:
 * - Providing OpenGL-specific core graphics functionality and resource management
 * *********************************************************************************
 * @section GAPI_GL_GraphicCore_Header Header
 * <RaeptorCogs/GAPI/GL/Core/Internal/GraphicCore.hpp>
 ***********************************************************************************
 * @section GAPI_GL_GraphicCore_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/GL/Ressources/Buffer.hpp>
#include <RaeptorCogs/GAPI/GL/Ressources/TextureData.hpp>
#include <RaeptorCogs/GAPI/GL/Ressources/Shader.hpp>
#include <RaeptorCogs/GAPI/GL/Ressources/VertexArray.hpp>

namespace RaeptorCogs::GAPI::GL {

#include <RaeptorCogs/EmbedShaders.hpp>

/**
 * @brief Maximum number of sprites supported.
 */
constexpr int MAX_SPRITES = 8000000;

/**
 * @brief Width of the instance data texture.
 */
constexpr int INSTANCE_SIZE = 32;

#ifdef __EMSCRIPTEN__
/**
 * @brief Height of the instance data texture.
 */
constexpr int IDATATEX_HEIGHT = (MAX_SPRITES * INSTANCE_SIZE + 16 * IDATATEX_WIDTH - 1) / (16 * IDATATEX_WIDTH);
#else
/**
 * @brief Default size of the dynamic instance data buffer.
 */
constexpr int DEFAULT_DYNAMIC_DATA_BUFFER_SIZE = (MAX_SPRITES * INSTANCE_SIZE);
/**
 * @brief Size of the static instance data buffer.
 */
constexpr int STATIC_INSTANCE_DATA_SIZE = (MAX_SPRITES * sizeof(Common::StaticInstanceData));
#endif

class GraphicCore : public Common::GraphicCore {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        #ifdef __EMSCRIPTEN__
        /**
         * @brief Main shader program.
         * 
         * Holds the main shader for rendering.
         */
        GraphicHandler<Common::Shader> mainShader/* = Shader(__shader__main_gles_vs, __shader__main_gles_fs)*/;
        #else
        /**
         * @brief Main shader program.
         * 
         * Holds the main shader for rendering.
         */
        ObjectHandler<Common::Shader> mainShader/* = Shader(__shader__main_vs, __shader__main_fs)*/;
        
        /**
         * @brief Mask shader program.
         * 
         * Holds the shader for rendering masks.
         */
        ObjectHandler<Common::Shader> maskShader/* = Shader(__shader__main_vs, __shader__mask_fs)*/;
        #endif

        /**
         * @brief Shader Storage Buffer Object (SSBO) for static instance data.
         * 
         * Holds the SSBO for static instance data.
         */
        std::pair<ObjectHandler<Common::FBO>, FBO> pingPongFramebuffer;

        /**
         * @brief Shader Storage Buffer Object (SSBO) for dynamic instance data.
         * 
         * Holds the SSBO for dynamic instance data.
         */
        ObjectHandler<Common::FBO> textureFramebuffer;

        /**
         * @brief Ping-pong framebuffers for mask rendering.
         * 
         * Holds the pair of framebuffers used for ping-pong rendering of masks.
         */
        std::pair<ObjectHandler<Common::FBO>, ObjectHandler<Common::FBO>> pingPongMaskFramebuffer;

        /**
         * @brief Mask textures for ping-pong rendering.
         * 
         * Holds the pair of textures used for ping-pong rendering of masks.
         */
        std::pair<ObjectHandler<Common::TextureData>, ObjectHandler<Common::TextureData>> maskTextures;

        /**
         * @brief Renderbuffer for offscreen rendering.
         * 
         * Holds the renderbuffer used for offscreen rendering.
         */
        ObjectHandler<Common::RBO> renderbuffer;

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
        GraphicCore(Common::RendererBackend& renderer) : Common::GraphicCore(renderer) {}

        /**
         * @brief Initialize graphics resources.
         * 
         * Sets up graphics resources such as shaders, buffers, and framebuffers.
         * 
         * @note Called during renderer backend initialization.
         */
        void initGraphics();

        /**
         * @brief Build necessary buffers.
         * 
         * Sets up vertex buffers and attribute pointers.
         * 
         * @note Called during renderer backend initialization.
         */
        void buildBuffers();

        /**
         * @brief Get the maximum number of texture units supported by the GPU.
         * 
         * @return Maximum number of texture units.
         * 
         * @note Useful for managing texture bindings.
         */
        GLuint getMaxTextureUnits() const;

        /**
         * @brief Get the mask textures used for ping-pong rendering.
         */
        std::pair<ObjectHandler<Common::TextureData>, ObjectHandler<Common::TextureData>>& getMaskTextures() {
            return this->maskTextures;
        }

        /**
         * @brief Get the main shader program.
         */
        ObjectHandler<Common::Shader>& getMainShader() {
            return this->mainShader;
        }

        /**
         * @brief Get the mask shader program.
         */
        ObjectHandler<Common::Shader>& getMaskShader() {
            return this->maskShader;
        }

        /**
         * @brief Get the renderbuffer used for offscreen rendering.
         */
        ObjectHandler<Common::RBO>& getRenderbuffer() {
            return this->renderbuffer;
        }

        /**
         * @brief Get the ping-pong framebuffers used for mask rendering.
         */
        std::pair<ObjectHandler<Common::FBO>, ObjectHandler<Common::FBO>>& getPingPongMaskFramebuffer() {
            return this->pingPongMaskFramebuffer;
        }

        /**
         * @brief Get the texture framebuffer used for rendering to textures.
         */
        ObjectHandler<Common::FBO>& getTextureFramebuffer() {
            return this->textureFramebuffer;
        }

        // ------------------------------------------------------------------------
        //                      State machine methods
        // ------------------------------------------------------------------------
        
        /**
         * @see Common::GraphicCore::setViewport
         */
        void setViewport(int x, int y, int width, int height) override;

        /**
         * @see Common::GraphicCore::clearSwapchainBuffers
         */
        void clearSwapchainBuffers() override;

        /**
         * @see Common::GraphicCore::useBlend
         */
        void useBlend() override;

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

        #ifdef __EMSCRIPTEN__
        /**
         * @see Common::GraphicCore::setSSBOTextureUniform
         */
        void setSSBOTextureUniform(ObjectHandler<Common::Shader> shader) override;
        #endif

        /**
         * @see Common::GraphicCore::drawElementsInstancedBaseVertexBaseInstance
         */
        void drawElementsInstancedBaseVertexBaseInstance(size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) override;
        
        /**
         * @see Common::GraphicCore::bindGraphicTexture
         */
        void bindGraphicTexture(Graphic2D& graphic) override;
        
        /**
         * @see Common::GraphicCore::enableStencilGuarding
         */
        void enableStencilGuarding() override;

};

}