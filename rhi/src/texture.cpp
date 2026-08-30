#include "texture.hpp"
#include "buffer.hpp"
#include <stb/stb_image.h>
#include <stdexcept>
#include <string>

namespace Axiom {

Texture::Texture(
  Device& device,
  VkCommandPool commandPool,
  const std::string& path
) : m_device(device), m_commandPool(commandPool){

  int width, height, channels;

  std::string fullPath = std::string(AXIOM_TEXTURE_DIR) + path;

  stbi_uc *pixels = stbi_load(fullPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if(!pixels) throw std::runtime_error("Failed to load texture: " + fullPath);

  VkDeviceSize size = width * height * 4;
  Buffer staging(
    m_device.GetPhysicalDevice(), m_device.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  staging.CopyData(pixels, size);
  
  stbi_image_free(pixels);

  CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB);
  TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(staging.GetBuffer(), width, height);
  TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  CreateImageView(VK_FORMAT_R8G8B8A8_SRGB);
  CreateSampler();
}

Texture::Texture(
  Device& device,
  VkCommandPool commandPool,
  const uint8_t* pixels,
  uint32_t width,
  uint32_t height,
  VkFormat format
) : m_device(device), m_commandPool(commandPool) {
  uint32_t bytesPerPixel = (format == VK_FORMAT_R8_UNORM) ? 1 : 4;
  VkDeviceSize size = width * height * bytesPerPixel;

  Buffer staging(
      m_device.GetPhysicalDevice(), m_device.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  staging.CopyData(pixels, size);

  CreateImage(width, height, format);
  TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  CopyBufferToImage(staging.GetBuffer(), width, height);
  TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  CreateImageView(format);
  CreateSampler();
}

Texture::~Texture() {
  vkDestroySampler(m_device.GetDevice(), m_sampler, nullptr);
  vkDestroyImageView(m_device.GetDevice(), m_imageView, nullptr);
  vkDestroyImage(m_device.GetDevice(), m_image, nullptr);
  vkFreeMemory(m_device.GetDevice(), m_imageMemory, nullptr);
}

void Texture::CreateImage(uint32_t width, uint32_t height, VkFormat format){
  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent = {width, height, 1};
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.format = format;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.samples = VK_SAMPLE_COUNT_1_BIT;

  if(vkCreateImage(m_device.GetDevice(), &info, nullptr, &m_image) != VK_SUCCESS)
    throw std::runtime_error("Failed to create Image");

  VkMemoryRequirements mem;
  vkGetImageMemoryRequirements(m_device.GetDevice(), m_image, &mem);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = mem.size;
  allocInfo.memoryTypeIndex = FindMemoryType(mem.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  if(vkAllocateMemory(m_device.GetDevice(), &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate image memory");
  vkBindImageMemory(m_device.GetDevice(), m_image, m_imageMemory, 0);
}

void Texture::CreateImageView(VkFormat format){
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = m_image;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = format;
  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.layerCount = 1;
  info.subresourceRange.levelCount = 1;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  
  VkResult result = vkCreateImageView(m_device.GetDevice(), &info, nullptr, &m_imageView);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Image view.");
}

void Texture::CreateSampler(){
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.magFilter = VK_FILTER_LINEAR;
  info.minFilter = VK_FILTER_LINEAR;
  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info.anisotropyEnable = VK_FALSE;
  info.maxAnisotropy = 1.0f;
  info.compareEnable = VK_FALSE;
  info.compareOp = VK_COMPARE_OP_ALWAYS;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.unnormalizedCoordinates = VK_FALSE;
  info.mipLodBias = 0.0f;
  info.minLod = 0.0f;
  info.maxLod = 0.0f;

  VkResult result = vkCreateSampler(m_device.GetDevice(), &info, nullptr, &m_sampler);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Sampler.");
}

void Texture::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height){
  VkCommandBuffer cmd = m_device.BeginSingleTimeCommands(m_commandPool);
  
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(cmd, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  m_device.EndSingleTimeCommands(cmd, m_commandPool, m_device.GetGraphicsQueue());
}

void Texture::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout){
  VkCommandBuffer cmd = m_device.BeginSingleTimeCommands(m_commandPool);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.image = m_image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;

  VkPipelineStageFlags srcStage{}, dstStage{};

  if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else
    throw std::runtime_error("Unsupported layout transition.");

  vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  m_device.EndSingleTimeCommands(cmd, m_commandPool, m_device.GetGraphicsQueue());
}

uint32_t Texture::FindMemoryType(uint32_t filterType, VkMemoryPropertyFlags properties){
  VkPhysicalDeviceMemoryProperties memProp;
  vkGetPhysicalDeviceMemoryProperties(m_device.GetPhysicalDevice(), &memProp);

  for(uint32_t i = 0; i < memProp.memoryTypeCount; i++){
    if((filterType & (1 << i)) && (memProp.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

}
