#include "buffer.hpp"
#include <cstring>
#include <stdexcept>

namespace Axiom{

Buffer::Buffer(
    VkPhysicalDevice physicalDevice, VkDevice device,
    VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
) : m_physicalDevice(physicalDevice), m_device(device){
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = size;
  info.usage = usage;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if(vkCreateBuffer(m_device, &info, nullptr, &m_buffer) != VK_SUCCESS)
    throw std::runtime_error("Failed to create buffer.");

  VkMemoryRequirements memReq;
  vkGetBufferMemoryRequirements(m_device, m_buffer, &memReq);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memReq.size;
  allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);
  
  if(vkAllocateMemory(m_device, &allocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate buffer memory");

  vkBindBufferMemory(m_device, m_buffer, m_deviceMemory, 0);
}

Buffer::~Buffer(){
  vkDestroyBuffer(m_device, m_buffer, nullptr);
  vkFreeMemory(m_device, m_deviceMemory, nullptr);
}

void Buffer::CopyData(const void *data, VkDeviceSize size){
  void *mapped;
  vkMapMemory(m_device, m_deviceMemory, 0, size, 0, &mapped);
  memcpy(mapped, data, static_cast<size_t>(size));
  vkUnmapMemory(m_device, m_deviceMemory);
}

uint32_t Buffer::FindMemoryType(uint32_t filterType, VkMemoryPropertyFlags properties){
  VkPhysicalDeviceMemoryProperties memProp;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProp);

  for(uint32_t i = 0; i < memProp.memoryTypeCount; i++){
    if((filterType & (1 << i)) && (memProp.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }

  throw std::runtime_error("Failed to find suitable memory type");
}


}
