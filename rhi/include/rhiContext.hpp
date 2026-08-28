#pragma once
#include "buffer.hpp"
#include "commandPool.hpp"
#include "descriptorSet.hpp"
#include "descriptorSetLayout.hpp"
#include "device.hpp"
#include "framebuffer.hpp"
#include "instance.hpp"
#include "pipeline.hpp"
#include "renderPass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "commandBuffer.hpp"
#include "syncObjects.hpp"
#include "texture.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>
#include <vector>

namespace Axiom{

struct UniformBufferObject{
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

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
  uint32_t m_indexCount{0};
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
  std::unique_ptr<Buffer> m_vertexBuffer{nullptr};
  std::unique_ptr<Buffer> m_indexBuffer{nullptr};
  std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout{nullptr};
  std::vector<std::unique_ptr<Buffer>> m_uniformBuffers;
  std::vector<std::unique_ptr<DescriptorSet>> m_descriptorSets;
  std::unique_ptr<Texture> m_texture{nullptr};
};

}
