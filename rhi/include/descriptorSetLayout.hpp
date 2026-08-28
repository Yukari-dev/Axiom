#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class DescriptorSetLayout{
public:
  DescriptorSetLayout(VkDevice device);
  ~DescriptorSetLayout();
  
  VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_layout; }
private:
  void Create();
private:
  VkDescriptorSetLayout m_layout;
  VkDevice m_device;
};

}
