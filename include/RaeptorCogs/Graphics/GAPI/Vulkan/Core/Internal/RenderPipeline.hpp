/** ********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Overview Overview
 * @file RenderPipeline.hpp
 * @brief Render pipeline interface.
 * @details
 * Typical use cases:
 * - Providing Vulkan-specific implementations for the render pipeline interface
 * *********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Header Header
 * <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/RenderPipeline.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Metadata Metadata
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
#include <RaeptorCogs/Graphics/GAPI/Common/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/VulkanFrame.hpp>
#include <vulkan/vulkan.h>

namespace RaeptorCogs::GAPI::Vulkan {

class RenderPipeline : public Common::RenderPipeline {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Current window context.
         */
        WindowContext* currentContext = nullptr;

        /**
         * @brief Clear color for the render pipeline.
         */
        glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        /**
         * @brief Vulkan frame data.
         */
        VulkanFrame vulkanFrame;

    protected:

        // ============================================================================
        //                               PROTECTED METHODS
        // ============================================================================

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::beginFrame()
         */
        void beginFrame() override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::endFrame()
         */
        void endFrame() override;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for RenderPipeline.
         */
        RenderPipeline(Common::RendererBackend& renderer) : Common::RenderPipeline(renderer) {}

        /**
         * @brief Destructor for RenderPipeline.
         */
        ~RenderPipeline() = default;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::renderPass()
         */
        void renderPass(int x, int y, int width, int height) override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::renderMask()
         */
        void renderMask(Window* window, int x, int y, int width, int height) override;

        /**
         * @brief Transition image layout.
         * @param commandBuffer The command buffer to record the transition commands.
         * @param image The Vulkan image to transition.
         * @param oldLayout The current layout of the image.
         * @param newLayout The desired layout of the image.
         * @param aspectMask The aspect mask for the image subresource.
         * @param srcStageMask The source pipeline stage mask.
         * @param dstStageMask The destination pipeline stage mask.
         * 
         * @note This function records the necessary commands to transition the image
         * layout within the provided command buffer.
         */
        void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

        /**
         * @brief Get the current window context.
         * @return Pointer to the current WindowContext.
         * @note Used to manage the active rendering context.
         */
        WindowContext* getCurrentContext() const;

        /**
         * @brief Set the current window context.
         * @param context Pointer to the WindowContext to set as current.
         * @note Used to manage the active rendering context.
         */
        void setCurrentContext(WindowContext* context);

        /**
         * @brief Get the clear color of the render pipeline.
         * @return The clear color as a glm::vec4.
         */
        glm::vec4 getClearColor() const;

        /**
         * @brief Set the clear color for the render pipeline.
         * @param color The clear color as a glm::vec4.
         * @note The color components should be in the range [0.0, 1.0].
         * The alpha component is used for transparency.
         */
        void setClearColor(const glm::vec4& color);

        /**
         * @brief Get the Vulkan frame data.
         * @return Reference to the VulkanFrame structure.
         */
        VulkanFrame& getVulkanFrame();

};

} // namespace RaeptorCogs::GAPI::Vulkan