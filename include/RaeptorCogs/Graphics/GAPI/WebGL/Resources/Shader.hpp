/** ********************************************************************************
 * @section GAPI_Common_WebGL_Shader_Overview Overview
 * @file Shader.hpp
 * @brief Shader interface.
 * @details
 * Typical use cases:
 * - Defining WebGL-specific shader implementations
 * *********************************************************************************
 * @section GAPI_Common_WebGL_Shader_Header Header
 * <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Shader.hpp>
 ***********************************************************************************
 * @section GAPI_Common_WebGL_Shader_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Object.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/Shader.hpp>

namespace RaeptorCogs::GAPI::WebGL {

/**
 * @see RaeptorCogs::GAPI::Common::Shader
 */
class Shader : public GL::Shader {};

/** @brief Register Shader with the FactoryRegistry.*/
REGISTER(Common::Shader, Shader);

}