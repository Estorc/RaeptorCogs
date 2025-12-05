/** ********************************************************************************
 * @section GAPI_Vulkan_GraphicCore_Overview Overview
 * @file GraphicCore.hpp
 * @brief Graphic core interface.
 * @details
 * Typical use cases:
 * - Providing Vulkan-specific core graphics functionality and resource management
 * *********************************************************************************
 * @section GAPI_Vulkan_GraphicCore_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Core/Internal/GraphicCore.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_GraphicCore_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/Buffer.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/TextureData.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/Shader.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/VertexArray.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <vulkan/vulkan.h>

namespace RaeptorCogs::GAPI::Vulkan {

#include <RaeptorCogs/EmbedShaders.hpp>

/**
 * @brief Maximum number of sprites supported.
 */
constexpr int MAX_SPRITES = 8000000;

/**
 * @brief Width of the instance data texture.
 */
constexpr int INSTANCE_SIZE = 32;

#ifdef __EMSCRIPTEN__
/**
 * @brief Height of the instance data texture.
 */
constexpr int IDATATEX_HEIGHT = (MAX_SPRITES * INSTANCE_SIZE + 16 * IDATATEX_WIDTH - 1) / (16 * IDATATEX_WIDTH);
#else
/**
 * @brief Default size of the dynamic instance data buffer.
 */
constexpr int DEFAULT_DYNAMIC_DATA_BUFFER_SIZE = (MAX_SPRITES * INSTANCE_SIZE);
/**
 * @brief Size of the static instance data buffer.
 */
constexpr int STATIC_INSTANCE_DATA_SIZE = (MAX_SPRITES * sizeof(Common::StaticInstanceData));
#endif

class GraphicCore : public Common::GraphicCore {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Vulkan instance.
         * 
         * Represents the Vulkan instance.
         */
        VkInstance instance;

        /**
         * @brief Vulkan physical device.
         * 
         * Represents the Vulkan physical device.
         */
        VkPhysicalDevice physicalDevice;

        /**
         * @brief Vulkan logical device.
         * 
         * Represents the Vulkan logical device.
         */
        VkDevice device;

        /**
         * @brief Vulkan graphics queue.
         * 
         * Handles graphics commands.
         */
        VkQueue graphicsQueue;

        /**
         * @brief Vulkan present queue.
         * 
         * Handles presentation commands.
         */
        VkQueue presentQueue;

        /**
         * @brief Vulkan pipeline cache.
         * 
         * Caches pipeline state objects.
         */
        VkPipelineCache pipelineCache;

        /**
         * @brief Vulkan render pass.
         * 
         * Defines the render pass configuration.
         * 
         * @note Lazily initialized in createRenderPass().
         */
        VkRenderPass renderPass = VK_NULL_HANDLE;

        /**
         * @brief Vulkan command pool.
         * 
         * Manages the allocation of command buffers.
         */
        VkCommandPool commandPool;

        /**
         * @brief Vulkan command buffer.
         * 
         * Records rendering commands.
         */
        VkCommandBuffer commandBuffer;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for GraphicCore.
         * 
         * Initializes the GraphicCore with a reference to the renderer backend.
         * 
         * @param renderer Reference to the renderer backend.
         */
        GraphicCore(Common::RendererBackend& renderer) : Common::GraphicCore(renderer) {}

        /**
         * @brief Create Vulkan render pass.
         * 
         * @param swapchainImageFormat Format of the swapchain images.
         * 
         * @note Initializes the renderPass member.
         */
        void createRenderPass(VkFormat swapchainImageFormat);

        /**
         * @brief Get the Vulkan instance.
         * 
         * @return Vulkan instance.
         */
        VkInstance& getInstance() { return this->instance; }

        /**
         * @brief Get the Vulkan physical device.
         * 
         * @return Vulkan physical device.
         */
        VkPhysicalDevice& getPhysicalDevice() { return this->physicalDevice; }

        /**
         * @brief Get the Vulkan logical device.
         * 
         * @return Vulkan logical device.
         */
        VkDevice& getDevice() { return this->device; }

        /**
         * @brief Get the Vulkan graphics queue.
         * 
         * @return Vulkan graphics queue.
         */
        VkQueue& getGraphicsQueue() { return this->graphicsQueue; }

        /**
         * @brief Get the Vulkan present queue.
         * 
         * @return Vulkan present queue.
         */
        VkQueue& getPresentQueue() { return this->presentQueue; }

        /**
         * @brief Get the Vulkan pipeline cache.
         * 
         * @return Vulkan pipeline cache.
         */
        VkPipelineCache& getPipelineCache() { return this->pipelineCache; }

        /**
         * @brief Get the Vulkan render pass.
         * 
         * @return Vulkan render pass.
         */
        VkRenderPass& getRenderPass() { return this->renderPass; }

        /**
         * @brief Get the Vulkan command pool.
         * 
         * @return Vulkan command pool.
         */
        VkCommandPool& getCommandPool() { return this->commandPool; }

        /**
         * @brief Get the Vulkan command buffer.
         * 
         * @return Vulkan command buffer.
         */
        VkCommandBuffer& getCommandBuffer() { return this->commandBuffer; }

        // ------------------------------------------------------------------------
        //                      State machine methods
        // ------------------------------------------------------------------------
        
        /**
         * @see Common::GraphicCore::setViewport
         */
        void setViewport(int x, int y, int width, int height) override;

        /**
         * @see Common::GraphicCore::clearSwapchainBuffers
         */
        void clearSwapchainBuffers() override;

        /**
         * @see Common::GraphicCore::useBlend
         */
        void useBlend() override;

        /**
         * @see Common::GraphicCore::bindMaskTexture
         */
        void bindMaskTexture() override;

        /**
         * @see Common::GraphicCore::setTextureUniform
         */
        void setTextureUniform(ObjectHandler<Common::Shader> shader) override;

        /**
         * @see Common::GraphicCore::setMaskTextureUniform
         */
        void setMaskTextureUniform(ObjectHandler<Common::Shader> shader) override;

        #ifdef __EMSCRIPTEN__
        /**
         * @see Common::GraphicCore::setSSBOTextureUniform
         */
        void setSSBOTextureUniform(ObjectHandler<Common::Shader> shader) override;
        #endif

        /**
         * @see Common::GraphicCore::drawElementsInstancedBaseVertexBaseInstance
         */
        void drawElementsInstancedBaseVertexBaseInstance(size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) override;
        
        /**
         * @see Common::GraphicCore::bindGraphicTexture
         */
        void bindGraphicTexture(Graphic2D& graphic) override;

        /**
         * @see Common::GraphicCore::enableStencilGuarding
         */
        void enableStencilGuarding() override;

};

}