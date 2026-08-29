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

void DescriptorSet::UpdateDescriptorSet(Buffer& uniformBuffer, VkDeviceSize size, Texture& texture){
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer.GetBuffer();
  bufferInfo.offset = 0;
  bufferInfo.range = size;

  VkDescriptorImageInfo samplerInfo{};
  samplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  samplerInfo.sampler = texture.GetSampler();
  samplerInfo.imageView = texture.GetImageView();

  VkWriteDescriptorSet writes[2]{};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].descriptorCount = 1;
  writes[0].dstSet = m_set;
  writes[0].dstBinding = 0;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes[0].pBufferInfo = &bufferInfo;

  writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[1].descriptorCount = 1;
  writes[1].dstSet = m_set;
  writes[1].dstBinding = 1;
  writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writes[1].pImageInfo = &samplerInfo;

  vkUpdateDescriptorSets(m_device, 2, writes, 0, nullptr);
}

void DescriptorSet::UpdateDescriptorSet(Buffer& uniformBuffer, VkDeviceSize size){
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = uniformBuffer.GetBuffer();
  bufferInfo.offset = 0;
  bufferInfo.range = size;

  VkWriteDescriptorSet writes[1]{};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].descriptorCount = 1;
  writes[0].dstSet = m_set;
  writes[0].dstBinding = 0;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes[0].pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(m_device, 1, writes, 0, nullptr);
}

void DescriptorSet::Create(){
  VkDescriptorPoolSize poolSizes[2]{};

  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = 1;

  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = 1;

  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.poolSizeCount = 2;
  info.pPoolSizes = poolSizes;
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
