#include "rhiContext.hpp"
#include "rhiContext.hpp"
#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderPass.hpp"
#include "framebuffer.hpp"
#include "commandPool.hpp"
#include "commandBuffer.hpp"
#include "syncObjects.hpp"
#include <stdexcept>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <vector>

namespace Axiom{

RhiContext::RhiContext(GLFWwindow *window, int width, int height) : m_window(window){
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
  m_instance = std::make_unique<Instance>();
  m_surface = std::make_unique<Surface>(m_instance->GetInstance(), m_window);
  m_device = std::make_unique<Device>(m_instance->GetInstance(), m_surface->GetSurface());
  m_swapChain = std::make_unique<SwapChain>(
    m_device->GetPhysicalDevice(), m_device->GetDevice(), m_surface->GetSurface(), window 
  );
  m_renderPass = std::make_unique<RenderPass>(
    m_device->GetDevice(), m_swapChain->GetFormat()
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

RhiContext::~RhiContext() = default;

void RhiContext::BeginFrame(){
  m_inFlightFence = m_syncObjects->GetFence();
  VkDevice device = m_device->GetDevice();
  vkWaitForFences(device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &m_inFlightFence);
  VkResult result = vkAcquireNextImageKHR(
    device, m_swapChain->GetSwapChain(), UINT64_MAX, m_syncObjects->GetImageAvailable(),
    VK_NULL_HANDLE, &m_imageIndex
  );

  if(result == VK_ERROR_OUT_OF_DATE_KHR){
    RecreateSwapChain();
    return;
  } else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw std::runtime_error("Failed to acquire swap chain image.");

  vkResetCommandBuffer(m_commandBuffer->GetHandler(), 0);
  m_commandBuffer->Begin();
  m_commandBuffer->BeginRenderPass(
    m_renderPass->GetRenderPass(), m_framebuffer->GetFramebuffer(m_imageIndex),
    m_swapChain->GetExtent()
  );
  m_commandBuffer->SetViewport(m_swapChain->GetExtent());
  m_commandBuffer->SetScissor(m_swapChain->GetExtent());
}

void RhiContext::EndFrame(){
  vkCmdEndRenderPass(m_commandBuffer->GetHandler());
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

  VkSemaphore signalSemaphores[] = {m_syncObjects->GetRenderFinished(m_imageIndex)};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkResult qResult = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, m_inFlightFence);
  if(qResult != VK_SUCCESS)
    throw std::runtime_error("Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  VkSwapchainKHR swapchains[] = { m_swapChain->GetSwapChain() };
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &m_imageIndex;

  VkResult result = vkQueuePresentKHR(m_device->GetGraphicsQueue(), &presentInfo);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized){
    m_framebufferResized = false;
    RecreateSwapChain();
  } else if (result != VK_SUCCESS)
    throw std::runtime_error("Failed to present swap chain image");
}

void RhiContext::WaitIdle(){
  vkDeviceWaitIdle(m_device->GetDevice());
}

void RhiContext::RecreateSwapChain(){
  int width = 0, height = 0;
  glfwGetFramebufferSize(m_window, &width, &height);
  while (width == 0 || height == 0){
    glfwGetFramebufferSize(m_window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(m_device->GetDevice());

  m_framebuffer.reset();
  m_swapChain.reset();

  m_swapChain = std::make_unique<SwapChain>(
    m_device->GetPhysicalDevice(), m_device->GetDevice(), m_surface->GetSurface(), m_window 
  );
  m_framebuffer = std::make_unique<Framebuffer>(
    m_device->GetDevice(), m_swapChain->GetImageViews(), m_swapChain->GetExtent(), m_renderPass->GetRenderPass()
  );
}

void RhiContext::FramebufferResizeCallback(GLFWwindow *window, int width, int height){
  auto* self = reinterpret_cast<RhiContext*>(glfwGetWindowUserPointer(window));
  self->m_framebufferResized = true;
}

Device& RhiContext::GetDeviceObject() const { return *m_device; }
CommandPool& RhiContext::GetCommandPoolObject() const { return *m_commandPool; }
CommandBuffer& RhiContext::GetCommandBufferObject() const { return *m_commandBuffer; }
RenderPass& RhiContext::GetRenderPassObject() const { return *m_renderPass; }
SwapChain& RhiContext::GetSwapChainObject() const { return *m_swapChain; }
Framebuffer& RhiContext::GetFramebufferObject() const { return *m_framebuffer; }
Instance& RhiContext::GetInstanceObject() const { return *m_instance; }
Surface& RhiContext::GetSurfaceObject() const { return *m_surface; }
SyncObjects& RhiContext::GetSyncObjectsObject() const { return *m_syncObjects; }
uint32_t RhiContext::GetImageIndex() const { return m_imageIndex; }

}
