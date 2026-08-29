#pragma once
#include <string>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Pipeline{
public:
  Pipeline(
    VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout,
    const std::string& vert, const std::string& frag,
    uint32_t pushConstantSize = 0,
    VkShaderStageFlags pushConstantStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
  );
  ~Pipeline();
  
  void SetShader(const std::string& vertShader, const std::string& fragShader);
  VkPipeline GetPipeline() const { return m_graphicsPipeline; }
  VkPipelineLayout GetPipelineLayout() const { return m_pipelineLayout; }
private:
  void Create(
    VkExtent2D extent, VkRenderPass renderPass, VkDescriptorSetLayout layout,
    const std::string& vert, const std::string& frag,
    uint32_t pushConstantSize = 0,
    VkShaderStageFlags pushConstantStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
  );
private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
};

}
