#pragma once
#include "buffer.hpp"
#include "commandBuffer.hpp"
#include "vertex.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Mesh{
public:
  Mesh(VkPhysicalDevice pDevice, VkDevice device);

  void SetData(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
  void Bind(CommandBuffer& cmd);
  uint32_t GetIndexCount() const { return m_indexCount; };
private:
  void EnsureCapacity(VkDeviceSize vertSize, VkDeviceSize indSize);

private:
  VkPhysicalDevice m_physicalDevice{};
  VkDevice m_device{};

  std::unique_ptr<Buffer> m_vertexBuffer{nullptr};
  std::unique_ptr<Buffer> m_indexBuffer{nullptr};

  VkDeviceSize m_verticesCapacity{0};
  VkDeviceSize m_indicesCapacity{0};
  uint32_t m_indexCount = 0;
};

}
