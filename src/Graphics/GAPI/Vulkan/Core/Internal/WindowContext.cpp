#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Memory.hpp>
#include <RaeptorCogs/Graphics/Window.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::Vulkan {

void WindowContext::createSurfaceAndRenderPass() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());
    // Create Vulkan surface for the window
    if (glfwCreateWindowSurface(vkRenderer.getGraphicCore().getInstance(), this->getWindow()->getGLFWWindow(), nullptr, &this->getSurface()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkRenderer.getGraphicCore().getPhysicalDevice(), this->getSurface(), &this->getSurfaceCapabilities());

    // Get surface format
    this->getSurfaceFormat() = findBestSurfaceFormat(vkRenderer.getGraphicCore().getPhysicalDevice(), this->getSurface());
    if (vkRenderer.getGraphicCore().getRenderPass() == VK_NULL_HANDLE) vkRenderer.getGraphicCore().createRenderPass(this->getSurfaceFormat().format);
}

void WindowContext::createSwapchainImages() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());
    // Choose swap extent (same as window size)
    this->getSwapExtent() = { static_cast<uint32_t>(this->getWindow()->getWidth()), static_cast<uint32_t>(this->getWindow()->getHeight()) };

    // Create swapchain
    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = this->getSurface();
    swapchainInfo.minImageCount = 2; // Double buffering
    swapchainInfo.imageFormat = this->getSurfaceFormat().format;
    swapchainInfo.imageColorSpace = this->getSurfaceFormat().colorSpace;
    swapchainInfo.imageExtent = this->getSwapExtent();
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = this->getSurfaceCapabilities().currentTransform;
    swapchainInfo.compositeAlpha = chooseCompositeAlpha(this->getSurfaceCapabilities());
    swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // V-Sync
    swapchainInfo.clipped = VK_TRUE;

    vkCreateSwapchainKHR(vkRenderer.getGraphicCore().getDevice(), &swapchainInfo, nullptr, &this->getSwapchain());
}

void WindowContext::createImageViews() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());
    // Get swapchain images
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(vkRenderer.getGraphicCore().getDevice(), this->getSwapchain(), &imageCount, nullptr);
    std::vector<VkImage>& swapchainImages = this->getSwapchainImages();
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkRenderer.getGraphicCore().getDevice(), this->getSwapchain(), &imageCount, swapchainImages.data());

    // Create image views
    std::vector<VkImageView>& swapchainImageViews = this->getSwapchainImageViews();
    swapchainImageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = this->getSurfaceFormat().format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(vkRenderer.getGraphicCore().getDevice(), &viewInfo, nullptr, &swapchainImageViews[i]);
    }

    // Create depth image view
    VkFormat depthFormat = findDepthFormat(vkRenderer.getGraphicCore().getPhysicalDevice());
    // 1. Create the image
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = this->getSwapExtent().width;
    imageInfo.extent.height = this->getSwapExtent().height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(vkRenderer.getGraphicCore().getDevice(), &imageInfo, nullptr, &this->getDepthImage()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create depth image!");
    }

    // 2. Allocate memory
    this->getDepthImageMemory() = AllocateMemory(&vkRenderer, this->getDepthImage());

    // 3. Create image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = this->getDepthImage();
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vkRenderer.getGraphicCore().getDevice(), &viewInfo, nullptr, &this->getDepthImageView()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create depth image view!");
    }
}

void WindowContext::createFramebuffers() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());
    // Create framebuffers for each swapchain image
    std::vector<VkFramebuffer> framebuffers(this->getSwapchainImageViews().size());
    for (size_t i = 0; i < this->getSwapchainImageViews().size(); i++) {
        std::vector<VkImageView> attachments = {
            this->getSwapchainImageViews()[i],
            this->getDepthImageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vkRenderer.getGraphicCore().getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = this->getSwapExtent().width;
        framebufferInfo.height = this->getSwapExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vkRenderer.getGraphicCore().getDevice(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    this->getFramebuffers() = framebuffers;
}

void WindowContext::createCommandBuffers() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());

    std::vector<VkCommandBuffer> cmdBuffers(this->getFramebuffers().size());

    VkCommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.commandPool = vkRenderer.getGraphicCore().getCommandPool();
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());

    vkAllocateCommandBuffers(vkRenderer.getGraphicCore().getDevice(), &cmdAllocInfo, cmdBuffers.data());
    this->getCommandBuffers() = cmdBuffers;
}

void WindowContext::createFencesAndSemaphores() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());

    this->getImageAvailableSemaphores().resize(this->getMaxFramesInFlight());
    this->getRenderFinishedSemaphores().resize(this->getMaxFramesInFlight());
    this->getInFlightFences().resize(this->getMaxFramesInFlight());
    this->getImagesInFlight().resize(this->getSwapchainImages().size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start signaled so first frame doesn't hang

    for (size_t i = 0; i < this->getMaxFramesInFlight(); ++i) {
        if (vkCreateSemaphore(vkRenderer.getGraphicCore().getDevice(), &semaphoreInfo, nullptr, &this->getImageAvailableSemaphore(i)) != VK_SUCCESS ||
            vkCreateSemaphore(vkRenderer.getGraphicCore().getDevice(), &semaphoreInfo, nullptr, &this->getRenderFinishedSemaphore(i)) != VK_SUCCESS ||
            vkCreateFence(vkRenderer.getGraphicCore().getDevice(), &fenceInfo, nullptr, &this->getInFlightFence(i)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void WindowContext::createDescriptorPools() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());

    this->getDescriptorPools().resize(this->getMaxFramesInFlight());

    for (size_t i = 0; i < this->getMaxFramesInFlight(); ++i) {
        std::vector<VkDescriptorPoolSize> poolSizes = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 }
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 100;

        if (vkCreateDescriptorPool(vkRenderer.getGraphicCore().getDevice(), &poolInfo, nullptr, &this->getDescriptorPools()[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
}

void WindowContext::initialize(Window* window) {
    this->window = window;
    this->createSurfaceAndRenderPass();
    this->createSwapchainImages();
    this->createImageViews();
    this->createFramebuffers();
    this->createCommandBuffers();
    this->createFencesAndSemaphores();
    this->createDescriptorPools();

    RaeptorCogs::MainWorker().addJob([this]() {
        if (!this->getRenderer()->isInitialized()) {
            static_cast<RendererBackend*>(this->getRenderer())->initializePipeline();
        }
    });
}

void WindowContext::bind() {
    static_cast<Vulkan::RenderPipeline&>(this->getRenderer()->getRenderPipeline()).setCurrentContext(this);
}

void WindowContext::recreateSwapchain(Window* window) {
    if (!window) {
        window = this->window;
    }

    if (window->getWidth() != this->swapExtent.width || window->getHeight() != this->swapExtent.height) {
        std::cout << "Recreating swapchain for window resize to " << window->getWidth() << "x" << window->getHeight() << std::endl;
        this->destroySwapchainResources();

        // Re-initialize
        this->initialize(window);
    }
}

WindowContext::~WindowContext() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    this->destroySwapchainResources();
    vkDestroyRenderPass(vkRenderer.getGraphicCore().getDevice(), vkRenderer.getGraphicCore().getRenderPass(), nullptr);
    vkFreeCommandBuffers(vkRenderer.getGraphicCore().getDevice(), vkRenderer.getGraphicCore().getCommandPool(),
                         static_cast<uint32_t>(this->getCommandBuffers().size()), this->getCommandBuffers().data());
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroyDescriptorPool(vkRenderer.getGraphicCore().getDevice(), getDescriptorPools()[i], nullptr);
        vkDestroySemaphore(vkRenderer.getGraphicCore().getDevice(), getRenderFinishedSemaphore(i), nullptr);
        vkDestroySemaphore(vkRenderer.getGraphicCore().getDevice(), getImageAvailableSemaphore(i), nullptr);
        vkDestroyFence(vkRenderer.getGraphicCore().getDevice(), getInFlightFence(i), nullptr);
    }
}

void WindowContext::destroySwapchainResources() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());

    vkDeviceWaitIdle(vkRenderer.getGraphicCore().getDevice());

    vkDestroyImageView(vkRenderer.getGraphicCore().getDevice(), this->depthImageView, nullptr);
    vkDestroyImage(vkRenderer.getGraphicCore().getDevice(), this->depthImage, nullptr);
    vkFreeMemory(vkRenderer.getGraphicCore().getDevice(), this->depthImageMemory, nullptr);

    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(vkRenderer.getGraphicCore().getDevice(), framebuffer, nullptr);
    }

    vkDestroySwapchainKHR(vkRenderer.getGraphicCore().getDevice(), this->swapchain, nullptr);
    vkDestroySurfaceKHR(vkRenderer.getGraphicCore().getInstance(), this->surface, nullptr);
}

}
#endif // VULKAN_SUPPORTED