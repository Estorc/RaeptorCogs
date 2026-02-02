#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/Window.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/ImGuiModule.hpp>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

namespace RaeptorCogs::GAPI::Vulkan {

void RenderPipeline::beginFrame() {
    // Vulkan-specific begin frame code would go here
}

void RenderPipeline::endFrame() {
    // Vulkan-specific end frame code would go here
}

void RenderPipeline::transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcAccessMask = 0; // To be set based on layouts
    barrier.dstAccessMask = 0; // To be set based on layouts
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1,
        &barrier
    );
}

void beginDraw(RendererBackend* backend, WindowContext* , VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) {

    backend->getRenderPipeline().getVulkanFrame().commandBuffer = commandBuffer;
    backend->getRenderPipeline().getVulkanFrame().framebuffer = framebuffer;

    vkResetCommandBuffer(commandBuffer, 0);

    // Begin recording
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void endDraw(GraphicCore& graphicCore, VkCommandBuffer commandBuffer, WindowContext* currentContext, uint32_t* imageIndex) {
    VkSemaphore imageAvailableSemaphore = currentContext->getImageAvailableSemaphore(currentContext->getCurrentFrame());
    VkSemaphore renderFinishedSemaphore = currentContext->getRenderFinishedSemaphore(currentContext->getCurrentFrame());
    VkFence inFlightFence = currentContext->getInFlightFence(currentContext->getCurrentFrame());

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicCore.getGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }


    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &currentContext->getSwapchain();
    presentInfo.pImageIndices = imageIndex;

    VkResult result;
    result = vkQueuePresentKHR(graphicCore.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        currentContext->recreateSwapchain();
    }

    currentContext->advanceFrame();
};

void acquireNextImage(WindowContext* currentContext, VkDevice device, uint32_t* imageIndex) {
    VkSemaphore imageAvailableSemaphore = currentContext->getImageAvailableSemaphore(currentContext->getCurrentFrame());
    VkFence inFlightFence = currentContext->getInFlightFence(currentContext->getCurrentFrame());

    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(device,
                                            currentContext->getSwapchain(),
                                            UINT64_MAX,
                                            imageAvailableSemaphore,
                                            VK_NULL_HANDLE,
                                            imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        currentContext->recreateSwapchain();
        return;
    }
    if (currentContext->getImageInFlight(*imageIndex) != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &currentContext->getImageInFlight(*imageIndex), VK_TRUE, UINT64_MAX);
    }
    currentContext->getImageInFlight(*imageIndex) = inFlightFence;
    vkResetFences(device, 1, &inFlightFence);
}

void RenderPipeline::renderMask(Window* window, int x, int y, int width, int height) {
    (void) window;
    (void) x;
    (void) y;
    (void) width;
    (void) height;
    // Vulkan-specific mask rendering code would go here
}

#include <RaeptorCogs/Graphics/Vertex.hpp>

void RenderPipeline::renderPass(int , int , int , int ) {
    Vulkan::GraphicCore& graphicCore = static_cast<Vulkan::GraphicCore&>(this->getRenderer().getGraphicCore());
    Vulkan::RendererBackend& renderer = static_cast<Vulkan::RendererBackend&>(this->getRenderer());

    if (renderer.isInitialized() == false) {
        return;
    }
    
    uint32_t imageIndex;
    acquireNextImage(currentContext, graphicCore.getDevice(), &imageIndex);

    VkCommandBuffer commandBuffer = currentContext->getCommandBuffers()[imageIndex];
    VkFramebuffer framebuffer = currentContext->getFramebuffers()[imageIndex];

    graphicCore.clearColorBuffer(0.0f, 0.0f, 0.0f, 0.0f);
    beginDraw(&renderer, currentContext, commandBuffer, framebuffer);
    ImGui::Render();

    glm::vec4 clearColor = static_cast<Vulkan::RenderPipeline&>(renderer.getRenderPipeline()).getClearColor();
    VkRenderingAttachmentInfo colorAttach = {};
        colorAttach.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttach.imageView = currentContext->getSwapchainImageViews()[imageIndex];
        colorAttach.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttach.clearValue = {{{clearColor.r, clearColor.g, clearColor.b, clearColor.a}}};
    
    VkRenderingAttachmentInfo depthAttach = {};
        depthAttach.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttach.imageView = currentContext->getDepthImageView();
        depthAttach.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttach.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttach.clearValue.depthStencil = {1.0f, 0};

    VkRenderingInfo renderInfo = {};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.renderArea = {{0,0}, currentContext->getSwapExtent()};
        renderInfo.layerCount = 1;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = &colorAttach;
        renderInfo.pDepthAttachment = &depthAttach;

    VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width  = (float)currentContext->getSwapExtent().width;
        viewport.height = (float)currentContext->getSwapExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = currentContext->getSwapExtent();

    //VkDeviceSize offsets[] = { 0 };


VkImageMemoryBarrier toColor{};
    toColor.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    toColor.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    toColor.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    toColor.srcAccessMask = 0;
    toColor.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    toColor.image = currentContext->getSwapchainImages()[imageIndex];
    toColor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    toColor.subresourceRange.baseMipLevel = 0;
    toColor.subresourceRange.levelCount = 1;
    toColor.subresourceRange.baseArrayLayer = 0;
    toColor.subresourceRange.layerCount = 1;

    transitionImageLayout(
        commandBuffer,
        currentContext->getSwapchainImages()[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
    transitionImageLayout(
        commandBuffer,
        currentContext->getDepthImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
    );

    vkCmdBeginRendering(commandBuffer, &renderInfo);

    //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    graphicCore.getMainShader()->bind();
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    graphicCore.getQuadVBO()->bind();
    graphicCore.getQuadEBO()->bind();
    vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

    vkCmdEndRendering(commandBuffer);

    transitionImageLayout(
        commandBuffer,
        currentContext->getSwapchainImages()[imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
    );


    // Set clear color
    std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { { clearColor.r, clearColor.g, clearColor.b, clearColor.a } };
        clearValues[1].depthStencil = {1.0f, 0};

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderer.getGraphicCore().getRenderPass();
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = currentContext->getSwapExtent();
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
    endDraw(graphicCore, commandBuffer, currentContext, &imageIndex);
}

WindowContext* RenderPipeline::getCurrentContext() const {
    return this->currentContext;
}

void RenderPipeline::setCurrentContext(WindowContext* context) {
    this->currentContext = context;
}

glm::vec4 RenderPipeline::getClearColor() const {
    return this->clearColor;
}

void RenderPipeline::setClearColor(const glm::vec4& color) {
    this->clearColor = color;
}

VulkanFrame& RenderPipeline::getVulkanFrame() {
    return this->vulkanFrame;
}

}
#endif // VULKAN_SUPPORTED