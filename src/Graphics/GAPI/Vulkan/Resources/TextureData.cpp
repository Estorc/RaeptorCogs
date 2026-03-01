#include <cstring>
#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Memory.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/TextureData.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <cmath>

namespace RaeptorCogs::GAPI::Vulkan {

#pragma region TextureData

VkImage &TextureData::getImage() {
  return this->image;
}

VkDeviceMemory &TextureData::getMemory() {
  return this->memory;
}

VkImageView &TextureData::getImageView() {
  return this->textureView;
}

TextureData::TextureData() {}

void TextureData::build(
    int width, int height, void *data, GLenum minFilter, GLenum magFilter) {
  RendererBackend &renderer =
      static_cast<RendererBackend &>(RaeptorCogs::Renderer().getBackend());
  int mipLevels =
      static_cast<int>(std::floor(std::log2(std::max(width, height)))) + 1; // Calculate
                                                                            // number of
                                                                            // mipmap
                                                                            // levels

  VkImageCreateInfo imageInfo = {
    .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType     = VK_IMAGE_TYPE_2D,
    .format        = VK_FORMAT_R8G8B8A8_UNORM,
    .extent        = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 },
    .mipLevels     = static_cast<uint32_t>(mipLevels),
    .arrayLayers   = 1,
    .samples       = VK_SAMPLE_COUNT_1_BIT,
    .tiling        = VK_IMAGE_TILING_OPTIMAL,
    .usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
  };

  vkCreateImage(renderer.getGraphicCore().getDevice(), &imageInfo, NULL, &this->image);
  this->memory = AllocateMemory<VkImage>(
      &renderer, this->image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  VkBufferCreateInfo bufferInfo = {
    .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size        = static_cast<uint32_t>(width * height * 4),
    .usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE
  };
  vkCreateBuffer(
      renderer.getGraphicCore().getDevice(), &bufferInfo, nullptr, &stagingBuffer);
  stagingBufferMemory = AllocateMemory<VkBuffer>(
      &renderer,
      stagingBuffer,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  void *mapped;
  vkMapMemory(
      renderer.getGraphicCore().getDevice(),
      stagingBufferMemory,
      0,
      width * height * 4,
      0,
      &mapped);
  memcpy(mapped, data, width * height * 4);
  vkUnmapMemory(renderer.getGraphicCore().getDevice(), stagingBufferMemory);

  VkBufferImageCopy region {};
  region.bufferOffset                    = 0;
  region.bufferRowLength                 = 0;
  region.bufferImageHeight               = 0;
  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;
  region.imageOffset                     = { 0, 0, 0 };
  region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };

  renderer.getRenderPipeline().transitionImageLayout(
      commandBuffer,
      this->image,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_ASPECT_COLOR_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      mipLevels);

  vkCmdCopyBufferToImage(
      commandBuffer,
      stagingBuffer,
      this->image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &region);

  renderer.getRenderPipeline().transitionImageLayout(
      commandBuffer,
      this->image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_IMAGE_ASPECT_COLOR_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      mipLevels);

  EndSingleTimeCommands(commandBuffer);

  VkImageViewCreateInfo viewInfo           = {};
  viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image                           = this->image;
  viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format                          = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.levelCount     = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;

  vkCreateImageView(
      renderer.getGraphicCore().getDevice(), &viewInfo, NULL, &this->textureView);

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter           = VK_FILTER_LINEAR;
  samplerInfo.minFilter           = VK_FILTER_LINEAR;
  samplerInfo.addressModeU        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  // samplerInfo.anisotropyEnable    = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  vkCreateSampler(
      renderer.getGraphicCore().getDevice(), &samplerInfo, NULL, &this->textureSampler);
}

void TextureData::bind() {
  VkDevice device =
      static_cast<RendererBackend &>(RaeptorCogs::Renderer().getBackend())
          .getGraphicCore()
          .getDevice();

  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView             = this->textureView;
  imageInfo.sampler               = this->textureSampler;

  VkWriteDescriptorSet descriptorWrite = {};
  descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet               = NULL /* Descriptor set to update */;
  descriptorWrite.dstBinding           = 0;
  descriptorWrite.dstArrayElement      = 0;
  descriptorWrite.descriptorType       = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount      = 1;
  descriptorWrite.pImageInfo           = &imageInfo;

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void TextureData::unbind() const {}

#pragma endregion

} // namespace RaeptorCogs::GAPI::Vulkan
#endif // VULKAN_SUPPORTED