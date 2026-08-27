#include "syncObjects.hpp"

namespace Axiom{

SyncObjects::SyncObjects(VkDevice device, uint32_t imageCount) : m_device(device){
  Create(imageCount);
}

SyncObjects::~SyncObjects(){
  vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
  for(auto sem : m_renderFinsishedSemaphores)
    vkDestroySemaphore(m_device, sem, nullptr);
  vkDestroyFence(m_device, m_inFlightFence, nullptr);
}

void SyncObjects::Create(uint32_t imageCount){
  VkSemaphoreCreateInfo semInfo{};
  semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;


  VkResult imageResult = vkCreateSemaphore(m_device, &semInfo, nullptr, &m_imageAvailableSemaphore);

  m_renderFinsishedSemaphores.resize(imageCount);
  for(auto& sem : m_renderFinsishedSemaphores){
    VkResult renderResult = vkCreateSemaphore(m_device, &semInfo, nullptr, &sem);
    if(renderResult != VK_SUCCESS)
      throw std::runtime_error("Failed to create Render Finished Semaphore");
  }

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  VkResult fenceResult = vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence);
  if(imageResult != VK_SUCCESS || fenceResult != VK_SUCCESS)
    throw std::runtime_error("Failed to create semaphores/fence.");
}

}
