#include "renderer2d.hpp"
#include "mesh.hpp"
#include <cstring>

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

  m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
  m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_roundedRectLayout.AddAttribute(VK_FORMAT_R32_SFLOAT);
  m_roundedRectPipeline = std::make_unique<Pipeline>(
    m_rhi.GetDevice(), m_rhi.GetExtent(), m_rhi.GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout(),
    "roundedRect2d.vert", "roundedRect2d.frag",
    m_roundedRectLayout
  );
  m_textureLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_textureLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
  m_textureLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
  m_defaultTexture = std::make_unique<Texture>(
    m_rhi.GetDeviceObject(), m_rhi.GetCommandPool(), "default.jpg"
  );
  m_texturePipeline = std::make_unique<Pipeline>(
    m_rhi.GetDevice(), m_rhi.GetExtent(), m_rhi.GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout(),
    "texture2d.vert", "texture2d.frag",
    m_textureLayout
  );
  
  size_t imageCount = m_rhi.GetSwapChainImageCount();
  m_uniformBuffers.resize(imageCount);
  m_descriptorSets.resize(imageCount);
  m_textureDescriptorSets.resize(imageCount);
  for (size_t i = 0; i < imageCount; i++) {
    m_uniformBuffers[i] = std::make_unique<Buffer>(
      m_rhi.GetPhysicalDevice(), m_rhi.GetDevice(), sizeof(Renderer2DUniforms),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_descriptorSets[i] = std::make_unique<DescriptorSet>(
      m_rhi.GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
    );
    m_textureDescriptorSets[i] = std::make_unique<DescriptorSet>(
      m_rhi.GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
    );
    m_textureDescriptorSets[i]->UpdateDescriptorSet(
      *m_uniformBuffers[i], sizeof(Renderer2DUniforms), *m_defaultTexture
    );
    m_descriptorSets[i]->UpdateDescriptorSet(
      *m_uniformBuffers[i], sizeof(Renderer2DUniforms)
    );
  }
}

VkDescriptorSet Renderer2D::GetOrCreateTextureSet(Texture& texture, uint32_t imgIdx){
  auto key = std::make_pair(&texture, imgIdx);
  auto it = m_textureSets.find(key);

  if(it != m_textureSets.end()){
    return it->second->GetSet();
  }

  auto descriptorSet = std::make_unique<DescriptorSet>(
    m_rhi.GetDevice(),
    m_descriptorSetLayout->GetDescriptorSetLayout()
  );

  descriptorSet->UpdateDescriptorSet(
    *m_uniformBuffers[imgIdx],
    sizeof(Renderer2DUniforms),
    texture
  );

  VkDescriptorSet rawSet = descriptorSet->GetSet();
  m_textureSets[key] = std::move(descriptorSet);
  return rawSet;
}

Batch& Renderer2D::GetBatch(Pipeline *pipeline, VkDescriptorSet set, const VertexLayout& layout){
  BatchKey key = { pipeline, set };
  auto it = m_batches.find(key);
  Batch* batch = nullptr;
  if (it == m_batches.end()){
    auto [inserted, _] = m_batches.emplace(
      key, Batch{pipeline, layout, set, {}, {}, Mesh(m_rhi.GetPhysicalDevice(), m_rhi.GetDevice())}
    );
    batch = &inserted->second;
  } else
    batch = &it->second;

  if(batch->indices.empty())
    m_renderQueue.push_back(batch);
  return *batch;
}

void Renderer2D::Begin(){
  m_renderQueue.clear();
  for(auto& [pipeline, batch] : m_batches){
    batch.vertexData.Clear();
    batch.indices.clear();
  }
}

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 col){
  DrawRect({pos.x, pos.y, size.x, size.y}, col);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 col){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = GetBatch(m_pipeline.get(), defaultSet, m_rectLayout);

  uint16_t base = GetIndexBase(batch);

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

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, PipelineHandle shaderHandle){
  DrawRect({pos.x, pos.y, size.x, size.y}, color, shaderHandle);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 col, PipelineHandle shaderHandle){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_descriptorSets[imgIdx]->GetSet();
  Pipeline& pipeline = GetPipelineFromHandle(shaderHandle);
  Batch& batch = GetBatch(&pipeline, defaultSet, m_customLayout);

  uint16_t base = GetIndexBase(batch);
  auto pushVertex = [&](glm::vec2 pos, glm::vec2 uv) {
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(col);
    batch.vertexData.PushFloat(10.0f);
    batch.vertexData.PushVec2(uv);
  };

  pushVertex({rec.x, rec.y}, {0.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y}, {1.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {1.0f, 1.0f});
  pushVertex({rec.x, rec.y + rec.height}, {0.0f, 1.0f});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, TextureHandle textureHandle){
  DrawRect({pos.x, pos.y, size.x, size.y}, color, textureHandle);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 color, TextureHandle textureHandle){
  Texture& texture = GetTextureFromHandle(textureHandle);
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet texSet = GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = GetBatch(m_texturePipeline.get(), texSet, m_textureLayout);

  uint16_t base = GetIndexBase(batch);
  auto pushVertex = [&](glm::vec2 pos, glm::vec2 uv) {
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(color);
    batch.vertexData.PushVec2(uv);
  };

  pushVertex({rec.x, rec.y}, {0.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y}, {1.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {1.0f, 1.0f});
  pushVertex({rec.x, rec.y + rec.height}, {0.0f, 1.0f});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec3 tint){
  DrawSprite({pos.x, pos.y, size.x, size.y}, tint);
}

void Renderer2D::DrawSprite(Rectangle rec, glm::vec3 tint){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet textureSet = m_textureDescriptorSets[imgIdx]->GetSet();
  Batch& batch = GetBatch(m_texturePipeline.get(), textureSet, m_textureLayout);

  uint16_t base = GetIndexBase(batch);

  auto pushVertex = [&](glm::vec2 pos, glm::vec2 uv) {
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(tint);
    batch.vertexData.PushVec2(uv);
  };

  pushVertex({rec.x, rec.y}, {0.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y}, {1.0f, 0.0f});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {1.0f, 1.0f});
  pushVertex({rec.x, rec.y + rec.height}, {0.0f, 1.0f});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec2 uvMin, glm::vec2 uvMax, glm::vec3 color, TextureHandle textureHandle) {
  Texture& texture = GetTextureFromHandle(textureHandle);
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet texSet = GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = GetBatch(m_texturePipeline.get(), texSet, m_textureLayout);

  uint16_t base = GetIndexBase(batch);
  auto pushVertex = [&](glm::vec2 pos, glm::vec2 uv) {
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(color);
    batch.vertexData.PushVec2(uv);
  };

  pushVertex({rec.x, rec.y}, {uvMin.x, uvMin.y});
  pushVertex({rec.x + rec.width, rec.y}, {uvMax.x, uvMin.y});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {uvMax.x, uvMax.y});
  pushVertex({rec.x, rec.y + rec.height}, {uvMin.x, uvMax.y});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec2 uvMin, glm::vec2 uvMax, glm::vec3 color, TextureHandle textureHandle, PipelineHandle pipelineHandle) {
  Pipeline& pipeline = GetPipelineFromHandle(pipelineHandle);
  Texture& texture = GetTextureFromHandle(textureHandle);

  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet texSet = GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = GetBatch(&pipeline, texSet, m_textureLayout);

  uint16_t base = GetIndexBase(batch);
  auto pushVertex = [&](glm::vec2 pos, glm::vec2 uv) {
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(color);
    batch.vertexData.PushVec2(uv);
  };

  pushVertex({rec.x, rec.y}, {uvMin.x, uvMin.y});
  pushVertex({rec.x + rec.width, rec.y}, {uvMax.x, uvMin.y});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {uvMax.x, uvMax.y});
  pushVertex({rec.x, rec.y + rec.height}, {uvMin.x, uvMax.y});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float roundness, glm::vec3 color){
  DrawRoundedRect({pos.x, pos.y, size.x, size.y}, roundness, color);
}

void Renderer2D::DrawRoundedRect(Rectangle rec, float roundness, glm::vec3 color){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = GetBatch(m_roundedRectPipeline.get(), defaultSet, m_roundedRectLayout);

  uint16_t base = GetIndexBase(batch);

  glm::vec2 size = {rec.width, rec.height};
  glm::vec2 half = size * 0.5f;

  auto pushVertex = [&](glm::vec2 pos, glm::vec2 localUV){
    batch.vertexData.PushVec2(pos);
    batch.vertexData.PushVec3(color);
    batch.vertexData.PushVec2(localUV);
    batch.vertexData.PushVec2(size);
    batch.vertexData.PushFloat(roundness);
  };

  pushVertex({rec.x, rec.y}, {-half.x, -half.y});
  pushVertex({rec.x + rec.width, rec.y}, {half.x, -half.y});
  pushVertex({rec.x + rec.width, rec.y + rec.height}, {half.x, half.y});
  pushVertex({rec.x, rec.y + rec.height}, {-half.x, half.y});

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawLine(glm::vec2 from, glm::vec2 to, float thick, glm::vec3 color){
  uint32_t imgIdx = m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = GetBatch(m_pipeline.get(), defaultSet, m_rectLayout);

  uint16_t base = GetIndexBase(batch);

  glm::vec2 dir = glm::normalize(to - from);
  glm::vec2 normal = {-dir.y, dir.x};
  glm::vec2 offset = normal * (thick * 0.5f);

  batch.vertexData.PushVec2(from - offset);
  batch.vertexData.PushVec3(color);

  batch.vertexData.PushVec2(to - offset);
  batch.vertexData.PushVec3(color);

  batch.vertexData.PushVec2(to + offset);
  batch.vertexData.PushVec3(color);

  batch.vertexData.PushVec2(from + offset);
  batch.vertexData.PushVec3(color);

  batch.indices.push_back(base + 0);
  batch.indices.push_back(base + 1);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 2);
  batch.indices.push_back(base + 3);
  batch.indices.push_back(base + 0);
}

void Renderer2D::DrawLine(glm::vec2 from, glm::vec2 to, glm::vec3 color){
  DrawLine(from, to, 1, color);
}

uint16_t Renderer2D::GetIndexBase(Batch& batch){
  return static_cast<uint16_t>(batch.vertexData.Size() / batch.vertexLayout.GetStride());
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

  for(Batch *batch : m_renderQueue){
    FlushBatch(*batch);
  }
}

void Renderer2D::FlushBatch(Batch& batch) {
  if (batch.indices.empty()) return;

  batch.mesh.SetData(batch.vertexData.Data(), batch.vertexData.Size(), batch.indices);

  CommandBuffer& cmd = m_rhi.GetCommandBufferObject();
  cmd.BindPipeline(batch.pipeline->GetPipeline());

  vkCmdBindDescriptorSets(
    cmd.GetHandler(), VK_PIPELINE_BIND_POINT_GRAPHICS,
    batch.pipeline->GetPipelineLayout(), 0, 1, &batch.descriptorSet, 0, nullptr
  );

  batch.mesh.Bind(cmd);
  cmd.DrawIndexed(batch.mesh.GetIndexCount());
}

PipelineHandle Renderer2D::LoadFragShader(const std::string& shaderName){
  auto it = m_pipelineLookup.find(shaderName);

  if(it != m_pipelineLookup.end()){
    return PipelineHandle{ it->second };
  }

  auto pipeline = std::make_unique<Pipeline>(
    m_rhi.GetDevice(), m_rhi.GetExtent(), m_rhi.GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout(),
    "customShader.vert", shaderName,
    m_customLayout
  );

  uint32_t newId = static_cast<uint32_t>(m_pipelines.size());
  m_pipelines.push_back(std::move(pipeline));
  m_pipelineLookup[shaderName] = newId;

  return PipelineHandle{ newId };
}

Pipeline& Renderer2D::GetPipelineFromHandle(PipelineHandle handle){
  if(handle.index < m_pipelines.size() && m_pipelines[handle.index])
    return *m_pipelines[handle.index];

  return *m_pipeline;
}

TextureHandle Renderer2D::LoadTexture(const std::string& textureName){
  auto it = m_textureLookup.find(textureName);

  if(it != m_textureLookup.end()){
    return TextureHandle{ it->second };
  }

  auto texture = std::make_unique<Texture>(
    m_rhi.GetDeviceObject(), m_rhi.GetCommandPool(), textureName
  );

  uint32_t newId = static_cast<uint32_t>(m_textures.size());
  m_textures.push_back(std::move(texture));
  m_textureLookup[textureName] = newId;

  return TextureHandle{ newId };
}

TextureHandle Renderer2D::CreateTextureFromData(const uint8_t *pixels, uint32_t width, uint32_t height){
  auto texture = std::make_unique<Texture>(
    m_rhi.GetDeviceObject(), 
    m_rhi.GetCommandPool(), 
    pixels, 
    width, 
    height, 
    VK_FORMAT_R8_UNORM
  );

  uint32_t newId = static_cast<uint32_t>(m_textures.size());
  m_textures.push_back(std::move(texture));

  return TextureHandle{ newId };
}

Texture& Renderer2D::GetTextureFromHandle(TextureHandle handle){
  if (handle.index < m_textures.size() && m_textures[handle.index])
    return *m_textures[handle.index];
  return *m_defaultTexture;
}

}
