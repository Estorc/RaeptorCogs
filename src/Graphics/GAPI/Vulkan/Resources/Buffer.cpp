#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Buffer.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Memory.hpp>
#include <cstring>

namespace RaeptorCogs::GAPI::Vulkan {

VkBuffer& Buffer::getBuffer() {
    return this->buffer;
}

VkDeviceMemory& Buffer::getMemory() {
    return this->memory;
}

void Buffer::vkBuild(const void* data, size_t size, VkBufferUsageFlags usage) {
    RendererBackend& backend = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    VkDevice &device = backend.getGraphicCore().getDevice();

    VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer stagingBuffer;
    vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    VkDeviceMemory stagingBufferMemory = AllocateMemory<VkBuffer>(&backend, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *memData;
    vkMapMemory(device, stagingBufferMemory, 0, bufferInfo.size, 0, &memData);
    memcpy(memData, data, (size_t)bufferInfo.size);
    vkUnmapMemory(device, stagingBufferMemory);

    bufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;


    vkCreateBuffer(device, &bufferInfo, nullptr, &this->buffer);
    this->memory = AllocateMemory<VkBuffer>(&backend, this->buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkCommandBuffer cmd = BeginSingleTimeCommands();
    CopyBuffer(&backend, stagingBuffer, this->buffer, size, cmd);
    EndSingleTimeCommands(cmd);


    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

Buffer::~Buffer() {
    RendererBackend& backend = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    VkDevice &device = backend.getGraphicCore().getDevice();

    if (this->buffer != nullptr) {
        vkDestroyBuffer(device, this->buffer, nullptr);
        this->buffer = nullptr;
    }
    if (this->memory != nullptr) {
        vkFreeMemory(device, this->memory, nullptr);
        this->memory = nullptr;
    }
}

Framebuffer::Framebuffer() {

}
void Framebuffer::bind() {

}
void Framebuffer::unbind() const {

}


Renderbuffer::Renderbuffer() {

}

void Renderbuffer::bind() {

}
void Renderbuffer::unbind() const {

}




Vertexbuffer::Vertexbuffer() {
    //
}
void Vertexbuffer::bind() {
    RendererBackend& backend = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(backend.getRenderPipeline().getVulkanFrame().commandBuffer, 0, 1, &this->getBuffer(), offsets);
}
void Vertexbuffer::unbind() const {
    //
}
void Vertexbuffer::build(const void* data, size_t size) {
    std::cout << "Building Vulkan Vertex Buffer of size " << size << " bytes." << std::endl;
    this->vkBuild(data, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}





Elementbuffer::Elementbuffer() {
    //
}
void Elementbuffer::bind() {
    RendererBackend& backend = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    vkCmdBindIndexBuffer(backend.getRenderPipeline().getVulkanFrame().commandBuffer, this->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
}
void Elementbuffer::unbind() const {
    //
}
void Elementbuffer::build(const void* data, size_t size) {
    this->vkBuild(data, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}





ShaderStorageBuffer::ShaderStorageBuffer() {
    //
}
void ShaderStorageBuffer::bind() {

}
void ShaderStorageBuffer::unbind() const {

}

void ShaderStorageBuffer::allocate(size_t size) {
    (void)size;
    // Vulkan-specific buffer allocation code would go here
}

void ShaderStorageBuffer::setBindingPoint(unsigned int bindingPoint) {
    (void)bindingPoint;
    // Vulkan-specific code to bind the buffer to a binding point would go here
}

void ShaderStorageBuffer::upload(const void* data, size_t size, size_t offset) {
    (void)data;
    (void)size;
    (void)offset;
}

}
#endif // VULKAN_SUPPORTED