/** ********************************************************************************
 * @section Window_Overview Overview
 * @file Window.hpp
 * @brief Window management and context handling.
 * @details
 * Typical use cases:
 * - Managing window creation, resizing, and input handling.
 * *********************************************************************************
 * @section Window_Header Header
 * <RaeptorCogs/Window.hpp>
 ***********************************************************************************
 * @section Window_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>
#include <string>


struct GLFWwindow;
struct GLFWmonitor;
struct GLFWcursor;
struct ImGuiContext;

namespace RaeptorCogs {

/**
 * @brief Window flags enumeration.
 * 
 * Defines flags for window state management.
 */
enum class WindowFlags : uint32_t {
    /** No flags set */
    NONE = 0,
    /** Context only flag (for GLAD initialization) */
    CONTEXT_ONLY = 1 << 0,
    /** Destroy after initialization flag */
    DESTROY_AFTER_INIT = 1 << 1
};

/**
 * @brief Class representing a window.
 * 
 * Manages window creation, resizing, and input handling.
 * 
 * @see RaeptorCogs::GAPI::Common::WindowContext
 */
class Window : public FlagSet<WindowFlags> {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the underlying GLFW window.
         * 
         * Used for low-level window management.
         */
        GLFWwindow* window;

        /**
         * @brief Window title.
         * 
         * Holds the current title of the window.
         */
        std::string title;

        /**
         * @brief Map of custom cursors.
         * 
         * Holds GLFWcursor pointers for different cursor shapes.
         */
        std::unordered_map<int, GLFWcursor*> cursors;

        #ifdef USE_IMGUI
        /**
         * @brief ImGui context associated with the window.
         * 
         * Used for ImGui rendering and input handling.
         */
        ImGuiContext* imGuiContext = nullptr;
        #endif

        /**
         * @brief Window context for rendering.
         * 
         * Holds the context used for rendering operations.
         */
        RaeptorCogs::GAPI::Common::WindowContext* context = nullptr;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Window.
         * 
         * @param width Width of the window.
         * @param height Height of the window.
         * @param title Title of the window.
         * @param monitor Pointer to the GLFW monitor (for fullscreen).
         * @param share Pointer to another GLFW window to share resources with.
         * @param context Pointer to a custom window context (optional).
         * @param flags Window flags for configuration.
         * 
         * @note Initializes the GLFW window with the specified parameters.
         */
        Window(int width, int height, const std::string& title, GLFWmonitor* monitor, GLFWwindow* share, RaeptorCogs::GAPI::Common::WindowContext* context = nullptr, WindowFlags flags = WindowFlags::NONE);
        
        /**
         * @brief Destructor for Window.
         * 
         * Cleans up resources associated with the window.
         */
        ~Window();

        /**
         * @brief Get the underlying GLFW window pointer.
         * 
         * @return Pointer to the GLFWwindow.
         */
        GLFWwindow* getGLFWWindow() const;

        /**
         * @brief Get the width of the window.
         * 
         * @return Width of the window in pixels.
         */
        unsigned int getWidth() const;

        /**
         * @brief Get the height of the window.
         * 
         * @return Height of the window in pixels.
         */
        unsigned int getHeight() const;

        /**
         * @brief Get the size of the window.
         * 
         * @return Size of the window as a glm::ivec2 (width, height).
         */
        glm::ivec2 getSize() const;

        /**
         * @brief Get the current mouse position within the window.
         * 
         * @return Mouse position as a glm::ivec2 (x, y).
         */
        glm::ivec2 getMousePosition() const;

        /**
         * @brief Check if the mouse cursor is within the window bounds.
         * 
         * @return true if the mouse is inside the window, false otherwise.
         */
        bool isMouseInWindow() const;

        /**
         * @brief Get the title of the window.
         * 
         * @return Reference to the window title string.
         */
        const std::string& getTitle() const;

        /**
         * @brief Get the window context used for rendering.
         * 
         * @return Pointer to the WindowContext object.
         */
        RaeptorCogs::GAPI::Common::WindowContext* getContext() const;

        /**
         * @brief Make the window's context current.
         * 
         * Sets the OpenGL context of this window as the current context for rendering.
         */
        void makeContextCurrent();

        /**
         * @brief Set the title of the window.
         * 
         * @param newTitle New title for the window.
         */
        void setTitle(const std::string& newTitle);

        /**
         * @brief Set the size of the window.
         * 
         * @param width New width of the window in pixels.
         * @param height New height of the window in pixels.
         * 
         * @note Resizes the window to the specified dimensions.
         */
        void setSize(int width, int height);

        /**
         * @brief Set the icon of the window.
         * 
         * @param iconPaths Vector of file paths to the icon images.
         */
        void setIcon(const std::vector<std::string>& iconPaths);
        
        /**
         * @brief Select the cursor shape for the window.
         * 
         * @param cursorShape Integer representing the cursor shape.
         */
        void selectCursor(int cursorShape);
};

}