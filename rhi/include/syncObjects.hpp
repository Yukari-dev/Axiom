#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class SyncObjects{
public:
  SyncObjects(VkDevice device, uint32_t swapChainImageCount);
  ~SyncObjects();
  
  VkSemaphore GetImageAvailable() const { return m_imageAvailableSemaphore; }
  VkSemaphore GetRenderFinished(uint32_t index) const { return m_renderFinsishedSemaphores[index]; }
  VkFence GetFence() const { return m_inFlightFence; }
private:
  void Create(uint32_t imageCount);
private:
  VkDevice m_device;
  VkSemaphore m_imageAvailableSemaphore;
  std::vector<VkSemaphore> m_renderFinsishedSemaphores;
  VkFence m_inFlightFence;

};

}
