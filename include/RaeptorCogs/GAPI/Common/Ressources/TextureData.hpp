/** ********************************************************************************
 * @section GAPI_Common_Core_TextureData_Overview Overview
 * @file TextureData.hpp
 * @brief TextureData interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for texture data management
 * *********************************************************************************
 * @section GAPI_Common_Core_TextureData_Header Header
 * <RaeptorCogs/GAPI/Common/Ressources/TextureData.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_TextureData_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Ressources/Object.hpp>
#include <RaeptorCogs/GAPI/GL/Constants.hpp>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief TextureData interface.
 * 
 * Provides a common interface for texture data management.
 * 
 * @note This is an abstract base class and should be inherited by specific texture data implementations.
 */
class TextureData : public ObjectData {
    public:
        /**
         * @brief Build the texture data.
         * 
         * @param width Width of the texture.
         * @param height Height of the texture.
         * @param data Pointer to the texture data.
         * @param minFilter Minification filter (default: GL_LINEAR_MIPMAP_NEAREST).
         * @param magFilter Magnification filter (default: GL_LINEAR).
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void build(int width, int height, void * data, GLenum minFilter = GL_LINEAR_MIPMAP_NEAREST, GLenum magFilter = GL_LINEAR) = 0;
};

}