/** ********************************************************************************
 * @section GAPI_Common_ImGuiModule_Overview Overview
 * @file ImGuiModule.hpp
 * @brief ImGui implementation interface.
 * @details
 * Typical use cases:
 * - Defining interfaces for ImGui backend implementations
 * *********************************************************************************
 * @section GAPI_Common_ImGuiModule_Header Header
 * <RaeptorCogs/GAPI/Common/Core/Internal/ImGuiModule.hpp>
 ***********************************************************************************
 * @section GAPI_Common_ImGuiModule_Metadata Metadata
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
struct GLFWwindow;
struct ImGuiContext;

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief ImGui implementation interface.
 * 
 * Provides an interface for ImGui backend implementations.
 */
class ImGuiModule {
    private:
        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        bool pendingFrame = false;

    protected:

        // ============================================================================
        //                             PROTECTED METHODS
        // ============================================================================

        void setPendingFrame(bool pending) {
            this->pendingFrame = pending;
        }

        bool isPendingFrame() const {
            return this->pendingFrame;
        }

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor.
         */
        ImGuiModule() = default;

        /**
         * @brief Virtual destructor.
         */
        virtual ~ImGuiModule() = default;

        /**
         * @brief Create ImGui context for the given window.
         * 
         * @param imGuiContext Reference to ImGui context pointer to be created.
         * @param window Pointer to the GLFW window.
         * 
         * @note This is a pure virtual function and must be implemented by derived classes.
         */
        virtual void createImGuiContext(ImGuiContext *&imGuiContext, GLFWwindow* window) = 0;

        /**
         * @brief Start a new ImGui frame.
         * 
         * @note Must be called at the beginning of each frame when using ImGui.
         */
        virtual void NewImGuiFrame() = 0;

        /**
         * @brief Render ImGui draw data.
         * 
         * @note Must be called to render ImGui draw data.
         */
        virtual void RenderImGuiDrawData() = 0;

};

}