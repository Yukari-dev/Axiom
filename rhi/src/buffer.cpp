#include "buffer.hpp"
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Axiom{

Buffer::Buffer(
  VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size,
  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties
) : m_physicalDevice(physicalDevice), m_device(device){
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if(vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
    throw std::runtime_error("Failed to create buffer.");

  VkMemoryRequirements mem{};
  vkGetBufferMemoryRequirements(m_device, m_buffer, &mem);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = mem.size;
  allocInfo.memoryTypeIndex = FindMemoryType(mem.memoryTypeBits, properties);

  if(vkAllocateMemory(m_device, &allocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate buffer memory.");
  vkBindBufferMemory(m_device, m_buffer, m_deviceMemory, 0);
}

Buffer::Buffer(Buffer&& other) noexcept
  : m_physicalDevice(other.m_physicalDevice), m_device(other.m_device),
  m_buffer(other.m_buffer), m_deviceMemory(other.m_deviceMemory){
  other.m_buffer = VK_NULL_HANDLE;
  other.m_deviceMemory = VK_NULL_HANDLE;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept{
  if(this != &other){
    vkDestroyBuffer(m_device,  m_buffer, nullptr);
    vkFreeMemory(m_device, m_deviceMemory, nullptr);

    m_physicalDevice = other.m_physicalDevice;
    m_device = other.m_device;
    m_buffer = other.m_buffer;
    m_deviceMemory = other.m_deviceMemory;

    other.m_buffer = VK_NULL_HANDLE;
    other.m_deviceMemory = VK_NULL_HANDLE;
  }
  return *this;
}

Buffer::~Buffer(){
  if(m_buffer != VK_NULL_HANDLE)
    vkDestroyBuffer(m_device, m_buffer, nullptr);
  if(m_deviceMemory != VK_NULL_HANDLE)
    vkFreeMemory(m_device, m_deviceMemory, nullptr);
}

void Buffer::CopyData(const void *data, VkDeviceSize size){
  void *mapped;
  vkMapMemory(m_device, m_deviceMemory, 0, size, 0, &mapped);
  memcpy(mapped, data, static_cast<size_t>(size));
  vkUnmapMemory(m_device, m_deviceMemory);
}

void Buffer::CopyBufferTo(Buffer& dest, VkDeviceSize size, VkCommandPool commandPool, VkQueue graphicsQueue){
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, m_buffer, dest.GetBuffer(), 1, &copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  
  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(m_device, commandPool, 1, &commandBuffer);
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
