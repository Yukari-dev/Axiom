#include "descriptorSet.hpp"
#include <stdexcept>

namespace Axiom{

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSetLayout layout) : m_device(device), m_layout(layout){
  Create();
  Allocate();
}

DescriptorSet::~DescriptorSet(){
  vkDestroyDescriptorPool(m_device, m_pool, nullptr);
}

void DescriptorSet::UpdateDescriptorSet(Buffer& uniformBuffer, VkDeviceSize size){
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer.GetBuffer();
  bufferInfo.offset = 0;
  bufferInfo.range = size;

  VkWriteDescriptorSet writeSet{};
  writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeSet.descriptorCount = 1;
  writeSet.dstSet = m_set;
  writeSet.dstBinding = 0;
  writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writeSet.pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(m_device, 1, &writeSet, 0, nullptr);
}

void DescriptorSet::Create(){
  VkDescriptorPoolSize poolSize{};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.poolSizeCount = 1;
  info.pPoolSizes = &poolSize;
  info.maxSets = 1;

  VkResult result = vkCreateDescriptorPool(m_device, &info, nullptr, &m_pool);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Descriptor Pool");
}

void DescriptorSet::Allocate(){
  VkDescriptorSetAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  info.descriptorSetCount = 1;
  info.pSetLayouts = &m_layout;
  info.descriptorPool = m_pool;

  VkResult result = vkAllocateDescriptorSets(m_device, &info, &m_set);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate Descriptor Set.");
}

}
