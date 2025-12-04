/** ********************************************************************************
 * @section Platform_Overview Overview
 * @file Platform.hpp
 * @brief Platform-specific window creation and management utilities.
 * @details
 * Typical use cases:
 * - Creating and destroying windows in a platform-agnostic manner.
 * *********************************************************************************
 * @section Platform_Header Header
 * <RaeptorCogs/Platform.hpp>
 ***********************************************************************************
 * @section Platform_Metadata Metadata
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
#include <RaeptorCogs/Singleton.hpp>
#include <string_view>
#include <vector>

namespace RaeptorCogs {
    class Window;
}

namespace RaeptorCogs::Singletons {

class Renderer;

/**
 * @brief Platform singleton class.
 * 
 * Manages platform-specific window creation and management.
 * 
 * @code{.cpp}
 * RaeptorCogs::Window* window = RaeptorCogs::Platform().createWindow(800, 600, "My Window");
 * // Use the window ...
 * RaeptorCogs::Platform().destroyWindow(window);
 * @endcode
 */
class Platform {
    private:
    
        // ============================================================================
        //                             PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the renderer singleton.
         * 
         * Used for window creation and management.
         * 
         * @see RaeptorCogs::Renderer
         */
        Renderer* renderer = nullptr;

        /**
         * @brief List of managed windows.
         * 
         * The renderer manages multiple windows for rendering.
         */
        std::vector<Window*> windows;

        friend SingletonAccessor<Platform>;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Create a new window with the specified dimensions and title.
         * 
         * @param width The width of the window.
         * @param height The height of the window.
         * @param title The title of the window.
         * @return Pointer to the created window.
         * 
         * @code{.cpp}
         * RaeptorCogs::Window* window = RaeptorCogs::Renderer().createWindow(800, 600, "My Window");
         * @endcode
         * @note The created window is managed by the renderer.
         */
        Window* createWindow(int width, int height, const std::string_view& title);

        /**
         * @brief Destroy the specified window.
         * 
         * @param window Pointer to the window to be destroyed.
         * 
         * @note The window is removed from the renderer's management.
         */
        void destroyWindow(Window* window);

        /**
         * @brief Get the list of managed windows.
         * 
         * @return Reference to the vector of managed windows.
         * 
         * @note Used for accessing all windows created by the platform.
         */
        std::vector<Window*>& getWindows() { return this->windows; }

        /**
         * @brief Set the renderer singleton.
         * 
         * @param renderer Pointer to the renderer singleton.
         */
        void setRenderer(Renderer* renderer) { this->renderer = renderer; }

        /**
         * @brief Get the renderer singleton.
         * 
         * @return Pointer to the renderer singleton.
         */
        Renderer* getRenderer() const { return this->renderer; }

        /**
         * @brief Destructor for the Platform singleton.
         * 
         * Cleans up all managed windows.
         */
        ~Platform();
};

} // namespace RaeptorCogs