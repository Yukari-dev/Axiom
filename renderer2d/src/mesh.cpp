#include "mesh.hpp"

namespace Axiom{

Mesh::Mesh(VkPhysicalDevice pDevice, VkDevice device) : m_physicalDevice(pDevice), m_device(device){

}

void Mesh::EnsureCapacity(VkDeviceSize vertSize, VkDeviceSize indSize){
  if (!m_vertexBuffer || vertSize > m_verticesCapacity){
    m_verticesCapacity = vertSize * 2;
    m_vertexBuffer = std::make_unique<Buffer>(
      m_physicalDevice, m_device, m_verticesCapacity, 
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
  }
  if (!m_indexBuffer || indSize > m_indicesCapacity){
    m_indicesCapacity = indSize * 2;
    m_indexBuffer = std::make_unique<Buffer>(
      m_physicalDevice, m_device, m_indicesCapacity, 
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
  }
}

void Mesh::SetData(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices){
  if(vertices.empty() || indices.empty()){
    m_indexCount = 0;
    return;
  }

  VkDeviceSize vertSize = sizeof(Vertex) * vertices.size();
  VkDeviceSize indSize = sizeof(uint16_t) * indices.size();

  EnsureCapacity(vertSize, indSize);

  m_vertexBuffer->CopyData(vertices.data(), vertSize);
  m_indexBuffer->CopyData(indices.data(), indSize);

  m_indexCount = static_cast<uint32_t>(indices.size());
}

void Mesh::Bind(CommandBuffer& cmd){
  cmd.BindVertexBuffer(m_vertexBuffer->GetBuffer());
  cmd.BindIndexBuffer(m_indexBuffer->GetBuffer());
}

}
