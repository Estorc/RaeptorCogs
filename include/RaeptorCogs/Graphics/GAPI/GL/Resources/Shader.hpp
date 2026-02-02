/** ********************************************************************************
 * @section GAPI_Common_GL_Shader_Overview Overview
 * @file Shader.hpp
 * @brief Shader interface.
 * @details
 * Typical use cases:
 * - Defining OpenGL-specific shader implementations
 * *********************************************************************************
 * @section GAPI_Common_GL_Shader_Header Header
 * <RaeptorCogs/Graphics/GAPI/GL/Resources/Shader.hpp>
 ***********************************************************************************
 * @section GAPI_Common_GL_Shader_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/Object.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Shader.hpp>

namespace RaeptorCogs::GAPI::GL {

/**
 * @see RaeptorCogs::GAPI::Common::Shader
 */
class Shader : public ObjectMixin<Common::Shader> {
    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Shader.
         */
        Shader();

        /**
         * @brief Destructor for Shader.
         */
        ~Shader();
        
        /**
         * @see RaeptorCogs::GAPI::Common::Shader::build()
         */
        virtual void build(const Common::ShaderInfo &shaderInfo) override;

        /**
         * @brief Build the shader program from source code.
         * @param vertexSource Source code for the vertex shader.
         * @param fragmentSource Source code for the fragment shader.
         */
        void build(const char* vertexSource, const char* fragmentSource);

        /**
         * @brief Bind the shader program for use.
         * 
         * Binds the OpenGL shader program.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void bind() override;

        /**
         * @brief Unbind the shader program.
         * 
         * Unbinds the OpenGL shader program.
         * 
         * @note Overrides the pure virtual method from the base class.
         */
        void unbind() const override;

        // -------------------------------------------------------------
        //          Utility methods to set uniform variables
        // -------------------------------------------------------------

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setBool() const
         */
        void setBool(const std::string &name, bool value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setInt() const
         */
        void setInt(const std::string &name, int value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setUInt() const
         */
        void setUInt(const std::string &name, unsigned int value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setFloat() const
         */
        void setFloat(const std::string &name, float value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec2() const
         */
        void setVec2(const std::string &name, const glm::vec2 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec3() const
         */
        void setVec3(const std::string &name, const glm::vec3 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec4() const
         */
        void setVec4(const std::string &name, const glm::vec4 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat2() const
         */
        void setMat2(const std::string &name, const glm::mat2 &mat) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat3() const
         */
        void setMat3(const std::string &name, const glm::mat3 &mat) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat4() const
         */
        void setMat4(const std::string &name, const glm::mat4 &mat) const override;
};

/** @brief Register Shader with the FactoryRegistry.*/
REGISTER(Common::Shader, Shader);

/**
 * @brief Find an embedded shader by name, stage, and profile.
 * 
 * @param name Name of the shader.
 * @param stage Stage of the shader (vertex, fragment, etc.).
 * @param profile Profile of the shader (e.g., "core", "es").
 * @return Pointer to the shader source code, or nullptr if not found.
 * 
 * @note Searches the provided array for a matching shader.
 */
const void * FindEmbeddedShader(const std::string& name, Common::ShaderStage stage, const std::string& profile);


}