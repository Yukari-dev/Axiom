#include "renderer2d.hpp"

namespace Axiom{

Renderer2D::Renderer2D(RhiContext& ctx) : m_rhi(ctx), m_mesh(ctx.GetPhysicalDevice(), ctx.GetDevice()){
  m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_rhi.GetDevice());
  m_pipeline = std::make_unique<Pipeline>(
    m_rhi.GetDevice(), m_rhi.GetExtent(), m_rhi.GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout(),
    "renderer2d.vert", "renderer2d.frag"
  );
  
  size_t imageCount = m_rhi.GetSwapChainImageCount();
  m_uniformBuffers.resize(imageCount);
  m_descriptorSets.resize(imageCount);
  for (size_t i = 0; i < imageCount; i++) {
    m_uniformBuffers[i] = std::make_unique<Buffer>(
      m_rhi.GetPhysicalDevice(), m_rhi.GetDevice(), sizeof(Renderer2DUniforms),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_descriptorSets[i] = std::make_unique<DescriptorSet>(
      m_rhi.GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
    );
    m_descriptorSets[i]->UpdateDescriptorSet(
      *m_uniformBuffers[i], sizeof(Renderer2DUniforms)
    );
  }
}

void Renderer2D::Begin(){
  m_vertices.clear();
  m_indices.clear();
}

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 col){
  uint16_t base = static_cast<uint16_t>(m_vertices.size());

  m_vertices.push_back({pos, col, {0.0f, 0.0f}});
  m_vertices.push_back({{pos.x + size.x, pos.y}, col, {1.0f, 0.0f}});
  m_vertices.push_back({{pos.x + size.x, pos.y + size.y}, col, {1.0f, 1.0f}});
  m_vertices.push_back({{pos.x, pos.y + size.y}, col, {0.0f, 1.0f}});

  m_indices.push_back(base + 0);
  m_indices.push_back(base + 1);
  m_indices.push_back(base + 2);
  m_indices.push_back(base + 2);
  m_indices.push_back(base + 3);
  m_indices.push_back(base + 0);
}

void Renderer2D::DrawLine(glm::vec2 fromPos, glm::vec2 toPos, glm::vec3 color){
  uint32_t base = static_cast<uint16_t>(m_vertices.size());
  m_vertices.push_back({fromPos, color, {0.0f, 0.0f}});
  m_vertices.push_back({toPos, color, {1.0f, 0.0f}});
  m_vertices.push_back({{toPos.x, toPos.y + 1}, color, {1.0f, 1.0f}});
  m_vertices.push_back({{fromPos.x, fromPos.y + 1}, color, {0.0f, 1.0f}});

  m_indices.push_back(base + 0);
  m_indices.push_back(base + 1);
  m_indices.push_back(base + 2);
  m_indices.push_back(base + 2);
  m_indices.push_back(base + 3);
  m_indices.push_back(base + 0);
}

void Renderer2D::End(){
  m_mesh.SetData(m_vertices, m_indices);
  if(m_mesh.GetIndexCount() == 0) return;

  uint32_t imgIdx = m_rhi.GetImageIndex();
  Renderer2DUniforms ubo{};
  VkExtent2D extent = m_rhi.GetExtent();
  ubo.projection = glm::ortho(
    0.0f, static_cast<float>(extent.width),
    0.0f,
    static_cast<float>(extent.height), 
    -1.0f, 1.0f
  );
  m_uniformBuffers[imgIdx]->CopyData(&ubo, sizeof(ubo));
  
  CommandBuffer& cmd = m_rhi.GetCommandBufferObject();
  cmd.BindPipeline(m_pipeline->GetPipeline());
  VkDescriptorSet set = m_descriptorSets[imgIdx]->GetSet();
  vkCmdBindDescriptorSets(
    cmd.GetHandler(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(),
    0, 1, &set, 0, nullptr
  );

  m_mesh.Bind(cmd);
  cmd.DrawIndexed(m_mesh.GetIndexCount());
}

}
