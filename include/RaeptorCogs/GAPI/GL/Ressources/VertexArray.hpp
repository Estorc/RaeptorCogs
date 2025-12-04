/** ********************************************************************************
 * @section GAPI_Common_GL_VertexArray_Overview Overview
 * @file VertexArray.hpp
 * @brief VertexArray interface.
 * @details
 * Typical use cases:
 * - Defining OpenGL-specific vertex array implementations
 * *********************************************************************************
 * @section GAPI_Common_GL_VertexArray_Header Header
 * <RaeptorCogs/GAPI/GL/Ressources/VertexArray.hpp>
 ***********************************************************************************
 * @section GAPI_Common_GL_VertexArray_Metadata Metadata
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
#include <RaeptorCogs/GAPI/GL/Ressources/Object.hpp>
#include <RaeptorCogs/GAPI/Common/Ressources/VertexArray.hpp>

namespace RaeptorCogs::GAPI::GL {

/**
 * @see RaeptorCogs::GAPI::Common::VertexArray
 */
class VertexArray : public Common::VertexArray {
    public:
        /**
         * @brief Initialize the vertex array.
         * 
         * Initializes the OpenGL vertex array object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        virtual void initialize() override;
        
        /**
         * @brief Bind the vertex array for use.
         * 
         * Binds the OpenGL vertex array object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void bind() override;

        /**
         * @brief Unbind the vertex array.
         * 
         * Unbinds the OpenGL vertex array object.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void unbind() const override;
};

/**
 * @see RaeptorCogs::GAPI::Common::VAO
 */
using VAO = VertexArray;

/** @brief Register VertexArray with the FactoryRegistry.*/
REGISTER(Common::VertexArray, VertexArray);

} // namespace RaeptorCogs::GAPI::GL