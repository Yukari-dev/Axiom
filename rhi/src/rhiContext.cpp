#include "rhiContext.hpp"
#include "syncObjects.hpp"
#include "vertex.hpp"
#include <chrono>
#include <stdexcept>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace Axiom{

RhiContext::RhiContext(GLFWwindow *window, int width, int height) : m_window(window){
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
  m_instance = std::make_unique<Instance>();
  m_surface = std::make_unique<Surface>(m_instance->GetInstance(), window);
  m_device = std::make_unique<Device>(m_instance->GetInstance(), m_surface->GetSurface());
  m_swapChain = std::make_unique<SwapChain>(
    m_device->GetPhysicalDevice(), m_device->GetDevice(), m_surface->GetSurface(), window 
  );
  m_renderPass = std::make_unique<RenderPass>(
    m_device->GetDevice(), m_swapChain->GetFormat()
  );
  m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_device->GetDevice());
  m_pipeline = std::make_unique<Pipeline>(
    m_device->GetDevice(), m_swapChain->GetExtent(), m_renderPass->GetRenderPass(),
    m_descriptorSetLayout->GetDescriptorSetLayout()
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
  std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
  };
  std::vector<uint16_t> indices = {
    0, 2, 1,
    2, 0, 3
  };
  m_indexCount = static_cast<uint32_t>(indices.size());
  m_vertexBuffer = std::make_unique<Buffer>(
    Buffer::CreateDeviceLocalBuffer(
      m_device->GetPhysicalDevice(), m_device->GetDevice(),
      m_commandPool->GetHandler(), m_device->GetGraphicsQueue(),
      vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    )
  );
  m_indexBuffer = std::make_unique<Buffer>(
    Buffer::CreateDeviceLocalBuffer(
      m_device->GetPhysicalDevice(), m_device->GetDevice(),
      m_commandPool->GetHandler(), m_device->GetGraphicsQueue(),
      indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    )
  );
  m_texture = std::make_unique<Texture>(
    *m_device, m_commandPool->GetHandler(), std::string(AXIOM_TEXTURE_DIR) + "test.jpg"
  );

  size_t imageCount = m_swapChain->GetImageViews().size();
  m_uniformBuffers.resize(imageCount);
  m_descriptorSets.resize(imageCount);
  for(size_t i = 0; i < imageCount; i++){
    m_uniformBuffers[i] = std::make_unique<Buffer>(
      m_device->GetPhysicalDevice(), m_device->GetDevice(),
      sizeof(UniformBufferObject),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_descriptorSets[i] = std::make_unique<DescriptorSet>(
      m_device->GetDevice(), m_descriptorSetLayout->GetDescriptorSetLayout()
    );
    m_descriptorSets[i]->UpdateDescriptorSet(*m_uniformBuffers[i], sizeof(UniformBufferObject), *m_texture);
  }
}

void RhiContext::DrawFrame(){
  VkFence inFlightFence = m_syncObjects->GetFence();
  VkDevice device = m_device->GetDevice();
  vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &inFlightFence);
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
    device, m_swapChain->GetSwapChain(), UINT64_MAX, m_syncObjects->GetImageAvailable(),
    VK_NULL_HANDLE, &imageIndex
  );

  if(result == VK_ERROR_OUT_OF_DATE_KHR){
    RecreateSwapChain();
    return;
  } else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw std::runtime_error("Failed to acquire swap chain image.");

  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), m_swapChain->GetExtent().width / (float)m_swapChain->GetExtent().height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;

  m_uniformBuffers[imageIndex]->CopyData(&ubo, sizeof(ubo));

  vkResetCommandBuffer(m_commandBuffer->GetHandler(), 0);
  m_commandBuffer->Begin();
  m_commandBuffer->BeginRenderPass(
    m_renderPass->GetRenderPass(), m_framebuffer->GetFramebuffer(imageIndex),
    m_swapChain->GetExtent()
  );
  m_commandBuffer->BindPipeline(m_pipeline->GetPipeline());
  m_commandBuffer->SetViewport(m_swapChain->GetExtent());
  m_commandBuffer->SetScissor(m_swapChain->GetExtent());
  
  VkDescriptorSet currentSet = m_descriptorSets[imageIndex]->GetSet();
  vkCmdBindDescriptorSets(
    m_commandBuffer->GetHandler(), VK_PIPELINE_BIND_POINT_GRAPHICS,
    m_pipeline->GetPipelineLayout(), 0, 1, &currentSet, 0, nullptr
  );

  m_commandBuffer->BindVertexBuffer(m_vertexBuffer->GetBuffer());
  m_commandBuffer->BindIndexBuffer(m_indexBuffer->GetBuffer());
  m_commandBuffer->DrawIndexed(m_indexCount);
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

  VkResult qResult = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);
  if(qResult != VK_SUCCESS)
    throw std::runtime_error("Failed to submit draw command buffer");

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  VkSwapchainKHR swapchains[] = { m_swapChain->GetSwapChain() };
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(m_device->GetGraphicsQueue(), &presentInfo);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized){
    m_framebufferResized = false;
    RecreateSwapChain();
  } else if (result != VK_SUCCESS)
    throw std::runtime_error("Failed to present swap chain image");
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

}
