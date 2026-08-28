#pragma once
#include "buffer.hpp"
#include "texture.hpp"
#include <vulkan/vulkan.hpp>

namespace Axiom{

class DescriptorSet{
public:
  DescriptorSet(VkDevice device, VkDescriptorSetLayout layout);
  ~DescriptorSet();
  
  void UpdateDescriptorSet(Buffer& uniformBuffer, VkDeviceSize size, Texture& texture);

  VkDescriptorSet GetSet() const { return m_set; }
private:
  void Create();
  void Allocate();
private:
  VkDevice m_device{VK_NULL_HANDLE};
  VkDescriptorSet m_set;
  VkDescriptorPool m_pool{};
  VkDescriptorSetLayout m_layout{};
};

}
