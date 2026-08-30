#pragma once
#include "buffer.hpp"
#include "descriptorSet.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "rhiContext.hpp"
#include "texture.hpp"
#include "vertexBuilder.hpp"
#include "vertexLayout.hpp"
#include <cstdint>
#include <map>
#include <unordered_map>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace Axiom{

struct Renderer2DUniforms{
  alignas(16) glm::mat4 projection;
};

struct Rectangle{
  float x, y, width, height;
};

struct Batch{
  Pipeline *pipeline;
  VertexLayout vertexLayout;
  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  VertexBuilder vertexData;
  std::vector<uint16_t> indices;
  Mesh mesh;
};

struct PipelineHandle{
  uint32_t index{0};
};

struct TextureHandle{
  uint32_t index{0};
};

class Renderer2D{
public:
  Renderer2D(RhiContext& rhi);
  
  void Begin();

  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
  void DrawRect(Rectangle rec, glm::vec3 color);
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, PipelineHandle shaderHandle);
  void DrawRect(Rectangle rec, glm::vec3 color, PipelineHandle shaderHandle);
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, TextureHandle textureHandle);
  void DrawRect(Rectangle rec, glm::vec3 color, TextureHandle textureHandle);

  void DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec3 tint);
  void DrawSprite(Rectangle rec, glm::vec3 tint);


  void DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float roundness, glm::vec3 color);
  void DrawRoundedRect(Rectangle rec, float roundness, glm::vec3 color);

  void DrawLine(glm::vec2 from, glm::vec2 to, glm::vec3 color);
  void DrawLine(glm::vec2 from, glm::vec2 to, float thick, glm::vec3 color);

  void End();

  PipelineHandle LoadFragShader(const std::string& shaderName);
  TextureHandle LoadTexture(const std::string& textureName);
private:
  void FlushBatch(Batch& batch);
  Batch& GetBatch(Pipeline *pipeline, VkDescriptorSet set, const VertexLayout& layout);
  VkDescriptorSet GetOrCreateTextureSet(Texture& texture, uint32_t imgIdx);
  Pipeline& GetPipelineFromHandle(PipelineHandle handle);
  Texture& GetTextureFromHandle(TextureHandle handle);
  uint16_t GetIndexBase(Batch& batch);
private:
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
};

}
