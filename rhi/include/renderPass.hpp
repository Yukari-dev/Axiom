#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class RenderPass{
public:
  RenderPass(VkDevice device, VkFormat swapChainFormat);
  ~RenderPass();
  
  VkRenderPass GetRenderPass() const { return m_renderPass; }
private:
  void Create(VkFormat swapChainFormat);
private:
  VkRenderPass m_renderPass;
  VkDevice m_device;
};

}
