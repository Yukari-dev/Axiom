#pragma once
#include "device.hpp"
#include <cstdint>
#include <string>
#include <vulkan/vulkan.hpp>

namespace Axiom {

class Texture {
public:
  Texture(
    Device& device,
    VkCommandPool commandPool,
    const std::string& path
  );
  ~Texture();

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  VkSampler GetSampler() const { return m_sampler; }
  VkImageView GetImageView() const { return m_imageView; }

private:
  void CreateImage(uint32_t width, uint32_t height, unsigned char *pixels, VkDeviceSize size);
  void CreateImageView();
  void CreateSampler();
  void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
  void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

  uint32_t FindMemoryType(uint32_t filterType, VkMemoryPropertyFlags properties);
private:
  Device& m_device;
  VkCommandPool m_commandPool{};

  VkImage m_image{};
  VkDeviceMemory m_imageMemory{};
  VkImageView m_imageView{};
  VkSampler m_sampler{};
};

}
