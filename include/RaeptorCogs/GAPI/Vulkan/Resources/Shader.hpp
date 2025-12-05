/** ********************************************************************************
 * @section GAPI_Common_Vulkan_Shader_Overview Overview
 * @file Shader.hpp
 * @brief Shader interface.
 * @details
 * Typical use cases:
 * - Defining Vulkan-specific shader implementations
 * *********************************************************************************
 * @section GAPI_Common_Vulkan_Shader_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Resources/Shader.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Vulkan_Shader_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Vulkan/Resources/Object.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Shader.hpp>

namespace RaeptorCogs::GAPI::Vulkan {

/**
 * @see RaeptorCogs::GAPI::Common::Shader
 */
class Shader : public Common::Shader {
    public:

        /**
         * @brief Default constructor.
         */
        Shader() = default;

        /**
         * @brief Virtual destructor.
         */
        ~Shader() override = default;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::build()
         */
        virtual void build(const char* vertexSource, const char* fragmentSource) override;

        /**
         * @brief Initialize the shader.
         * 
         * Initializes the Vulkan shader modules.
         */
        void initialize() override;

        /**
         * @brief Bind the shader for use.
         * 
         * Binds the Vulkan shader program.
         */
        void bind() override;

        /**
         * @brief Unbind the shader.
         * 
         * Unbinds the Vulkan shader program.
         */
        void unbind() const override;

        // -------------------------------------------------------------
        //          Utility methods to set uniform variables
        // -------------------------------------------------------------

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setBool()
         */
        void setBool(const std::string &name, bool value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setInt()
         */
        void setInt(const std::string &name, int value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setUInt()
         */
        void setUInt(const std::string &name, unsigned int value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setFloat()
         */
        void setFloat(const std::string &name, float value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec2()
         */
        void setVec2(const std::string &name, const glm::vec2 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec3()
         */
        void setVec3(const std::string &name, const glm::vec3 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setVec4()
         */
        void setVec4(const std::string &name, const glm::vec4 &value) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat2()
         */
        void setMat2(const std::string &name, const glm::mat2 &mat) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat3()
         */
        void setMat3(const std::string &name, const glm::mat3 &mat) const override;

        /**
         * @see RaeptorCogs::GAPI::Common::Shader::setMat4()
         */
        void setMat4(const std::string &name, const glm::mat4 &mat) const override;
};

/** @brief Register Shader with the FactoryRegistry.*/
REGISTER(Common::Shader, Shader);

}