#define GLFW_INCLUDE_VULKAN
#include <RaeptorCogs/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <GLFW/glfw3.h>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

namespace RaeptorCogs::GAPI::Vulkan {

VkDebugUtilsMessengerEXT debugMessenger;

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    (void)messageType;
    (void)pUserData;
    std::cerr << "[VULKAN] " << pCallbackData->pMessage << std::endl;
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        throw std::runtime_error("Vulkan validation layer reported an error.");
    }
    return VK_FALSE;
}
    
void RendererBackend::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Vulkan-specific initialization code would go here
    std::vector<const char*> extensions;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    if (!glfwVulkanSupported()) {
        if (glfwGetError(NULL) == GLFW_NOT_INITIALIZED) {
            std::cerr << "GLFW is not initialized!" << std::endl;
        } else {
            std::cerr << "Vulkan loader not found!" << std::endl;
        }
        std::cerr << "GLFW Vulkan support is not available" << std::endl;
        return;
    }
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensions) {
        std::cerr << "Failed to get required GLFW Vulkan extensions" << std::endl;
        return;
    }

    extensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
    // Add the debug utils extension
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Optional: Enable validation layers
    std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "RaeptorCogs App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "RaeptorCogs";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();


    if (checkValidationLayerSupport(validationLayers)) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        std::cerr << "Validation layers requested, but not available!" << std::endl;
    }

    vkCreateInstance(&createInfo, NULL, &this->graphicCore.getInstance());

    // Debug messenger setup

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(this->graphicCore.getInstance(), "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        std::cout << "Setting up Vulkan debug messenger." << std::endl;
        func(this->graphicCore.getInstance(), &debugCreateInfo, nullptr, &debugMessenger);
    } else {
        std::cerr << "Could not set up debug messenger!" << std::endl;
    }

    // Get physical device, logical device, queues, etc.

    this->graphicCore.getPhysicalDevice() = VK_NULL_HANDLE;
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(this->graphicCore.getInstance(), &physicalDeviceCount, NULL);
    if (physicalDeviceCount == 0) {
        std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
        return;
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(this->graphicCore.getInstance(), &physicalDeviceCount, physicalDevices.data());
    // Find the best suitable physical device
    int bestScore = 0;
    for (const auto& device : physicalDevices) {
        int score = rateDeviceSuitability(device);
        if (score >= bestScore) {
            bestScore = score;
            this->graphicCore.getPhysicalDevice() = device;
        }
    }
    if (this->graphicCore.getPhysicalDevice() == VK_NULL_HANDLE) {
        std::cerr << "Failed to find a suitable GPU!" << std::endl;
        return;
    } else {
        std::cout << "Selected GPU: ";
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(this->graphicCore.getPhysicalDevice(), &deviceProperties);
        std::cout << deviceProperties.deviceName << std::endl;
    }

    // Create queues
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(this->graphicCore.getPhysicalDevice(), &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(this->graphicCore.getPhysicalDevice(), &queueFamilyCount, queueFamilies.data());

    /**
     * TODO: Ensure that the selected queue family supports both graphics and presentation.
     * For simplicity, we assume that graphics queue also supports presentation.
     */

    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    uint32_t graphicsQueueFamilyIndex = 0;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            float queuePriority = 1.0f;
            graphicsQueueFamilyIndex = i;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = 1;
            deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
            deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

            vkCreateDevice(this->graphicCore.getPhysicalDevice(), &deviceCreateInfo, nullptr, &this->graphicCore.getDevice());
            vkGetDeviceQueue(this->graphicCore.getDevice(), i, 0, &this->graphicCore.getGraphicsQueue());
            vkGetDeviceQueue(this->graphicCore.getDevice(), i, 0, &this->graphicCore.getPresentQueue());
            break;
        }
    }

    // Create pipeline cache
    VkPipelineCacheCreateInfo cacheInfo{};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    vkCreatePipelineCache(this->graphicCore.getDevice(), &cacheInfo, nullptr, &this->graphicCore.getPipelineCache());

    // Render pass setup
    // Do it when the first window is created

    // Command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        // allows resetting command buffers individually

    if (vkCreateCommandPool(this->graphicCore.getDevice(), &poolInfo, nullptr, &this->graphicCore.getCommandPool()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    // Descriptor pool
    std::array<VkDescriptorPoolSize, 11> pool_sizes = {
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * static_cast<uint32_t>(pool_sizes.size());
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    vkCreateDescriptorPool(this->graphicCore.getDevice(), &pool_info, nullptr, &this->imGui.getImGuiDescriptorPool());

    std::cout << "Vulkan Renderer Backend initialized." << std::endl;
}







void recordCommandBuffer(RaeptorCogs::GAPI::Vulkan::RendererBackend* backend, RaeptorCogs::GAPI::Vulkan::WindowContext* context, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) {
    // Begin recording
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    float value = 0.1f;
    value = std::pow(value, 2.2f); // encode to sRGB
    // Set clear color
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{value, value, value, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = backend->getGraphicCore().getRenderPass();
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = context->getSwapExtent();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // >>> Your draw commands go here <<<
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    

    vkCmdEndRenderPass(commandBuffer);

    // Finish recording
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}


void RendererBackend::render(Window* window, int x, int y, int width, int height) {
    (void) width;
    (void) height;
    (void) x;
    (void) y;
    auto* context = static_cast<RaeptorCogs::GAPI::Vulkan::WindowContext*>(window->getContext());
    VkDevice device = this->graphicCore.getDevice();

    // 1️⃣ Wait for the previous frame to finish
    vkWaitForFences(device, 1, &context->getInFlightFence(context->getCurrentFrame()), VK_TRUE, UINT64_MAX);

    // 2️⃣ Acquire next image
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device,
                                            context->getSwapchain(),
                                            UINT64_MAX,
                                            context->getImageAvailableSemaphore(context->getCurrentFrame()),
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        context->recreateSwapchain();
        return;
    }

    // 3️⃣ If this image is already being rendered, wait for it
    if (context->getImageInFlight(imageIndex) != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &context->getImageInFlight(imageIndex), VK_TRUE, UINT64_MAX);
    }

    // Mark this image as now in flight with this frame’s fence
    context->getImageInFlight(imageIndex) = context->getInFlightFence(context->getCurrentFrame());

    // 4️⃣ Reset the fence and command buffer for this frame
    vkResetFences(device, 1, &context->getInFlightFence(context->getCurrentFrame()));
    vkResetCommandBuffer(context->getCommandBuffers()[imageIndex], 0);

    // 5️⃣ Record commands
    ImGui::Render();
    recordCommandBuffer(this, context, context->getCommandBuffers()[imageIndex], context->getFramebuffers()[imageIndex]);

    // 6️⃣ Submit draw command
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { context->getImageAvailableSemaphore(context->getCurrentFrame()) };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context->getCommandBuffers()[imageIndex];

    VkSemaphore signalSemaphores[] = { context->getRenderFinishedSemaphore(context->getCurrentFrame()) };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(this->graphicCore.getGraphicsQueue(), 1, &submitInfo, context->getInFlightFence(context->getCurrentFrame())) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 7️⃣ Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context->getSwapchain();
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(this->graphicCore.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        context->recreateSwapchain();
    }

    // 8️⃣ Advance frame index
    context->advanceFrame();
}

void RendererBackend::render(Texture& texture, int x, int y, int width, int height) {
    (void) texture;
    (void) x;
    (void) y;
    (void) width;
    (void) height;
    // Vulkan-specific rendering code for texture would go here
}

Common::WindowContext* RendererBackend::createWindowContext() {
    return new WindowContext(this);
}

bool RendererBackend::isInitialized() const {
    // Vulkan-specific code to check initialization status would go here
    return !this->getPlatform().getWindows().empty();
}

};