/** ********************************************************************************
 * @section GAPI_WebGL_RenderBackend_Overview Overview
 * @file RendererBackend.hpp
 * @brief WebGL Renderer backend implementation.
 * @details
 * Typical use cases:
 * - Defining WebGL-specific renderer backend implementations
 * *********************************************************************************
 * @section GAPI_WebGL_RendererBackend_Header Header
 * <RaeptorCogs/Graphics/GAPI/WebGL/RendererBackend.hpp>
 ***********************************************************************************
 * @section GAPI_WebGL_RendererBackend_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Constants.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/Graphics/Renderer.hpp>

/**
 * @brief RaeptorCogs GAPI WebGL namespace.
 *
 * Contains WebGL graphics API related classes and functions.
 *
 * @note This namespace is used for WebGL-specific implementations.
 * @see RaeptorCogs::GAPI::Common
 */
namespace RaeptorCogs::GAPI::WebGL {};

namespace RaeptorCogs::GAPI::WebGL {

/**
 * @brief WebGL Renderer backend implementation.
 *
 * Provides WebGL-specific implementations for the renderer backend interface.
 *
 * @note This class inherits from the common RendererBackend interface.
 * @see RaeptorCogs::GAPI::GL::RendererBackend
 */
class RendererBackend : public GL::RendererBackend {
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
     * Holds the WebGL-specific ImGui implementation.
     */
    ImGuiModule imGui;

    /**
     * @brief Graphic core implementation.
     *
     * Manages WebGL core functionalities.
     */
    GraphicCore graphicCore;

    /**
     * @brief Render pipeline implementation.
     *
     * Manages the WebGL rendering pipeline.
     */
    RenderPipeline renderPipeline;

  public:
    // ============================================================================
    //                             PUBLIC METHODS
    // ============================================================================

    /**
     * @brief Constructor for RendererBackend.
     */
    RendererBackend() : GL::RendererBackend(), imGui(), graphicCore(*this), renderPipeline(*this) {}

    /**
     * @brief Destructor for RendererBackend.
     *
     * Cleans up resources used by the RendererBackend.
     */
    ~RendererBackend() override = default;

    /**
     * @see RaeptorCogs::GAPI::Common::RendererBackend::Create()
     */
    Common::ObjectData *Create(std::type_index type) override {
      auto &map = FactoryRegistry::get();
      auto it   = map.find(type);
      if (it == map.end()) throw std::runtime_error("Type not registered");
      return it->second();
    }

    // --------------------------------------------
    //                  Modules
    // --------------------------------------------

    /**
     * @see RaeptorCogs::GAPI::GL::RendererBackend::getImGuiModule()
     */
    ImGuiModule &getImGuiModule() override {
      return this->imGui;
    }

    /**
     * @see RaeptorCogs::GAPI::GL::RendererBackend::getGraphicCore()
     */
    GraphicCore &getGraphicCore() override {
      return this->graphicCore;
    }

    /**
     * @see RaeptorCogs::GAPI::GL::RendererBackend::getRenderPipeline()
     */
    RenderPipeline &getRenderPipeline() override {
      return this->renderPipeline;
    }
};

} // namespace RaeptorCogs::GAPI::WebGL