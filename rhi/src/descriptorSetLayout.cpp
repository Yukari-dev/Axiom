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
  VkDescriptorSetLayoutBinding uboBinding{};
  uboBinding.binding = 0;
  uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboBinding.descriptorCount = 1;
  uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerBinding{};
  samplerBinding.binding = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.descriptorCount = 1;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[] = {uboBinding, samplerBinding};

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 2;
  info.pBindings = bindings;

  VkResult result = vkCreateDescriptorSetLayout(m_device, &info, nullptr, &m_layout);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Descriptor Set layout");
}

}
