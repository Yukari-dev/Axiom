#pragma once
#include <functional>
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
  using ResizeCallback = std::function<void(int, int)>;
  void SetResizeCallback(ResizeCallback callback);

  using CursorPosCallback = std::function<void(double, double)>;
  void SetCursorPosCallback(CursorPosCallback callback);

  using MouseButtonCallback = std::function<void(int, int, int)>;
  void SetMouseButtonCallback(MouseButtonCallback callback);
private:
  static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  static void CursorposCallback(GLFWwindow *window, double xpos, double ypos);
  static void MouseButtonInternalCallback(GLFWwindow *window, int button, int actions, int modes);

  void RecreateSwapChain();

  GLFWwindow* m_window{nullptr};
  VkFence m_inFlightFence{};
  bool m_framebufferResized{false};
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
  ResizeCallback m_userResizeCallback{};
  CursorPosCallback m_userCursorPosCallback{};
  MouseButtonCallback m_userMouseButtonCallback{};
};

}
