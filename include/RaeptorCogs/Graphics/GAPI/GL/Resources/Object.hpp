/** ********************************************************************************
 * @section GAPI_Common_GL_Object_Overview Overview
 * @file Object.hpp
 * @brief Object interface.
 * @details
 * Typical use cases:
 * - Defining OpenGL-specific object implementations
 * *********************************************************************************
 * @section GAPI_Common_GL_Object_Header Header
 * <RaeptorCogs/Graphics/GAPI/GL/Resources/Object.hpp>
 ***********************************************************************************
 * @section GAPI_Common_GL_Object_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Constants.hpp>

namespace RaeptorCogs::GAPI::GL {

template <typename T, typename = typename std::enable_if<std::is_base_of<Common::ObjectData, T>::value>::type>
class ObjectMixin : public T {
  protected:
    // ============================================================================
    //                             PROTECTED ATTRIBUTES
    // ============================================================================

    /**
     * @brief OpenGL object ID.
     *
     * Stores the OpenGL-specific object identifier.
     */
    GLuint glObjectID = 0;

  public:
    // ============================================================================
    //                             PUBLIC METHODS
    // ============================================================================

    /**
     * @brief Constructor for OpenGL Object.
     */
    ObjectMixin() = default;

    /**
     * @brief Destructor for OpenGL Object.
     */
    virtual ~ObjectMixin() = default;

    /**
     * @brief Get the OpenGL object ID.
     *
     * @return GLuint The OpenGL object identifier.
     */
    size_t getID() const override {
      return static_cast<size_t>(this->glObjectID);
    }

    /**
     * @brief Check if the object is valid.
     *
     * @return true if the object is valid, false otherwise.
     *
     * @note An object is considered valid if its OpenGL object ID is non-zero.
     */
    bool isValid() const override {
      return this->glObjectID != 0;
    }
};

/**
 * @brief Declaration of factory registry for OpenGL objects.
 * @see RaeptorCogs::GAPI::Common::ObjectData
 */
FACTORY_REGISTRY_DECLARATION();

} // namespace RaeptorCogs::GAPI::GL