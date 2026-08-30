#pragma once
#include "commandPool.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "instance.hpp"
#include "renderPass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "commandBuffer.hpp"
#include "syncObjects.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

namespace Axiom{

struct UniformBufferObject{
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

class RhiContext{
public:
  RhiContext(GLFWwindow *window, int width, int height);

  void BeginFrame();
  void EndFrame();

  VkDevice GetDevice() const { return m_device->GetDevice(); }
  Device& GetDeviceObject() const { return *m_device; }
  VkPhysicalDevice GetPhysicalDevice() const { return m_device->GetPhysicalDevice(); }
  VkQueue GetGraphicsQueue() const { return m_device->GetGraphicsQueue(); }
  VkCommandPool GetCommandPool() const { return m_commandPool->GetHandler(); }
  VkCommandBuffer GetCommandBuffer() const { return m_commandBuffer->GetHandler(); }
  CommandBuffer& GetCommandBufferObject() const { return *m_commandBuffer; }
  VkRenderPass GetRenderPass() const { return m_renderPass->GetRenderPass(); }
  VkExtent2D GetExtent() const { return m_swapChain->GetExtent(); }
  uint32_t GetImageIndex() const { return m_imageIndex; }
  size_t GetSwapChainImageCount() const { return m_swapChain->GetImageViews().size(); }
private:
  static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
  void RecreateSwapChain();
private:
  GLFWwindow *m_window{nullptr};
  VkFence m_inFlightFence{};
  bool m_framebufferResized{false};
  uint32_t m_indexCount{0};
  uint32_t m_imageIndex{0};
  std::unique_ptr<Instance> m_instance{nullptr};
  std::unique_ptr<Surface> m_surface{nullptr};
  std::unique_ptr<Device> m_device{nullptr};
  std::unique_ptr<SwapChain> m_swapChain{nullptr};
  std::unique_ptr<RenderPass> m_renderPass{nullptr};
  std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
  std::unique_ptr<CommandPool> m_commandPool{nullptr};
  std::unique_ptr<CommandBuffer> m_commandBuffer{nullptr};
  std::unique_ptr<SyncObjects> m_syncObjects{nullptr};
};

}
