#include "descriptorSetLayout.hpp"
#include <stdexcept>

namespace Axiom{

DescriptorSetLayout::DescriptorSetLayout(VkDevice device) : m_device(device){
  Create();
}

DescriptorSetLayout::~DescriptorSetLayout(){
  vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
}

void DescriptorSetLayout::Create(){
  VkDescriptorSetLayoutBinding binding{};
  binding.binding = 0;
  binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  binding.descriptorCount = 1;
  binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings = &binding;

  VkResult result = vkCreateDescriptorSetLayout(m_device, &info, nullptr, &m_layout);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Descriptor Set layout");
}

}
