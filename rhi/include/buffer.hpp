#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

enum class BufferType{
  VERTEX, INDEX
};

class Buffer{
public:
  Buffer(
    VkPhysicalDevice physicalDevice, VkDevice device,
    VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
  );
  ~Buffer();

  void CopyData(const void *data, VkDeviceSize size);

  VkBuffer GetBuffer() const { return m_buffer; }
private:
  uint32_t FindMemoryType(uint32_t filterType, VkMemoryPropertyFlags properties);
private:
  VkPhysicalDevice m_physicalDevice{};
  VkDevice m_device{};
  VkBuffer m_buffer{};
  VkDeviceMemory m_deviceMemory{};
};

}
