#pragma once
#include <vulkan/vulkan.hpp>

namespace Axiom{

class CommandBuffer{
public:
  CommandBuffer(VkDevice device, VkCommandPool pool);

  void Begin();
  void BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
  void BindPipeline(VkPipeline pipeline);
  void Draw(uint32_t vertexCount);
  void BindVertexBuffer(VkBuffer buffer);
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
