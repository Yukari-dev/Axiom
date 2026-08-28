#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Pipeline{
public:
  Pipeline(VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout);
  ~Pipeline();
  
  VkPipeline GetPipeline() const { return m_graphicsPipeline; }
  VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }
private:
  void Create(VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout);
private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
};

}
