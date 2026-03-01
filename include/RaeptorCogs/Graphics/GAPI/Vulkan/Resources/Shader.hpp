/** ********************************************************************************
 * @section GAPI_Common_Vulkan_Shader_Overview Overview
 * @file Shader.hpp
 * @brief Shader interface.
 * @details
 * Typical use cases:
 * - Defining Vulkan-specific shader implementations
 * *********************************************************************************
 * @section GAPI_Common_Vulkan_Shader_Header Header
 * <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Shader.hpp>
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
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Shader.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Object.hpp>

namespace RaeptorCogs::GAPI::Vulkan {

/**
 * @see RaeptorCogs::GAPI::Common::Shader
 */
class Shader : public Common::Shader {
  private:
    /**
     * @brief Vulkan shader module for the vertex shader.
     *
     * Used to hold the compiled vertex shader code.
     */
    VkPipeline pipeline = VK_NULL_HANDLE;

    /**
     * @brief Vulkan descriptor set layout.
     *
     * Defines the layout of descriptor sets used by the shader.
     */
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    /**
     * @brief Vulkan descriptor set layout.
     *
     * Defines the layout of descriptor sets used by the shader.
     */
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

  public:
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

    /**
     * @brief Get the Vulkan pipeline handle.
     *
     * @return Vulkan pipeline handle.
     */
    VkPipeline getPipeline() const;

    /**
     * @brief Get the Vulkan pipeline layout handle.
     *
     * @return Vulkan pipeline layout handle.
     */
    VkPipelineLayout getPipelineLayout() const;

    /**
     * @brief Get the Vulkan descriptor set layout handle.
     *
     * @return Vulkan descriptor set layout handle.
     */
    VkDescriptorSetLayout getDescriptorSetLayout() const;
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
const void *FindEmbeddedShader(
    const std::string &name, Common::ShaderStage stage, const std::string &profile);

} // namespace RaeptorCogs::GAPI::Vulkan