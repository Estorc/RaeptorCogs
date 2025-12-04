/** ********************************************************************************
 * @section GAPI_Common_FrameData_Overview Overview
 * @file FrameData.hpp
 * @brief Frame data structure.
 * @details
 * Typical use cases:
 * - Holding frame-specific data for rendering
 * *********************************************************************************
 * @section GAPI_Common_FrameData_Header Header
 * <RaeptorCogs/GAPI/Common/Core/Internal/FrameData.hpp>
 ***********************************************************************************
 * @section GAPI_Common_FrameData_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Ressources/Shader.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include <variant>


namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Type alias for uniform data.
 * 
 * Holds the possible types for uniform variables.
 */
using UniformData = std::variant<bool, int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

/**
 * @brief Type alias for uniform hash map.
 * 
 * Maps uniform names to their corresponding data.
 */
using UniformHashMap = std::unordered_map<std::string, UniformData>;


/**
 * @brief Frame data structure.
 * 
 * Holds data relevant to the current frame being rendered.
 */
struct FrameData {
    /**
     * @brief Frame width.
     * 
     * Holds the width of the current frame.
     */
    double width;

    /**
     * @brief Frame height.
     * 
     * Holds the height of the current frame.
     */
    double height;

    /**
     * @brief Uniforms for the frame.
     * 
     * Holds a map of uniform names to their corresponding data.
     */
    UniformHashMap uniforms = {
        {"uTime", 0.0f},
        {"uViewMatrix", glm::mat4(1.0f)},
        {"uProjectionMatrix", glm::mat4(1.0f)},
    };

    /** @brief Reference to the time uniform. */
    float& time = std::get<float>(uniforms["uTime"]);
    
    /** @brief Reference to the view matrix uniform. */
    glm::mat4& viewMatrix = std::get<glm::mat4>(uniforms["uViewMatrix"]);

    /** @brief Reference to the projection matrix uniform. */
    glm::mat4& projectionMatrix = std::get<glm::mat4>(uniforms["uProjectionMatrix"]);
    
    /**
     * @brief Active shader.
     * 
     * Holds the currently active shader for the frame.
     */
    ObjectHandler<Common::Shader> activeShader;

    /**
     * @brief Upload uniforms to the active shader.
     * 
     * Uploads all uniforms stored in the `uniforms` map to the currently active shader.
     */
    void upload() {
        if (!activeShader.get()) return;
        activeShader->bind();
        for (auto& [name, value] : uniforms) {
            std::visit([&](auto&& v) {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T,int>)
                    activeShader->setInt(name, v);
                else if constexpr (std::is_same_v<T,float>)
                    activeShader->setFloat(name, v);
                else if constexpr (std::is_same_v<T,glm::vec3>)
                    activeShader->setVec3(name, v);
                else if constexpr (std::is_same_v<T,glm::mat4>)
                    activeShader->setMat4(name, v);
            }, value);
        }
    }

};

}