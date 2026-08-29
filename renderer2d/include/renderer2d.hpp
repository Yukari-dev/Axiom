#pragma once
#include "buffer.hpp"
#include "descriptorSet.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "rhiContext.hpp"
#include "vertexBuilder.hpp"
#include "vertexLayout.hpp"
#include <cstdint>
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
  VertexBuilder vertexData;
  std::vector<uint16_t> indices;
  Mesh mesh;
};

class Renderer2D{
public:
  Renderer2D(RhiContext& rhi);
  
  void Begin();

  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
  void DrawRect(Rectangle rec, glm::vec3 color);

  void DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float roundness, glm::vec3 color);
  void DrawRoundedRect(Rectangle rec, float roundness, glm::vec3 color);

  void DrawLine(glm::vec2 from, glm::vec2 to, glm::vec3 color);
  void DrawLine(glm::vec2 from, glm::vec2 to, float thick, glm::vec3 color);

  void End();
private:
  void FlushBatch(Batch& batch);
  Batch& GetBatch(Pipeline *pipeline, const VertexLayout& layout);
private:
  RhiContext& m_rhi;
  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{nullptr};
  std::unique_ptr<Pipeline> m_pipeline{nullptr};
  std::unique_ptr<Pipeline> m_roundedRectPipeline{nullptr};
  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DescriptorSet>> m_descriptorSets;

  std::unordered_map<Pipeline*, Batch> m_batches;
  VertexLayout m_rectLayout{};
  VertexLayout m_roundedRectLayout{};
};

}
