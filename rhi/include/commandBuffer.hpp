#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class CommandBuffer{
public:
  CommandBuffer(VkDevice device, VkCommandPool pool);

  void Begin();
  void BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
  void BindPipeline(VkPipeline pipeline);
  void BindVertexBuffer(VkBuffer buffer);
  void BindIndexBuffer(VkBuffer buffer, VkIndexType indexType = VK_INDEX_TYPE_UINT16);
  void Draw(uint32_t vertexCount);
  void DrawIndexed(uint32_t indexCount);
  void SetViewport(VkExtent2D extent);
  void SetScissor(VkExtent2D extent);
  void EndRenderPass();
  void End();

  VkCommandBuffer GetHandler() const { return m_commandBuffer; }
private:
  void Create();
private:
  VkCommandBuffer m_commandBuffer;
  VkCommandPool m_commandPool;
  VkDevice m_device;
};

}
