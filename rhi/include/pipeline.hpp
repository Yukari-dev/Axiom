#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Pipeline{
public:
  Pipeline(VkDevice device, VkExtent2D extent, VkRenderPass renderPass);
  ~Pipeline();
  
  VkPipeline GetPipeline() const { return m_graphicsPipeline; }
private:
  void Create(VkExtent2D extent, VkRenderPass renderPass);
private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
};

}
