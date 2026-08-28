#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

enum class BufferType{
  VERTEX, INDEX
};

class Buffer{
public:
  Buffer(
    VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size,
    VkBufferUsageFlags usage, VkMemoryPropertyFlags properties
  );
  ~Buffer();

  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;

  void CopyData(const void *data, VkDeviceSize size);
  void CopyBufferTo(Buffer& dest, VkDeviceSize size, VkCommandPool commandPool, VkQueue graphicsQueue);

  VkBuffer GetBuffer() const { return m_buffer; }
  template <typename T>
  static Buffer CreateDeviceLocalBuffer(
    VkPhysicalDevice physicalDevice, VkDevice device,
    VkCommandPool commandPool, VkQueue graphicsQueue,
    const std::vector<T>& data,
    VkBufferUsageFlags targetUsage
  ){
    VkDeviceSize size = sizeof(data[0]) * data.size();

    Buffer stagingBuffer(
      physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    
    stagingBuffer.CopyData(data.data(), size);

    Buffer deviceLocalBuffer(
      physicalDevice, device, size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | targetUsage,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    stagingBuffer.CopyBufferTo(deviceLocalBuffer, size, commandPool, graphicsQueue);
    
    return deviceLocalBuffer;
  }
private:
  uint32_t FindMemoryType(uint32_t filterType, VkMemoryPropertyFlags properties);
private:
  VkPhysicalDevice m_physicalDevice{};
  VkDevice m_device{};
  VkBuffer m_buffer{};
  VkDeviceMemory m_deviceMemory{};
};

}
