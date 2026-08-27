#include "commandBuffer.hpp"
#include <stdexcept>

namespace Axiom{

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool pool) : m_device(device), m_commandPool(pool){
  Create();
}

void CommandBuffer::Create(){
  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandBufferCount = 1;
  info.commandPool = m_commandPool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  VkResult result = vkAllocateCommandBuffers(m_device, &info, &m_commandBuffer);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate Command buffer");
}

void CommandBuffer::Begin(){
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

void CommandBuffer::BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent){
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

  VkRenderPassBeginInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  info.renderPass = renderPass;
  info.framebuffer = framebuffer;
  info.clearValueCount = 1;
  info.pClearValues = &clearColor;
  info.renderArea.offset = {0, 0};
  info.renderArea.extent = extent;

  vkCmdBeginRenderPass(m_commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindPipeline(VkPipeline pipeline){
  vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::Draw(uint32_t vertexCount){
  vkCmdDraw(m_commandBuffer, vertexCount, 1, 0, 0);
}

void CommandBuffer::EndRenderPass(){
  vkCmdEndRenderPass(m_commandBuffer);
}

void CommandBuffer::End(){
  vkEndCommandBuffer(m_commandBuffer);
}

}
