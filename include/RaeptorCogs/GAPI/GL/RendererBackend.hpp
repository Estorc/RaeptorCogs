/** ********************************************************************************
 * @section GAPI_GL_RenderBackend_Overview Overview
 * @file RendererBackend.hpp
 * @brief OpenGL Renderer backend implementation.
 * @details
 * Typical use cases:
 * - Defining OpenGL-specific renderer backend implementations
 *
 * TODO: 
 * - Optimize dynamic instance data management to reduce memory usage and improve performance. (Map/Unmap or Persistent Mapping)
 * - Implement frustum culling to avoid rendering objects outside the view.
 * - Implement MSAA or FXAA for better visual quality.
 * - Add support for more complex shapes and 3D graphics.
 * - Implement a more advanced shader system to support various effects and materials.
 * - Implement a profiler to identify and address performance bottlenecks.
 * 
 * - Make texture atlas support different filtering modes (nearest, linear, mipmaps).
 * 
 * - Make the masking faster and more efficient by optimizing the shader and reducing overdraw.
 * - Maybe one day remove this abomination: "&shader == &this->maskShader" UPDATE : did it :)
 * - Add batch break indices.
 * *********************************************************************************
 * @section GAPI_GL_RendererBackend_Header Header
 * <RaeptorCogs/GAPI/GL/RendererBackend.hpp>
 ***********************************************************************************
 * @section GAPI_GL_RendererBackend_Metadata Metadata
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
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Common/Ressources/Object.hpp>
#include <RaeptorCogs/GAPI/GL/Constants.hpp>


/**
 * @brief RaeptorCogs GAPI GL namespace.
 * 
 * Contains OpenGL graphics API related classes and functions.
 * 
 * @note This namespace is used for OpenGL-specific implementations.
 * @see RaeptorCogs::GAPI::Common
 */
namespace RaeptorCogs::GAPI::GL {};

namespace RaeptorCogs::GAPI::GL {

/**
 * @brief OpenGL Renderer backend implementation.
 * 
 * Provides OpenGL-specific implementations for the renderer backend interface.
 * 
 * @note This class inherits from the common RendererBackend interface.
 * @see RaeptorCogs::GAPI::Common::RendererBackend
 */
class RendererBackend : public Common::RendererBackend {
    private:

        // ============================================================================
        //                             PRIVATE ATTRIBUTES
        // ============================================================================

        // --------------------------------------------
        //                  Modules
        // --------------------------------------------

        /**
         * @brief ImGui implementation instance.
         * 
         * Holds the OpenGL-specific ImGui implementation.
         */
        ImGuiModule imGui;

        /**
         * @brief Graphic core implementation.
         * 
         * Manages OpenGL core functionalities.
         */
        GraphicCore graphicCore;

        /**
         * @brief Render pipeline implementation.
         * 
         * Manages the OpenGL rendering pipeline.
         */
        RenderPipeline renderPipeline;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for RendererBackend.
         */
        RendererBackend() : Common::RendererBackend(),
            imGui(),
            graphicCore(*this),
            renderPipeline(*this) {}

        /**
         * @brief Destructor for RendererBackend.
         * 
         * Cleans up resources used by the RendererBackend.
         */
        ~RendererBackend() override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::initialize()
         */
        void initialize() override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::isInitialized()
         */
        bool isInitialized() const override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::getBackendType()
         */
        GraphicsBackend getBackendType() const override { return GraphicsBackend::GL; }

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::Create()
         */
        std::shared_ptr<Common::ObjectData> Create(std::type_index type) override {
            auto& map = FactoryRegistry::get();
            auto it = map.find(type);
            if (it == map.end())
                throw std::runtime_error("Type not registered");
            return it->second();
        }

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::render(Window*, int, int, int, int)
         */
        void render(Window* window, int x, int y, int width, int height) override;
        
        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::render(Texture&, int, int, int, int)
         */
        void render(Texture& texture, int x, int y, int width, int height) override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::createWindowContext()
         */
        Common::WindowContext* createWindowContext() override;

        // --------------------------------------------
        //                  Modules
        // --------------------------------------------

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::getImGuiModule()
         */
        ImGuiModule& getImGuiModule() override { return this->imGui; }

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::getGraphicCore()
         */
        GraphicCore& getGraphicCore() override { return this->graphicCore; }

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::getRenderPipeline()
         */
        RenderPipeline& getRenderPipeline() override { return this->renderPipeline; }
};

}