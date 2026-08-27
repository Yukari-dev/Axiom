#pragma once
#include <array>
#include <cstddef>
#include <vulkan/vulkan.hpp>

namespace Axiom{

struct Vertex{
  float pos[2];

  static VkVertexInputBindingDescription GetBindingDescription(){
    VkVertexInputBindingDescription desc{};
    desc.binding = 0;
    desc.stride = sizeof(Vertex);
    desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return desc;
  }

  static std::array<VkVertexInputAttributeDescription, 1> GetAttributeDescriptions(){
    std::array<VkVertexInputAttributeDescription, 1> attDesc{};
    attDesc[0].binding = 0;
    attDesc[0].location = 0;
    attDesc[0].format = VK_FORMAT_R32G32_SFLOAT;
    attDesc[0].offset = offsetof(Vertex, pos);
    return attDesc;
  }
};

}
