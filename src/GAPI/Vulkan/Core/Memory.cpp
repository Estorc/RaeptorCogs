#include <RaeptorCogs/GAPI/Vulkan/Core/Memory.hpp>
#include <vulkan/vulkan.h>

template<typename T>
VkDeviceMemory AllocateMemory(RaeptorCogs::GAPI::Vulkan::RendererBackend*, T) {
    static_assert(sizeof(T) == 0, "AllocateMemory not specialized for this type");
    return VK_NULL_HANDLE;
}

template<>
VkDeviceMemory AllocateMemory<VkMemoryRequirements>(RaeptorCogs::GAPI::Vulkan::RendererBackend* renderer, VkMemoryRequirements memRequirements) {
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;

    // Find suitable memory type
    auto findMemoryType = [&](uint32_t typeFilter, VkMemoryPropertyFlags properties) -> uint32_t {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(renderer->getGraphicCore().getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    };

    memAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkDeviceMemory memory;
    vkAllocateMemory(renderer->getGraphicCore().getDevice(), &memAllocInfo, nullptr, &memory);
    return memory;
}


template<>
VkDeviceMemory AllocateMemory<VkImage>(RaeptorCogs::GAPI::Vulkan::RendererBackend* renderer, VkImage image) {
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderer->getGraphicCore().getDevice(), image, &memRequirements);
    VkDeviceMemory memory = AllocateMemory(renderer, memRequirements);
    vkBindImageMemory(renderer->getGraphicCore().getDevice(), image, memory, 0);
    return memory;
}

template<>
VkDeviceMemory AllocateMemory<VkBuffer>(RaeptorCogs::GAPI::Vulkan::RendererBackend* renderer, VkBuffer buffer) {
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderer->getGraphicCore().getDevice(), buffer, &memRequirements);
    VkDeviceMemory memory = AllocateMemory(renderer, memRequirements);
    vkBindBufferMemory(renderer->getGraphicCore().getDevice(), buffer, memory, 0);
    return memory;
}