#pragma once
#include "vertexLayout.hpp"
#include <string>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Pipeline{
public:
  Pipeline(
    VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout,
    const std::string& vert, const std::string& frag,
    const VertexLayout& vertexLayout
  );
  Pipeline(
    VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout,
    const uint32_t *vertCode, size_t vertCodeSize,
    const uint32_t *fragCode, size_t fragCodeSize,
    const VertexLayout& vertexLayout
  );
  ~Pipeline();
  
  void SetShader(const std::string& vertShader, const std::string& fragShader);
  VkPipeline GetPipeline() const { return m_graphicsPipeline; }
  VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }
private:
  void Create(
    VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout,
    const uint32_t *vertCode, size_t vertCodeSize,
    const uint32_t *fragCode, size_t fragCodeSize,
    const VertexLayout& vertexLayout
  );
private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
};

}
