#include <RaeptorCogs/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>

namespace RaeptorCogs::GAPI::Vulkan {

void GraphicCore::createRenderPass(VkFormat swapchainImageFormat) {
    VkFormat depthFormat = findDepthFormat(this->physicalDevice);

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // index in attachment list
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkCreateRenderPass(this->device, &renderPassInfo, nullptr, &this->renderPass);
}


void GraphicCore::enableStencilGuarding() {
    // Setup stencil operations
}

void GraphicCore::drawElementsInstancedBaseVertexBaseInstance(size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) {
    (void) count;
    (void) instanceCount;
    (void) first;
    (void) baseVertex;
    (void) baseInstance;
    // Issue draw call
}

void GraphicCore::bindGraphicTexture(Graphic2D& graphic) {
    (void) graphic;
    // Bind the texture for rendering
}

void GraphicCore::bindMaskTexture() {
    // Bind the mask texture for rendering
}

void GraphicCore::setTextureUniform(ObjectHandler<Common::Shader> shader) {
    (void) shader;
    // Set the texture uniform in the shader
}

void GraphicCore::setMaskTextureUniform(ObjectHandler<Common::Shader> shader) {
    (void) shader;
    // Set the mask texture uniform in the shader
}

#ifdef __EMSCRIPTEN__
void GraphicCore::setSSBOTextureUniform(ObjectHandler<Common::Shader> shader) {
    (void) shader;
    // Set the SSBO texture uniform in the shader
}
#endif

void GraphicCore::setViewport(int x, int y, int width, int height) {
    (void) x;
    (void) y;
    (void) width;
    (void) height;
    // Set the viewport in Vulkan
}

void GraphicCore::clearSwapchainBuffers() {
    // Clear the swapchain buffers in Vulkan
}

void GraphicCore::useBlend() {
    // Enable blending in Vulkan
}

}