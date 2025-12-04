#include <RaeptorCogs/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Memory.hpp>
#include <RaeptorCogs/Window.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::Vulkan {

    void WindowContext::initialize(Window* window) {
        this->window = window;
        RendererBackend& vkRenderer = static_cast<RendererBackend&>(*this->getRenderer());

        // Create Vulkan surface for the window
        if (glfwCreateWindowSurface(vkRenderer.getGraphicCore().getInstance(), window->getGLFWWindow(), nullptr, &this->surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

        // Query surface capabilities
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkRenderer.getGraphicCore().getPhysicalDevice(), this->surface, &capabilities);

        // Get surface format
        VkSurfaceFormatKHR surfaceFormat = findBestSurfaceFormat(vkRenderer.getGraphicCore().getPhysicalDevice(), this->surface);

        if (vkRenderer.getGraphicCore().getRenderPass() == VK_NULL_HANDLE) vkRenderer.getGraphicCore().createRenderPass(surfaceFormat.format);

        // Choose swap extent (same as window size)
        this->swapExtent = { static_cast<uint32_t>(window->getWidth()), static_cast<uint32_t>(window->getHeight()) };

        // Create swapchain
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = this->surface;
        swapchainInfo.minImageCount = 2; // Double buffering
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = swapExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.preTransform = capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        swapchainInfo.clipped = VK_TRUE;

        vkCreateSwapchainKHR(vkRenderer.getGraphicCore().getDevice(), &swapchainInfo, nullptr, &this->swapchain);

        // Get swapchain images
        uint32_t imageCount;
        vkGetSwapchainImagesKHR(vkRenderer.getGraphicCore().getDevice(), this->swapchain, &imageCount, nullptr);
        std::vector<VkImage> swapchainImages(imageCount);
        vkGetSwapchainImagesKHR(vkRenderer.getGraphicCore().getDevice(), this->swapchain, &imageCount, swapchainImages.data());

        // Create image views
        std::vector<VkImageView> swapchainImageViews(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = surfaceFormat.format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;
            vkCreateImageView(vkRenderer.getGraphicCore().getDevice(), &viewInfo, nullptr, &swapchainImageViews[i]);
        }

        // Create depth image view
        VkFormat depthFormat = findDepthFormat(vkRenderer.getGraphicCore().getPhysicalDevice());
        // 1. Create the image
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapExtent.width;
        imageInfo.extent.height = swapExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(vkRenderer.getGraphicCore().getDevice(), &imageInfo, nullptr, &this->depthImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create depth image!");
        }

        // 2. Allocate memory
        this->depthImageMemory = AllocateMemory(&vkRenderer, this->depthImage);

        // 3. Create image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vkRenderer.getGraphicCore().getDevice(), &viewInfo, nullptr, &this->depthImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create depth image view!");
        }

        // Create framebuffers for each swapchain image
        std::vector<VkFramebuffer> framebuffers(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            std::vector<VkImageView> attachments = {
                swapchainImageViews[i],
                this->depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vkRenderer.getGraphicCore().getRenderPass();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapExtent.width;
            framebufferInfo.height = swapExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vkRenderer.getGraphicCore().getDevice(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
        this->framebuffers = framebuffers;

        std::vector<VkCommandBuffer> cmdBuffers(framebuffers.size());

        VkCommandBufferAllocateInfo cmdAllocInfo{};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.commandPool = vkRenderer.getGraphicCore().getCommandPool();
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());

        vkAllocateCommandBuffers(vkRenderer.getGraphicCore().getDevice(), &cmdAllocInfo, cmdBuffers.data());
        this->commandBuffers = cmdBuffers;

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // start signaled so first frame doesn't hang

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(vkRenderer.getGraphicCore().getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(vkRenderer.getGraphicCore().getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(vkRenderer.getGraphicCore().getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void WindowContext::bind() {
        // In Vulkan, binding is handled via command buffers, so this function may be empty
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
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(vkRenderer.getGraphicCore().getDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(vkRenderer.getGraphicCore().getDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(vkRenderer.getGraphicCore().getDevice(), inFlightFences[i], nullptr);
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