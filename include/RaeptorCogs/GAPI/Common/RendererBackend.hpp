/** ********************************************************************************
 * @section GAPI_Common_RendererBackend_Overview Overview
 * @file RendererBackend.hpp
 * @brief Renderer backend interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for different graphics API backends
 * *********************************************************************************
 * @section GAPI_Common_RendererBackend_Header Header
 * <RaeptorCogs/GAPI/Common/RendererBackend.hpp>
 ***********************************************************************************
 * @section GAPI_Common_RendererBackend_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/GAPI/Common/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Common/Core/Internal/RenderPipeline.hpp>

#include <typeindex>

/**
 * @brief RaeptorCogs GAPI namespace.
 * 
 * Contains graphics API related classes and functions.
 * 
 * @note This namespace is further divided into sub-namespaces for different graphics APIs and common functionality.
 */
namespace RaeptorCogs::GAPI{};

/**
 * @brief RaeptorCogs GAPI Common namespace.
 * 
 * Contains common graphics API related classes and functions shared across different backends.
 * 
 * @note This namespace is used for defining interfaces and shared functionality.
 */
namespace RaeptorCogs::GAPI::Common{};

namespace RaeptorCogs {
    class Window;
    class Texture;

    namespace Singletons {
        class Platform;
    }

    /**
     * @brief Graphics backend enumeration.
     * 
     * Defines supported graphics backends.
     */
    enum class GraphicsBackend {
        /** OpenGL graphics backend */
        GL,
        /** Vulkan graphics backend */
        Vulkan,
    };
}

namespace RaeptorCogs::GAPI::Common {

class WindowContext;

class RendererBackend {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Platform pointer.
         * 
         * Holds a pointer to the platform singleton.
         */
        Singletons::Platform *platform = nullptr;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Virtual destructor for RendererBackend.
         */
        virtual ~RendererBackend() = default;

        /**
         * @brief Initialize the renderer backend.
         * 
         * @note Sets up necessary resources and state for rendering.
         */
        virtual void initialize() = 0;

        /**
         * @brief Check if the renderer backend is initialized.
         * 
         * @return true if initialized, false otherwise.
         */
        virtual bool isInitialized() const = 0;

        /**
         * @brief Get the graphics backend type.
         * 
         * @return GraphicsBackend enumeration value.
         */
        virtual GraphicsBackend getBackendType() const = 0;

        /**
         * @brief Create an object of the specified type.
         * 
         * @param type Type index of the object to be created.
         * @return Shared pointer to the created object data.
         * 
         * @note This is a pure virtual function and must be implemented by derived classes.
         */
        virtual std::shared_ptr<Common::ObjectData> Create(std::type_index type) = 0;

        /**
         * @brief Backend implementation for rendering to a window.
         * 
         * @param window Pointer to the target window. If nullptr, renders to the default framebuffer.
         * @param x X coordinate of the rendering area.
         * @param y Y coordinate of the rendering area.
         * @param width Width of the rendering area.
         * @param height Height of the rendering area.
         * 
         * @note Renders the scene to the specified window or default framebuffer.
         */
        virtual void render(Window* window, int x, int y, int width, int height) = 0;

        /**
         * @brief Backend implementation for rendering to a texture.
         * 
         * @param texture Reference to the target texture.
         * @param x X coordinate of the rendering area.
         * @param y Y coordinate of the rendering area.
         * @param width Width of the rendering area.
         * @param height Height of the rendering area.
         * 
         * @note Renders the scene directly into the provided texture.
         */
        virtual void render(Texture& texture, int x, int y, int width, int height) = 0;

        /**
         * @brief Create a window context.
         * 
         * @return Pointer to the created window context.
         */
        virtual Common::WindowContext* createWindowContext() = 0;

        /**
         * @brief Set the platform singleton.
         * 
         * @param platform Pointer to the platform singleton.
         */
        void setPlatform(Singletons::Platform* platform) { this->platform = platform; }

        // --------------------------------------------
        //                  Modules
        // --------------------------------------------

        /**
         * @brief Get the platform singleton.
         * 
         * @return Reference to the platform singleton.
         */
        Singletons::Platform& getPlatform() const { return *this->platform; }

        /**
         * @brief Get the ImGui implementation.
         * 
         * @return Reference to the ImGui implementation.
         */
        virtual ImGuiModule& getImGuiModule() = 0;

        /**
         * @brief Get the GraphicCore implementation.
         * 
         * @return Reference to the GraphicCore implementation.
         */
        virtual GraphicCore& getGraphicCore() = 0;

        /**
         * @brief Get the RenderPipeline implementation.
         * 
         * @return Reference to the RenderPipeline implementation.
         */
        virtual RenderPipeline& getRenderPipeline() = 0;
};

}