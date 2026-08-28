#pragma once
#include <cstdint>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace Axiom{

struct QueueFamilyIndices{
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class Device{
public:
  Device(const VkInstance& instance, VkSurfaceKHR surface);
  ~Device();
  
  VkCommandBuffer BeginSingleTimeCommands(VkCommandPool pool);
  void EndSingleTimeCommands(VkCommandBuffer cmd, VkCommandPool pool, VkQueue queue);

  VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
  VkDevice GetDevice() const { return m_device; }

  VkQueue GetPresentQueue() const { return m_presentQueue; }
  VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
  uint32_t GetGraphicsQueueFamily() const { return m_indices.graphicsFamily.value(); }
private:
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  bool IsDeviceSuitable(VkPhysicalDevice device);
  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
  int RateDeviceSuitabilty(VkPhysicalDevice device);
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;
  VkSurfaceKHR m_surface;
  VkInstance m_instance;
  QueueFamilyIndices m_indices{0};
  const std::vector<const char *> m_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
};

}
