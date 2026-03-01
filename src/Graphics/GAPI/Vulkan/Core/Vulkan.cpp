#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <cstring>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>


VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
  const std::vector<VkFormat> candidates = {
    VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
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

VkCompositeAlphaFlagBitsKHR chooseCompositeAlpha(VkSurfaceCapabilitiesKHR caps) {
  if (caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
    return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

  if (caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
    return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

  if (caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
    return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

  return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
};

// Get the first supported format from the candidates list for the given surface and else
// get the first available format The selection ensures that the first position in
// candidates is the most preferred format, and don't just return any supported format.
VkSurfaceFormatKHR findBestSurfaceFormat(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
    const std::vector<VkFormat> &candidates) {
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
  std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(
      physicalDevice, surface, &formatCount, availableFormats.data());
  VkSurfaceFormatKHR bestFormat = {
    VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
  };
  size_t bestIndex = candidates.size();
  for (const auto &availableFormat : availableFormats) {
    std::cout
        << "Available surface format: " << string_VkFormat(availableFormat.format)
        << ", color space: " << string_VkColorSpaceKHR(availableFormat.colorSpace)
        << std::endl;
    for (const VkFormat &candidate : candidates) {
      size_t candidateIndex = static_cast<size_t>(&candidate - &candidates[0]);
      if (availableFormat.format == candidate && bestIndex > candidateIndex) {
        bestFormat = availableFormat;
        bestIndex  = candidateIndex;
        break;
      }
    }
  }
  // If no suitable format is found, return the first available format
  if (!availableFormats.empty() && bestFormat.format == VK_FORMAT_UNDEFINED) {
    bestFormat = availableFormats[0];
  }

  if (bestFormat.format != VK_FORMAT_UNDEFINED) {
    std::cout
        << "Selected surface format: " << string_VkFormat(bestFormat.format) << std::endl;
    return bestFormat;
  }

  throw std::runtime_error("failed to find a suitable surface format!");
}

bool checkValidationLayerSupport(const std::vector<const char *> validationLayers) {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;
    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) return false;
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
    std::cout << " - Discrete GPU detected." << std::endl, score += 1000;
  else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    std::cout << " - Integrated GPU detected." << std::endl, score += 100;

  // Prefer higher max image dimension (proxy for performance)
  score += props.limits.maxImageDimension2D;

  // Must support required features (e.g., geometry shader)
  if (!features.geometryShader) score = 0; // unsuitable

  std::cout << " - Score: " << score << std::endl;

  return score;
}

VkCommandBuffer BeginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool =
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getCommandPool();
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getDevice(),
      &allocInfo,
      &commandBuffer);
  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo {};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &commandBuffer;

  vkQueueSubmit(
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getGraphicsQueue(),
      1,
      &submitInfo,
      VK_NULL_HANDLE);
  vkQueueWaitIdle(
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getGraphicsQueue());

  vkFreeCommandBuffers(
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getDevice(),
      static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend *>(
          &RaeptorCogs::Renderer().getBackend())
          ->getGraphicCore()
          .getCommandPool(),
      1,
      &commandBuffer);
}

#endif // VULKAN_SUPPORTED