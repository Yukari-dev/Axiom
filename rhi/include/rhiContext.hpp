#pragma once
#include <memory>
#include <cstdint>

struct GLFWwindow;
typedef struct VkFence_T* VkFence;

namespace Axiom {

class Instance;
class Surface;
class Device;
class SwapChain;
class RenderPass;
class Framebuffer;
class CommandPool;
class CommandBuffer;
class SyncObjects;

class RhiContext {
public:
  RhiContext(GLFWwindow* window, int width, int height);
  ~RhiContext();

  void BeginFrame();
  void EndFrame();
  void WaitIdle();

  Device& GetDeviceObject() const;
  CommandPool& GetCommandPoolObject() const;
  CommandBuffer& GetCommandBufferObject() const;
  RenderPass& GetRenderPassObject() const;
  SwapChain& GetSwapChainObject() const;
  Framebuffer& GetFramebufferObject() const;
  Instance& GetInstanceObject() const;
  Surface& GetSurfaceObject() const;
  SyncObjects& GetSyncObjectsObject() const;
  uint32_t GetImageIndex() const;
private:
  static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  void RecreateSwapChain();

  GLFWwindow* m_window{nullptr};
  VkFence m_inFlightFence{};
  bool m_framebufferResized{false};
  uint32_t m_imageIndex{0};
  std::unique_ptr<Instance> m_instance;
  std::unique_ptr<Surface> m_surface;
  std::unique_ptr<Device> m_device;
  std::unique_ptr<SwapChain> m_swapChain;
  std::unique_ptr<RenderPass> m_renderPass;
  std::unique_ptr<Framebuffer> m_framebuffer;
  std::unique_ptr<CommandPool> m_commandPool;
  std::unique_ptr<CommandBuffer> m_commandBuffer;
  std::unique_ptr<SyncObjects> m_syncObjects;
};

}
