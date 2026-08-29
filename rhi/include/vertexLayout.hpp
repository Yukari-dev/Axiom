#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

struct VertexAttribute{
  uint32_t offset;
  VkFormat format;
};

class VertexLayout{
public:
  void AddAttribute(VkFormat format){
    VertexAttribute attr{};
    attr.offset = m_stride;
    attr.format = format;
    m_attributes.push_back(attr);
    
    m_stride += FormatSize(format);
  }

  uint32_t FormatSize(VkFormat format){
    switch(format){
      case VK_FORMAT_R32_SFLOAT: return 4;
      case VK_FORMAT_R32G32_SFLOAT: return 8;
      case VK_FORMAT_R32G32B32_SFLOAT: return 12;
      case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;
      default: throw std::runtime_error("Unhandled VkFormat in FormatSize");
    }
  }

  uint32_t GetStride() const { return m_stride; }
  const std::vector<VertexAttribute>& GetAttribute() const { return m_attributes; }

private:
  std::vector<VertexAttribute> m_attributes;
  uint32_t m_stride{0};
};

}
