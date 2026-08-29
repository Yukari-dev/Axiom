#pragma once
#include <array>
#include <cstddef>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace Axiom{

struct Vertex{
  glm::vec2 pos;
  glm::vec3 color;
  glm::vec2 uv;

  static VkVertexInputBindingDescription GetBindingDescription(){
    VkVertexInputBindingDescription desc{};
    desc.binding = 0;
    desc.stride = sizeof(Vertex);
    desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return desc;
  }

  static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions(){
    std::array<VkVertexInputAttributeDescription, 3> attDesc{};
    attDesc[0].binding = 0;
    attDesc[0].location = 0;
    attDesc[0].format = VK_FORMAT_R32G32_SFLOAT;
    attDesc[0].offset = offsetof(Vertex, pos);

    attDesc[1].binding = 0;
    attDesc[1].location = 1;
    attDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attDesc[1].offset = offsetof(Vertex, color);

    attDesc[2].binding = 0;
    attDesc[2].location = 2;
    attDesc[2].format = VK_FORMAT_R32G32_SFLOAT;
    attDesc[2].offset = offsetof(Vertex, uv);
    return attDesc;
  }
};

}
