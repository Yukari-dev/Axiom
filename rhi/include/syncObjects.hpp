#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class SyncObjects{
public:
  SyncObjects(VkDevice device, uint32_t swapChainImageCount, uint32_t framesInFlight);
  ~SyncObjects();
  
  VkSemaphore GetImageAvailable(uint32_t frameIndex) const { return m_imageAvailableSemaphores[frameIndex]; }
  VkSemaphore GetRenderFinished(uint32_t index) const { return m_renderFinsishedSemaphores[index]; }
  VkFence GetFence(uint32_t frameIndex) const { return m_inFlightFences[frameIndex]; }
private:
  void Create(uint32_t imageCount, uint32_t framesInFlight);
private:
  VkDevice m_device;
  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_renderFinsishedSemaphores;
  std::vector<VkFence> m_inFlightFences;

};

}
