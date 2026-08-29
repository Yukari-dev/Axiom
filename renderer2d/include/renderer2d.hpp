#pragma once
#include "buffer.hpp"
#include "descriptorSet.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "rhiContext.hpp"
#include "vertex.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Axiom{

struct Renderer2DUniforms{
  alignas(16) glm::mat4 projection;
};

class Renderer2D{
public:
  Renderer2D(RhiContext& rhi);
  
  void Begin();
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
  void End();
private:
  static constexpr uint32_t MAX_QUADS = 1000;

  RhiContext& m_rhi;
  Mesh m_mesh;
  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{nullptr};
  std::unique_ptr<Pipeline> m_pipeline{nullptr};
  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DescriptorSet>> m_descriptorSets;

  std::vector<Vertex> m_vertices;
  std::vector<uint16_t> m_indices;
};

}
