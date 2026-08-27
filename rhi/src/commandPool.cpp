#include "commandPool.hpp"
#include <stdexcept>

namespace Axiom {

CommandPool::CommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex) : m_device(device){
  Create(graphicsQueueFamilyIndex);
}

CommandPool::~CommandPool(){
  vkDestroyCommandPool(m_device, m_commandPool, nullptr);
}

void CommandPool::Create(uint32_t graphicsQueueFamilyIndex){
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.queueFamilyIndex = graphicsQueueFamilyIndex;
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkResult result = vkCreateCommandPool(m_device, &info, nullptr, &m_commandPool);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Command Pool");
}

}
