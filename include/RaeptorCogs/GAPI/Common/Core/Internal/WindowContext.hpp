/** ********************************************************************************
 * @section GAPI_Common_WindowContext_Overview Overview
 * @file WindowContext.hpp
 * @brief Window context interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for different window context implementations
 * *********************************************************************************
 * @section GAPI_Common_WindowContext_Header Header
 * <RaeptorCogs/GAPI/Common/Core/Internal/WindowContext.hpp>
 ***********************************************************************************
 * @section GAPI_Common_WindowContext_Metadata Metadata
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

namespace RaeptorCogs {
    class Window;
}

namespace RaeptorCogs::GAPI::Common {

class RendererBackend;

/**
 * @brief Window context interface.
 * 
 * Provides an interface for window context implementations.
 */
class WindowContext {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Pointer to renderer.
         * 
         * Holds a pointer to the renderer singleton.
         */
        RendererBackend* renderer;

    protected:

        // ============================================================================
        //                             PROTECTED MEMBERS
        // ============================================================================

        /**
         * @brief Get the renderer pointer.
         * 
         * @return Pointer to the renderer singleton.
         */
        RendererBackend* getRenderer() const {
            return this->renderer;
        }

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor.
         */
        WindowContext(RendererBackend* renderer) : renderer(renderer) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~WindowContext() = default;

        /**
         * @brief Create ImGui context for the given window.
         * 
         * @param window Pointer to the window.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void initialize(Window* window) = 0;

        /**
         * @brief Bind the window context for rendering.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void bind() = 0;
};

}