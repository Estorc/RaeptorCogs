/** ********************************************************************************
 * @section GAPI_Vulkan_RendererBackend_Overview Overview
 * @file RendererBackend.hpp
 * @brief Vulkan Renderer backend implementation.
 * @details
 * Typical use cases:
 * - Providing Vulkan-specific implementations for the renderer backend interface
 * *********************************************************************************
 * @section GAPI_Vulkan_RendererBackend_Header Header
 * <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_RendererBackend_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/Object.hpp>

/**
 * @brief RaeptorCogs GAPI Vulkan namespace.
 * 
 * Contains Vulkan graphics API related classes and functions.
 * 
 * @note This namespace is used for Vulkan-specific implementations.
 * @see RaeptorCogs::GAPI::Common
 */
namespace RaeptorCogs::GAPI::Vulkan {};

namespace RaeptorCogs::GAPI::Vulkan {

/**
 * @brief Vulkan Renderer backend implementation.
 * 
 * Provides Vulkan-specific implementations for the renderer backend interface.
 */
class RendererBackend : public Common::RendererBackend {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        // --------------------------------------------
        //                  Modules
        // --------------------------------------------

        /**
         * @brief ImGui implementation for Vulkan.
         * 
         * Handles ImGui rendering in Vulkan.
         */
        ImGuiModule imGui;

        /**
         * @brief Graphic core instance.
         * 
         * Manages Vulkan core functionalities.
         */
        GraphicCore graphicCore;

        /**
         * @brief Render pipeline instance.
         * 
         * Manages the Vulkan rendering pipeline.
         */
        RenderPipeline renderPipeline;


    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for RendererBackend.
         * 
         * Initializes the Vulkan renderer backend.
         */
        RendererBackend() : Common::RendererBackend(),
            imGui(),
            graphicCore(*this),
            renderPipeline(*this) {}

        /**
         * @brief Destructor for RendererBackend.
         */
        ~RendererBackend() override = default;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::isInitialized()
         */
        bool isInitialized() const override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::getBackendType()
         */
        GraphicsBackend getBackendType() const override { return GraphicsBackend::Vulkan; }

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
         * @see RaeptorCogs::GAPI::Common::RendererBackend::initialize()
         */
        void initialize() override;

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