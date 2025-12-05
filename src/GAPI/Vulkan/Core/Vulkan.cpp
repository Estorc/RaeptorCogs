#include <RaeptorCogs/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <cstring>
#include <iostream>

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
    const std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported depth format!");
}

// Get the first supported format from the candidates list for the given surface and else get the first available format
// The selection ensures that the first position in candidates is the most preferred format, and don't just return any supported format.
VkSurfaceFormatKHR findBestSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<VkFormat>& candidates) {
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, availableFormats.data());
    VkSurfaceFormatKHR bestFormat = {
        VK_FORMAT_UNDEFINED,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };
    size_t bestIndex = candidates.size();
    for (const auto& availableFormat : availableFormats) {
        std::cout << "Available surface format: " << string_VkFormat(availableFormat.format) << ", color space: " << string_VkColorSpaceKHR(availableFormat.colorSpace) << std::endl;
        for (const VkFormat& candidate : candidates) {
            size_t candidateIndex = static_cast<size_t>(&candidate - &candidates[0]);
            if (availableFormat.format == candidate && bestIndex > candidateIndex) {
                bestFormat = availableFormat;
                bestIndex = candidateIndex;
                break;
            }
        }
    }
    // If no suitable format is found, return the first available format
    if (!availableFormats.empty() && bestFormat.format == VK_FORMAT_UNDEFINED) {
        bestFormat = availableFormats[0];
    }

    if (bestFormat.format != VK_FORMAT_UNDEFINED) {
        std::cout << "Selected surface format: " << string_VkFormat(bestFormat.format) << std::endl;
        return bestFormat;
    }

    throw std::runtime_error("failed to find a suitable surface format!");
}

bool checkValidationLayerSupport(const std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }
    return true;
}

int rateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    std::cout << "Device Name: " << props.deviceName << std::endl;

    int score = 0;

    // Prefer discrete GPUs
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        std::cout << " - Discrete GPU detected." << std::endl,
        score += 1000;
    else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        std::cout << " - Integrated GPU detected." << std::endl,
        score += 100;

    // Prefer higher max image dimension (proxy for performance)
    score += props.limits.maxImageDimension2D;

    // Must support required features (e.g., geometry shader)
    if (!features.geometryShader)
        score = 0; // unsuitable

    std::cout << " - Score: " << score << std::endl;
        
    return score;
}