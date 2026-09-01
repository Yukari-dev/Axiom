#include "renderer2d.hpp"
#include "descriptorSet.hpp"
#include "descriptorSetLayout.hpp"
#include "rhiContext.hpp"
#include "commandPool.hpp"
#include "swapchain.hpp"
#include "renderPass.hpp"
#include "mesh.hpp"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <map>
#include <utility>
#include <vector>

#include "pipeline.hpp"
#include "renderer2d.vert.spv.h"
#include "renderer2d.frag.spv.h"
#include "roundedRect2d.vert.spv.h"
#include "roundedRect2d.frag.spv.h"
#include "texture2d.vert.spv.h"
#include "texture2d.frag.spv.h"
#include "vertexBuilder.hpp"

namespace Axiom {

struct Batch {
  Pipeline* pipeline{nullptr};
  VertexLayout vertexLayout{};
  VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
  VertexBuilder vertexData{};
  std::vector<uint16_t> indices;
  Mesh mesh;
};

struct Renderer2DUniforms {
  alignas(16) glm::mat4 projection;
};

struct Renderer2D::Impl {
  RhiContext& m_rhi;

  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{nullptr};
  std::unique_ptr<Pipeline> m_pipeline{nullptr};
  std::unique_ptr<Pipeline> m_roundedRectPipeline{nullptr};
  std::unique_ptr<Pipeline> m_texturePipeline{nullptr};
  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DescriptorSet>> m_descriptorSets;
  std::vector<std::unique_ptr<DescriptorSet>> m_textureDescriptorSets;
  std::map<std::pair<Texture*, uint32_t>, std::unique_ptr<DescriptorSet>> m_textureSets;
  std::unique_ptr<Texture> m_defaultTexture{nullptr};

  std::vector<std::unique_ptr<Pipeline>> m_pipelines;
  std::unordered_map<std::string, uint32_t> m_pipelineLookup;

  std::vector<std::unique_ptr<Texture>> m_textures;
  std::unordered_map<std::string, uint32_t> m_textureLookup;

  using BatchKey = std::pair<Pipeline*, VkDescriptorSet>;
  struct HashBatchKey {
    std::size_t operator()(const BatchKey& key) const {
      std::size_t h1 = std::hash<Pipeline*>{}(key.first);
      std::size_t h2 = std::hash<VkDescriptorSet>{}(key.second);
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
  };

  std::unordered_map<BatchKey, Batch, HashBatchKey> m_batches;
  std::vector<Batch*> m_renderQueue;

  VertexLayout m_rectLayout{};
  VertexLayout m_roundedRectLayout{};
  VertexLayout m_textureLayout{};
  VertexLayout m_customLayout{};

  explicit Impl(RhiContext& ctx) : m_rhi(ctx) {
    m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_rhi.GetDeviceObject().GetDevice());

    m_rectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_rectLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
    m_pipeline = std::make_unique<Pipeline>(
      m_rhi.GetDeviceObject().GetDevice(), m_rhi.GetSwapChainObject().GetExtent(), m_rhi.GetRenderPassObject().GetRenderPass(),
      m_descriptorSetLayout->GetDescriptorSetLayout(),
      g_renderer2d_vert, sizeof(g_renderer2d_vert),
      g_renderer2d_frag, sizeof(g_renderer2d_frag),
      m_rectLayout
    );

    m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
    m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_roundedRectLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_roundedRectLayout.AddAttribute(VK_FORMAT_R32_SFLOAT);
    m_roundedRectPipeline = std::make_unique<Pipeline>(
      m_rhi.GetDeviceObject().GetDevice(), m_rhi.GetSwapChainObject().GetExtent(), m_rhi.GetRenderPassObject().GetRenderPass(),
      m_descriptorSetLayout->GetDescriptorSetLayout(),
      g_roundedRect2d_vert, sizeof(g_roundedRect2d_vert),
      g_roundedRect2d_frag, sizeof(g_roundedRect2d_frag),
      m_roundedRectLayout
    );

    m_textureLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_textureLayout.AddAttribute(VK_FORMAT_R32G32B32_SFLOAT);
    m_textureLayout.AddAttribute(VK_FORMAT_R32G32_SFLOAT);
    m_defaultTexture = std::make_unique<Texture>(
      m_rhi.GetDeviceObject(), m_rhi.GetCommandPoolObject().GetHandler(), "default.jpg"
    );
    m_texturePipeline = std::make_unique<Pipeline>(
      m_rhi.GetDeviceObject().GetDevice(), m_rhi.GetSwapChainObject().GetExtent(), m_rhi.GetRenderPassObject().GetRenderPass(),
      m_descriptorSetLayout->GetDescriptorSetLayout(),
      g_texture2d_vert, sizeof(g_texture2d_vert),
      g_texture2d_frag, sizeof(g_texture2d_frag),
      m_textureLayout
    );

    size_t imageCount = m_rhi.GetSwapChainObject().GetImageCount();
    m_uniformBuffers.resize(imageCount);
    m_descriptorSets.resize(imageCount);
    m_textureDescriptorSets.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++) {
      m_uniformBuffers[i] = std::make_unique<Buffer>(
        m_rhi.GetDeviceObject().GetPhysicalDevice(), m_rhi.GetDeviceObject().GetDevice(), sizeof(Renderer2DUniforms),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
      );
      m_descriptorSets[i] = std::make_unique<DescriptorSet>(
        m_rhi.GetDeviceObject().GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
      );
      m_textureDescriptorSets[i] = std::make_unique<DescriptorSet>(
        m_rhi.GetDeviceObject().GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
      );
      m_textureDescriptorSets[i]->UpdateDescriptorSet(
        *m_uniformBuffers[i], sizeof(Renderer2DUniforms), *m_defaultTexture
      );
      m_descriptorSets[i]->UpdateDescriptorSet(
        *m_uniformBuffers[i], sizeof(Renderer2DUniforms)
      );
    }
  }

  VkDescriptorSet GetOrCreateTextureSet(Texture& texture, uint32_t imgIdx) {
    auto key = std::make_pair(&texture, imgIdx);
    auto it = m_textureSets.find(key);

    if (it != m_textureSets.end()) {
      return it->second->GetSet();
    }

    auto descriptorSet = std::make_unique<DescriptorSet>(
      m_rhi.GetDeviceObject().GetDevice(),
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

  Batch& GetBatch(Pipeline* pipeline, VkDescriptorSet set, const VertexLayout& layout) {
    BatchKey key = { pipeline, set };
    auto it = m_batches.find(key);
    Batch* batch = nullptr;
    if (it == m_batches.end()) {
      auto [inserted, _] = m_batches.emplace(
        key, Batch{pipeline, layout, set, {}, {}, Mesh(m_rhi.GetDeviceObject().GetPhysicalDevice(), m_rhi.GetDeviceObject().GetDevice())}
      );
      batch = &inserted->second;
    } else {
      batch = &it->second;
    }

    if (batch->indices.empty()) {
      m_renderQueue.push_back(batch);
    }
    return *batch;
  }

  uint16_t GetIndexBase(Batch& batch) {
    return static_cast<uint16_t>(batch.vertexData.Size() / batch.vertexLayout.GetStride());
  }

  void FlushBatch(Batch& batch) {
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

  Pipeline& GetPipelineFromHandle(PipelineHandle handle) {
    if (handle.index < m_pipelines.size() && m_pipelines[handle.index])
      return *m_pipelines[handle.index];

    return *m_pipeline;
  }

  Texture& GetTextureFromHandle(TextureHandle handle) {
    if (handle.index < m_textures.size() && m_textures[handle.index])
      return *m_textures[handle.index];
    return *m_defaultTexture;
  }
};

Renderer2D::Renderer2D(RhiContext& ctx)
  : m_impl(std::make_unique<Impl>(ctx)) {}

Renderer2D::~Renderer2D() = default;

void Renderer2D::Begin() {
  m_impl->m_renderQueue.clear();
  for (auto& [pipeline, batch] : m_impl->m_batches) {
    batch.vertexData.Clear();
    batch.indices.clear();
  }
}

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 col) {
  DrawRect({pos.x, pos.y, size.x, size.y}, col);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 col) {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_impl->m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = m_impl->GetBatch(m_impl->m_pipeline.get(), defaultSet, m_impl->m_rectLayout);

  uint16_t base = m_impl->GetIndexBase(batch);

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

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, PipelineHandle shaderHandle) {
  DrawRect({pos.x, pos.y, size.x, size.y}, color, shaderHandle);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 col, PipelineHandle shaderHandle) {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_impl->m_descriptorSets[imgIdx]->GetSet();
  Pipeline& pipeline = m_impl->GetPipelineFromHandle(shaderHandle);
  Batch& batch = m_impl->GetBatch(&pipeline, defaultSet, m_impl->m_customLayout);

  uint16_t base = m_impl->GetIndexBase(batch);
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

void Renderer2D::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, TextureHandle textureHandle) {
  DrawRect({pos.x, pos.y, size.x, size.y}, color, textureHandle);
}

void Renderer2D::DrawRect(Rectangle rec, glm::vec3 color, TextureHandle textureHandle) {
  Texture& texture = m_impl->GetTextureFromHandle(textureHandle);
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet texSet = m_impl->GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = m_impl->GetBatch(m_impl->m_texturePipeline.get(), texSet, m_impl->m_textureLayout);

  uint16_t base = m_impl->GetIndexBase(batch);
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

void Renderer2D::DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec3 tint) {
  DrawSprite({pos.x, pos.y, size.x, size.y}, tint);
}

void Renderer2D::DrawSprite(Rectangle rec, glm::vec3 tint) {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet textureSet = m_impl->m_textureDescriptorSets[imgIdx]->GetSet();
  Batch& batch = m_impl->GetBatch(m_impl->m_texturePipeline.get(), textureSet, m_impl->m_textureLayout);

  uint16_t base = m_impl->GetIndexBase(batch);

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
  Texture& texture = m_impl->GetTextureFromHandle(textureHandle);
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet texSet = m_impl->GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = m_impl->GetBatch(m_impl->m_texturePipeline.get(), texSet, m_impl->m_textureLayout);

  uint16_t base = m_impl->GetIndexBase(batch);
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
  Pipeline& pipeline = m_impl->GetPipelineFromHandle(pipelineHandle);
  Texture& texture = m_impl->GetTextureFromHandle(textureHandle);

  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet texSet = m_impl->GetOrCreateTextureSet(texture, imgIdx);

  Batch& batch = m_impl->GetBatch(&pipeline, texSet, m_impl->m_textureLayout);

  uint16_t base = m_impl->GetIndexBase(batch);
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

void Renderer2D::DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float roundness, glm::vec3 color) {
  DrawRoundedRect({pos.x, pos.y, size.x, size.y}, roundness, color);
}

void Renderer2D::DrawRoundedRect(Rectangle rec, float roundness, glm::vec3 color) {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_impl->m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = m_impl->GetBatch(m_impl->m_roundedRectPipeline.get(), defaultSet, m_impl->m_roundedRectLayout);

  uint16_t base = m_impl->GetIndexBase(batch);

  glm::vec2 size = {rec.width, rec.height};
  glm::vec2 half = size * 0.5f;

  auto pushVertex = [&](glm::vec2 pos, glm::vec2 localUV) {
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

void Renderer2D::DrawLine(glm::vec2 from, glm::vec2 to, float thick, glm::vec3 color) {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkDescriptorSet defaultSet = m_impl->m_descriptorSets[imgIdx]->GetSet();
  Batch& batch = m_impl->GetBatch(m_impl->m_pipeline.get(), defaultSet, m_impl->m_rectLayout);

  uint16_t base = m_impl->GetIndexBase(batch);

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

void Renderer2D::DrawLine(glm::vec2 from, glm::vec2 to, glm::vec3 color) {
  DrawLine(from, to, 1.0f, color);
}

void Renderer2D::End() {
  uint32_t imgIdx = m_impl->m_rhi.GetImageIndex();
  VkExtent2D extent = m_impl->m_rhi.GetSwapChainObject().GetExtent();
  Renderer2DUniforms ubo{};
  ubo.projection = glm::ortho(
    0.0f, static_cast<float>(extent.width), 0.0f,
    static_cast<float>(extent.height), -1.0f, 1.0f
  );
  m_impl->m_uniformBuffers[imgIdx]->CopyData(&ubo, sizeof(ubo));

  for (Batch* batch : m_impl->m_renderQueue) {
    m_impl->FlushBatch(*batch);
  }
}

PipelineHandle Renderer2D::LoadFragShader(const std::string& shaderName) {
  auto it = m_impl->m_pipelineLookup.find(shaderName);

  if (it != m_impl->m_pipelineLookup.end()) {
    return PipelineHandle{ it->second };
  }

  auto pipeline = std::make_unique<Pipeline>(
    m_impl->m_rhi.GetDeviceObject().GetDevice(), m_impl->m_rhi.GetSwapChainObject().GetExtent(), m_impl->m_rhi.GetRenderPassObject().GetRenderPass(),
    m_impl->m_descriptorSetLayout->GetDescriptorSetLayout(),
    g_renderer2d_vert, sizeof(g_renderer2d_vert),
    g_renderer2d_frag, sizeof(g_renderer2d_frag),
    m_impl->m_customLayout
  );

  uint32_t newId = static_cast<uint32_t>(m_impl->m_pipelines.size());
  m_impl->m_pipelines.push_back(std::move(pipeline));
  m_impl->m_pipelineLookup[shaderName] = newId;

  return PipelineHandle{ newId };
}

TextureHandle Renderer2D::LoadTexture(const std::string& textureName) {
  auto it = m_impl->m_textureLookup.find(textureName);

  if (it != m_impl->m_textureLookup.end()) {
    return TextureHandle{ it->second };
  }

  auto texture = std::make_unique<Texture>(
    m_impl->m_rhi.GetDeviceObject(), m_impl->m_rhi.GetCommandPoolObject().GetHandler(), textureName
  );

  uint32_t newId = static_cast<uint32_t>(m_impl->m_textures.size());
  m_impl->m_textures.push_back(std::move(texture));
  m_impl->m_textureLookup[textureName] = newId;

  return TextureHandle{ newId };
}

TextureHandle Renderer2D::CreateTextureFromData(const uint8_t* pixels, uint32_t width, uint32_t height) {
  auto texture = std::make_unique<Texture>(
    m_impl->m_rhi.GetDeviceObject(), 
    m_impl->m_rhi.GetCommandPoolObject().GetHandler(), 
    pixels, 
    width, 
    height, 
    VK_FORMAT_R8_UNORM
  );

  uint32_t newId = static_cast<uint32_t>(m_impl->m_textures.size());
  m_impl->m_textures.push_back(std::move(texture));

  return TextureHandle{ newId };
}

}
