/** ********************************************************************************
 * @section GAPI_GL_ImGuiModule_Overview Overview
 * @file ImGuiModule.hpp
 * @brief OpenGL ImGui implementation.
 * @details
 * Typical use cases:
 * - Defining OpenGL-specific ImGui backend implementations
 * *********************************************************************************
 * @section GAPI_GL_ImGuiModule_Header Header
 * <RaeptorCogs/GAPI/GL/Core/Internal/ImGuiModule.hpp>
 ***********************************************************************************
 * @section GAPI_GL_ImGuiModule_Metadata Metadata
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

namespace RaeptorCogs::GAPI::GL {

/**
 * @brief OpenGL ImGui implementation.
 * 
 * Provides OpenGL-specific implementations for the ImGui backend interface.
 */
class ImGuiModule : public Common::ImGuiModule {
    public:
        /**
         * @brief Default constructor.
         */
        ImGuiModule() = default;

        /**
         * @brief Virtual destructor.
         */
        ~ImGuiModule() override = default;

        /**
         * @see RaeptorCogs::GAPI::Common::ImGuiModule::createImGuiContext()
         */
        void createImGuiContext(ImGuiContext *&imGuiContext, GLFWwindow* window) override;

        /**
         * @see RaeptorCogs::GAPI::Common::ImGuiModule::NewImGuiFrame()
         */
        void NewImGuiFrame() override;

        /**
         * @see RaeptorCogs::GAPI::Common::ImGuiModule::RenderImGuiDrawData()
         */
        void RenderImGuiDrawData() override;
};

}