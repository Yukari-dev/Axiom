#pragma once
#include <cstdint>
#include <optional>
#include <vulkan/vulkan.hpp>
#include "instance.hpp"

namespace Axiom{

struct QueueFamilyIndices{
  std::optional<uint32_t> graphicsFamily;

  bool IsComplete() { return graphicsFamily.has_value(); }
};

class Device{
public:
  Device(const Instance& instance);
  ~Device();

private:
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  bool IsDeviceSuitable(VkPhysicalDevice device);
  int RateDeviceSuitabilty(VkPhysicalDevice device);
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
private:
  VkDevice m_device = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkQueue m_graphicsQueue;
  Instance m_instance;
};

}
