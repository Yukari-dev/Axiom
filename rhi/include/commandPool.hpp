#pragma once
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Axiom {

class CommandPool {
public:
  CommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex);
  ~CommandPool();

  VkCommandPool GetHandler() const { return m_commandPool; }
  VkCommandBuffer AllocateCommandBuffer();
private:
  void Create(uint32_t graphicsQueueFamilyIndex);
private:
  VkDevice m_device;
  VkCommandPool m_commandPool;
};

} // namespace Axiom
