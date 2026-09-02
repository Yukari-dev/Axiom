#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

struct VertexAttribute{
  uint32_t offset;
  VkFormat format;
};

enum class VertexFormat{
  Float, Float2, Float3, Float4
};

class VertexLayout{
public:
  VertexLayout AddAttribute(VkFormat format){
    VertexAttribute attr{};
    attr.offset = m_stride;
    attr.format = format;
    m_attributes.push_back(attr);
    
    m_stride += FormatSize(format);
    return *this;
  }

  VertexLayout AddAttribute(VertexFormat format){
    VertexAttribute attr{};
    attr.offset = m_stride;
    VkFormat vkFormat = VertexFormatToVkFormat(format);
    attr.format = vkFormat;
    m_attributes.push_back(attr);
    
    m_stride += FormatSize(vkFormat);
    return *this;
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

  VkFormat VertexFormatToVkFormat(VertexFormat format){
    switch(format){
      case VertexFormat::Float: return VK_FORMAT_R32_SFLOAT;
      case VertexFormat::Float2: return VK_FORMAT_R32G32_SFLOAT;
      case VertexFormat::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
      case VertexFormat::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
      default: throw std::runtime_error("Unhandled Vertex Forma in VertexFormatToVkFormat");
    }
  }

  uint32_t GetStride() const { return m_stride; }
  const std::vector<VertexAttribute>& GetAttribute() const { return m_attributes; }

private:
  std::vector<VertexAttribute> m_attributes;
  uint32_t m_stride{0};
};

}
