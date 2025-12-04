/** ********************************************************************************
 * @section Renderer_Overview Overview
 * @file Renderer.hpp
 * @brief High-level rendering utilities.
 * @details
 * Typical use cases:
 * - Managing rendering backends and pipelines
 * - Handling frame rendering and window management
 * *********************************************************************************
 * @section Renderer_Header Header
 * <RaeptorCogs/Renderer.hpp>
 ***********************************************************************************
 * @section Renderer_Metadata Metadata
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
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/Window.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <deque>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>
#include "../../shaders/constants.glsl"

#include <RaeptorCogs/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/Singleton.hpp>
#include <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>

namespace RaeptorCogs {

class Graphic2D;
class Component;

}
namespace RaeptorCogs::Singletons {

/**
 * @brief Renderer singleton class.
 * 
 * Manages the rendering backend and provides high-level rendering functionalities.
 * 
 * @code{.cpp}
 * RaeptorCogs::Renderer().initialize(RaeptorCogs::GraphicsBackend::GL); // Initialize OpenGL backend
 * RaeptorCogs::Window* window = RaeptorCogs::Renderer().createWindow(800, 600, "My Window"); // Create a window
 * RaeptorCogs::StartLoop([&]() {
 *      // Update logic here ...
 *      RaeptorCogs::Renderer().render(window); // Render to a window
 * }); // Start the main loop
 * RaeptorCogs::Renderer().destroyWindow(window); // Destroy the window
 * RaeptorCogs::Destroy(); // Cleanup RaeptorCogs and the renderer
 * @endcode
 */
class Renderer {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the active rendering backend.
         * 
         * Manages the rendering operations and backend-specific implementations.
         * 
         * @note Initialized during renderer setup.
         * @see GraphicsBackend
         */
        GAPI::Common::RendererBackend* activeBackend = nullptr;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Private constructor for the Renderer singleton.
         * 
         * Ensures that the Renderer can only be instantiated through the SingletonAccessor.
         */
        Renderer() = default;

        /**
         * @brief Destructor for the Renderer singleton.
         * 
         * Cleans up the active rendering backend.
         */
        ~Renderer();

        /**
         * @brief Set the rendering backend.
         * 
         * @param backend The graphics backend to use.
         * 
         * @note Called during initialization.
         */
        void setBackend(GraphicsBackend backend);

        friend SingletonAccessor<Renderer>;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Initialize the renderer with the specified graphics backend.
         * 
         * @param backend The graphics backend to use. Defaults to OpenGL (GL).
         * 
         * @code{.cpp}
         * RaeptorCogs::Renderer().initialize(RaeptorCogs::GraphicsBackend::GL); // Initialize OpenGL backend
         * @endcode
         * @note This method must be called before any rendering operations.
         * @see GraphicsBackend
         */
        void initialize(GraphicsBackend backend = GraphicsBackend::GL);

        /**
         * @brief Check if the renderer backend is initialized.
         * 
         * @return true if the renderer backend is initialized, false otherwise.
         * 
         * @code{.cpp}
         * if (RaeptorCogs::Renderer().isInitialized()) {
         *    // Renderer is ready for use
         * }
         * @endcode
         * @note Useful for lazy initialization checks.
         */
        bool isInitialized() const;

        /**
         * @brief Set the current render list ID.
         * 
         * @param index The index of the render list to set as current.
         * 
         * @code{.cpp}
         * RaeptorCogs::Renderer().setRenderListID(1); // Set render list ID to 1
         * // Now graphics added will go to render list 1 and be rendered accordingly
         * @endcode
         * @note A render list is a collection of graphics to be rendered together.
         */
        void setRenderListID(int index);

        /**
         * @brief Add a graphic to the renderer.
         * 
         * @param graphic Pointer to the graphic to be added.
         */
        void add(Graphic2D&);

        /**
         * @brief Remove a graphic from the renderer.
         * 
         * @param graphic Pointer to the graphic to be removed.
         */
        void remove(Graphic2D&);

        /**
         * @brief Add a component to the renderer.
         * 
         * @param component Pointer to the component to be added.
         */
        void add(Component&);

        /**
         * @brief Remove a component from the renderer.
         * 
         * @param component Pointer to the component to be removed.
         */
        void remove(Component&);

        /**
         * @brief Render the scene to the specified target.
         * 
         * @param window Reference to the target window.
         * @param width Width of the rendering area. If 0, uses the window's width.
         * @param height Height of the rendering area. If 0, uses the window's height.
         * 
         * @code{.cpp}
         * RaeptorCogs::Window* window = RaeptorCogs::Renderer().createWindow(800, 600, "My Window");
         * RaeptorCogs::Renderer().render(*window); // Render to the window
         * @endcode
         * @note If a window is provided, renders to that window; otherwise, renders to the default framebuffer.
         */
        virtual void render(Window& window, int width = 0, int height = 0);

        /**
         * @brief Render the scene to the specified texture.
         * 
         * @param texture Reference to the target texture.
         * @param width Width of the rendering area. If 0, uses the texture's width.
         * @param height Height of the rendering area. If 0, uses the texture's height.
         * 
         * @code{.cpp}
         * RaeptorCogs::Window* window = RaeptorCogs::Renderer().createWindow(800, 600, "My Window");
         * RaeptorCogs::Renderer().render(window); // Render to the window
         * @endcode
         * @note Renders the scene directly into the provided texture.
         */
        virtual void render(Texture& texture, int width = 0, int height = 0);

        /**
         * @brief Get the active renderer backend.
         * 
         * @return Reference to the active renderer backend.
         */
        GAPI::Common::RendererBackend& getBackend();
        
        // --------------------------------------------
        //                   ImGui
        // --------------------------------------------

        /**
         * @brief Create a new ImGui frame.
         * 
         * @note Must be called at the beginning of each frame before any ImGui rendering.
         */
        void CreateImGuiFrame();

};
}