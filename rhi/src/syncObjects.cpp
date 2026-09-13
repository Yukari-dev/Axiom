#include "syncObjects.hpp"
#include <stdexcept>

namespace Axiom{

SyncObjects::SyncObjects(VkDevice device, uint32_t imageCount, uint32_t framesInFlight) : m_device(device){
  Create(imageCount, framesInFlight);
}

SyncObjects::~SyncObjects(){
  for(auto sem : m_imageAvailableSemaphores)
    vkDestroySemaphore(m_device, sem, nullptr);
  for(auto sem : m_renderFinsishedSemaphores)
    vkDestroySemaphore(m_device, sem, nullptr);
  for(auto fence : m_inFlightFences)
    vkDestroyFence(m_device, fence, nullptr);
}

void SyncObjects::Create(uint32_t imageCount, uint32_t framesInFlight){
  VkSemaphoreCreateInfo semInfo{};
  semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  m_imageAvailableSemaphores.resize(framesInFlight);
  m_inFlightFences.resize(framesInFlight);

  for(uint32_t i = 0; i < framesInFlight; i++){
    if(vkCreateSemaphore(m_device, &semInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS
      || vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
      throw std::runtime_error("Failed to create per frame semaphore/fence.");

  }

  m_renderFinsishedSemaphores.resize(imageCount);
  for(auto& sem : m_renderFinsishedSemaphores){
    if(vkCreateSemaphore(m_device, &semInfo, nullptr, &sem) != VK_SUCCESS)
      throw std::runtime_error("Failed to create Render Finished Semaphore");
  }
}

}
