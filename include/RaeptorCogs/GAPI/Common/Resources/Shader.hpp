/** ********************************************************************************
 * @section GAPI_Common_Core_Shader_Overview Overview
 * @file Shader.hpp
 * @brief Shader interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for shader management
 * *********************************************************************************
 * @section GAPI_Common_Core_Shader_Header Header
 * <RaeptorCogs/GAPI/Common/Resources/Shader.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_Shader_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <string>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Shader interface.
 * 
 * Provides a common interface for shader management.
 * 
 * @note This is an abstract base class and should be inherited by specific shader implementations.
 */
class Shader : public ObjectData {
    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor.
         */
        Shader() = default;

        /**
         * @brief Virtual destructor.
         */
        virtual ~Shader() = default;

        /**
         * @brief Build the shader program from source code.
         * 
         * @param vertexSource Source code for the vertex shader.
         * @param fragmentSource Source code for the fragment shader.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void build(const char* vertexSource, const char* fragmentSource) = 0;

        // -------------------------------------------------------------
        //          Utility methods to set uniform variables
        // -------------------------------------------------------------

        /**
         * @brief Set a boolean uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value Boolean value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setBool(const std::string &name, bool value) const = 0;

        /**
         * @brief Set an integer uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value Integer value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setInt(const std::string &name, int value) const = 0;

        /**
         * @brief Set an unsigned integer uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value Unsigned integer value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setUInt(const std::string &name, unsigned int value) const = 0;

        /**
         * @brief Set a float uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value Float value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setFloat(const std::string &name, float value) const = 0;

        /**
         * @brief Set a vec2 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value vec2 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setVec2(const std::string &name, const glm::vec2 &value) const = 0;

        /**
         * @brief Set a vec3 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value vec3 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setVec3(const std::string &name, const glm::vec3 &value) const = 0;

        /**
         * @brief Set a vec4 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param value vec4 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setVec4(const std::string &name, const glm::vec4 &value) const = 0;

        /**
         * @brief Set a mat2 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param mat mat2 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setMat2(const std::string &name, const glm::mat2 &mat) const = 0;

        /**
         * @brief Set a mat3 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param mat mat3 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setMat3(const std::string &name, const glm::mat3 &mat) const = 0;

        /**
         * @brief Set a mat4 uniform variable.
         * 
         * @param name Name of the uniform variable.
         * @param mat mat4 value to set.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void setMat4(const std::string &name, const glm::mat4 &mat) const = 0;
};

}