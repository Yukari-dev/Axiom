#pragma once
#include "buffer.hpp"
#include "descriptorSet.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "rhiContext.hpp"
#include "vertex.hpp"
#include <cstdint>
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

struct RoundedRectPushConstants{
  glm::mat4 projection;
  glm::vec2 rectSize;
  float radius;
  float padding;
};

class Renderer2D{
public:
  Renderer2D(RhiContext& rhi);
  
  void Begin();
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
  void DrawRect(Rectangle rec, glm::vec3 color);
  void DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float radius, glm::vec3 color);
  void DrawRoundedRect(Rectangle rec, float radius, glm::vec3 color);
  void DrawLine(glm::vec2 fromPos, glm::vec2 toPos, glm::vec3 color);
  void DrawLine(glm::vec2 fromPos, glm::vec2 toPos, float thickness, glm::vec3 color);
  void End();
private:
  void Flush();
  void SetPipeline(Pipeline *pipeline){
    if(m_activePipeline != pipeline){
      Flush();
      m_activePipeline = pipeline;
    }
  }
private:
  RhiContext& m_rhi;
  Mesh m_mesh;
  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{nullptr};
  std::unique_ptr<Pipeline> m_pipeline{nullptr};
  std::unique_ptr<Pipeline> m_roundedRectPipeline{nullptr};
  Pipeline *m_activePipeline{nullptr};
  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DescriptorSet>> m_descriptorSets;

  std::vector<Vertex> m_vertices;
  std::vector<uint16_t> m_indices;
};

}
