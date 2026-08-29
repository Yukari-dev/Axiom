#include "renderer2d.hpp"
#include "mesh.hpp"

namespace Axiom{

Renderer2D::Renderer2D(RhiContext& ctx) : m_rhi(ctx){
  m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_rhi.GetDevice());
  m_rectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_rectLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
  m_pipeline = std::make_unique<Pipeline>(
    m_rhi.GetDevice(), m_rhi.GetExtent(), m_rhi.GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout(),
    "renderer2d.vert", "renderer2d.frag",
    m_rectLayout
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

Batch& Renderer2D::GetBatch(Pipeline *pipeline, const VertexLayout& layout){
  auto it = m_batches.find(pipeline);
  if (it == m_batches.end()){
    auto [inserted, _] = m_batches.emplace(
      pipeline, Batch{pipeline, layout, {}, {}, Mesh(m_rhi.GetPhysicalDevice(), m_rhi.GetDevice())}
    );
    return inserted->second;
  }
  return it->second;
}

void Renderer2D::Begin(){
  for(auto& [pipeline, batch] : m_batches){
    batch.vertexData.Clear();
    batch.indices.clear();
  }
}

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 col){
  DrawRect({pos.x, pos.y, size.x, size.y}, col);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 col){
  Batch& batch = GetBatch(m_pipeline.get(), m_rectLayout);

  uint16_t base = static_cast<uint16_t>(batch.vertexData.Size() / batch.vertexLayout.GetStride());

  batch.vertexData.PushVec2({rec.x, rec.y});
  batch.vertexData.PushVec3(col);

  batch.vertexData.PushVec2({rec.x + rec.width, rec.y});
  batch.vertexData.PushVec3(col);

  batch.vertexData.PushVec2({rec.x + rec.width, rec.y + rec.height});
  batch.vertexData.PushVec3(col);

  batch.vertexData.PushVec2({rec.x, rec.y + rec.height});
  batch.vertexData.PushVec3(col);

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::End(){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkExtent2D extent = m_rhi.GetExtent();
  Renderer2DUniforms ubo{};
  ubo.projection = glm::ortho(
    0.0f, static_cast<float>(extent.width), 0.0f,
    static_cast<float>(extent.height), -1.0f, 1.0f
  );
  m_uniformBuffers[imgIdx]->CopyData(&ubo, sizeof(ubo));
  
  for(auto& [pipeline, batch] : m_batches){
    FlushBatch(batch);
  }
}

void Renderer2D::FlushBatch(Batch& batch){
  if(batch.indices.empty()) return;

  batch.mesh.SetData(batch.vertexData.Data(), batch.vertexData.Size(), batch.indices);

  CommandBuffer& cmd = m_rhi.GetCommandBufferObject();
  cmd.BindPipeline(batch.pipeline->GetPipeline());

  if(batch.pipeline == m_pipeline.get()){
    uint32_t imgIndx = m_rhi.GetImageIndex();
    VkDescriptorSet set = m_descriptorSets[imgIndx]->GetSet();
    vkCmdBindDescriptorSets(
      cmd.GetHandler(), VK_PIPELINE_BIND_POINT_GRAPHICS,
      batch.pipeline->GetPipelineLayout(), 0, 1, &set, 0, nullptr
    );
  }
  batch.mesh.Bind(cmd);
  cmd.DrawIndexed(batch.mesh.GetIndexCount());
}

}
