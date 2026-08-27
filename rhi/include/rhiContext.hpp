#pragma once
#include "commandPool.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "instance.hpp"
#include "pipeline.hpp"
#include "renderPass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "commandBuffer.hpp"
#include "syncObjects.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace Axiom{

class RhiContext{
public:
  RhiContext(GLFWwindow *window, int width, int height);

  void DrawFrame();
private:
  static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
  void RecreateSwapChain();
private:
  GLFWwindow *m_window{nullptr};
  bool m_framebufferResized{false};
  std::unique_ptr<Instance> m_instance{nullptr};
  std::unique_ptr<Surface> m_surface{nullptr};
  std::unique_ptr<Device> m_device{nullptr};
  std::unique_ptr<SwapChain> m_swapChain{nullptr};
  std::unique_ptr<RenderPass> m_renderPass{nullptr};
  std::unique_ptr<Pipeline> m_pipeline{nullptr};
  std::unique_ptr<Framebuffer> m_framebuffer{nullptr};
  std::unique_ptr<CommandPool> m_commandPool{nullptr};
  std::unique_ptr<CommandBuffer> m_commandBuffer{nullptr};
  std::unique_ptr<SyncObjects> m_syncObjects{nullptr};
};

}
