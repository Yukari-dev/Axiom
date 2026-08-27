#include "rhiContext.hpp"
#include "syncObjects.hpp"

namespace Axiom{

RhiContext::RhiContext(GLFWwindow *window, int width, int height){
  m_instance = std::make_unique<Instance>();
  m_surface = std::make_unique<Surface>(m_instance->GetInstance(), window);
  m_device = std::make_unique<Device>(m_instance->GetInstance(), m_surface->GetSurface());
  m_swapChain = std::make_unique<SwapChain>(
    m_device->GetPhysicalDevice(), m_device->GetDevice(), m_surface->GetSurface(), window 
  );
  m_renderPass = std::make_unique<RenderPass>(
    m_device->GetDevice(), m_swapChain->GetFormat()
  );
  m_pipeline = std::make_unique<Pipeline>(
    m_device->GetDevice(), m_swapChain->GetExtent(), m_renderPass->GetRenderPass()
  );
  m_framebuffer = std::make_unique<Framebuffer>(
    m_device->GetDevice(), m_swapChain->GetImageViews(), m_swapChain->GetExtent(), m_renderPass->GetRenderPass()
  );
  m_commandPool = std::make_unique<CommandPool>(m_device->GetDevice(), m_device->GetGraphicsQueueFamily());
  m_commandBuffer = std::make_unique<CommandBuffer>(m_device->GetDevice(), m_commandPool->GetHandler());
  m_syncObjects = std::make_unique<SyncObjects>(
    m_device->GetDevice(), 
    static_cast<uint32_t>(m_swapChain->GetImageViews().size())
  );
}

void RhiContext::DrawFrame(){
  VkFence inFlightFence = m_syncObjects->GetFence();
  VkDevice device = m_device->GetDevice();
  vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &inFlightFence);
  uint32_t imageIndex;
  vkAcquireNextImageKHR(
    device, m_swapChain->GetSwapChain(), UINT64_MAX, m_syncObjects->GetImageAvailable(),
    VK_NULL_HANDLE, &imageIndex
  );

  vkResetCommandBuffer(m_commandBuffer->GetHandler(), 0);
  m_commandBuffer->Begin();
  m_commandBuffer->BeginRenderPass(
    m_renderPass->GetRenderPass(), m_framebuffer->GetFramebuffer(imageIndex),
    m_swapChain->GetExtent()
  );
  m_commandBuffer->BindPipeline(m_pipeline->GetPipeline());
  m_commandBuffer->Draw(3);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->End();

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {m_syncObjects->GetImageAvailable()};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer commandBuffer = m_commandBuffer->GetHandler();
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {m_syncObjects->GetRenderFinished(imageIndex)};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkResult result = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  VkSwapchainKHR swapchains[] = { m_swapChain->GetSwapChain() };
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;

  vkQueuePresentKHR(m_device->GetGraphicsQueue(), &presentInfo);
}

}
