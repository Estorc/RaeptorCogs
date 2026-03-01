#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Memory.hpp>
#include <vulkan/vulkan.h>

namespace RaeptorCogs::GAPI::Vulkan {

template <typename T>
VkDeviceMemory
AllocateMemory(RaeptorCogs::GAPI::Vulkan::RendererBackend *, T, VkMemoryPropertyFlags) {
  static_assert(sizeof(T) == 0, "AllocateMemory not specialized for this type");
  return VK_NULL_HANDLE;
}

template <>
VkDeviceMemory AllocateMemory<VkMemoryRequirements>(
    RaeptorCogs::GAPI::Vulkan::RendererBackend *renderer,
    VkMemoryRequirements memRequirements, VkMemoryPropertyFlags properties) {
  VkMemoryAllocateInfo memAllocInfo {};
  memAllocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.allocationSize = memRequirements.size;

  // Find suitable memory type
  auto findMemoryType =
      [&](uint32_t typeFilter, VkMemoryPropertyFlags properties) -> uint32_t {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(
        renderer->getGraphicCore().getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
        return i;
      }
    }

    throw std::runtime_error("failed to find suitable memory type!");
  };

  memAllocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  VkDeviceMemory memory;
  vkAllocateMemory(
      renderer->getGraphicCore().getDevice(), &memAllocInfo, nullptr, &memory);
  return memory;
}

template <>
VkDeviceMemory AllocateMemory<VkImage>(
    RaeptorCogs::GAPI::Vulkan::RendererBackend *renderer, VkImage image,
    VkMemoryPropertyFlags properties) {
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(
      renderer->getGraphicCore().getDevice(), image, &memRequirements);
  VkDeviceMemory memory = AllocateMemory(renderer, memRequirements, properties);
  vkBindImageMemory(renderer->getGraphicCore().getDevice(), image, memory, 0);
  return memory;
}

template <>
VkDeviceMemory AllocateMemory<VkBuffer>(
    RaeptorCogs::GAPI::Vulkan::RendererBackend *renderer, VkBuffer buffer,
    VkMemoryPropertyFlags properties) {
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(
      renderer->getGraphicCore().getDevice(), buffer, &memRequirements);
  VkDeviceMemory memory = AllocateMemory(renderer, memRequirements, properties);
  vkBindBufferMemory(renderer->getGraphicCore().getDevice(), buffer, memory, 0);
  return memory;
}

void CopyBuffer(
    RaeptorCogs::GAPI::Vulkan::RendererBackend *renderer, VkBuffer srcBuffer,
    VkBuffer dstBuffer, VkDeviceSize size, VkCommandBuffer cmd) {
  VkCommandBufferAllocateInfo cmdAlloc {};
  cmdAlloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdAlloc.commandPool        = renderer->getGraphicCore().getCommandPool();
  cmdAlloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdAlloc.commandBufferCount = 1;

  VkBufferCopy copyRegion {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size      = size;

  vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion);
};

} // namespace RaeptorCogs::GAPI::Vulkan

#endif // VULKAN_SUPPORTED