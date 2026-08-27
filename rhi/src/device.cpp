#include "device.hpp"
#include <stdexcept>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace Axiom{

Device::Device(const VkInstance& instance, VkSurfaceKHR surface) : m_instance(instance), m_surface(surface){
  PickPhysicalDevice();
  CreateLogicalDevice();
}

Device::~Device(){
  if(m_device)
    vkDestroyDevice(m_device, nullptr);
}

void Device::PickPhysicalDevice(){
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
  if(deviceCount == 0)
    throw std::runtime_error("Failed to find GPUs with Vulkan Support");
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

  std::multimap<int, VkPhysicalDevice> candidates;
  for(const auto& device : devices){
    int score = RateDeviceSuitabilty(device);
    candidates.insert(std::make_pair(score, device));
  }
  if(candidates.rbegin()->first > 0)
    m_physicalDevice = candidates.rbegin()->second;
  else
    throw std::runtime_error("Failed to find a suitable GPU");
}

void Device::CreateLogicalDevice(){
  m_indices = FindQueueFamilies(m_physicalDevice);
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = m_indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {m_indices.graphicsFamily.value(), m_indices.presentFamily.value()};
  float queuePriority = 1.0f;
  for(uint32_t queueFamily : uniqueQueueFamilies){
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeature{};
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pEnabledFeatures = &deviceFeature;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
  
  createInfo.ppEnabledLayerNames = nullptr;
  createInfo.enabledLayerCount = 0;

  VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Logical Device");
  vkGetDeviceQueue(m_device, m_indices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_device, m_indices.presentFamily.value(), 0, &m_presentQueue);
}

int Device::RateDeviceSuitabilty(VkPhysicalDevice device){
  if(!IsDeviceSuitable(device)) return 0;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  int score = 0;
  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  score += deviceProperties.limits.maxImageDimension2D;

  if(!deviceFeatures.geometryShader)
    return 0;

  return score;
}

QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device){
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
  int i = 0;
  for(const auto& family : queueFamilies){
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
    if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      indices.graphicsFamily = i;
    if(presentSupport)
      indices.presentFamily = i;
    if(indices.IsComplete())
      break;
    i++;
  }

  return indices;
}

bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device){
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
  std::set<std::string> requiredExtension(m_deviceExtensions.begin(), m_deviceExtensions.end());
  for(const auto& extension : availableExtensions)
    requiredExtension.insert(extension.extensionName);
  return !requiredExtension.empty();
}

SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface){
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if(formatCount != 0){
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
  if(presentModeCount != 0){
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }
  return details;
}

bool Device::IsDeviceSuitable(VkPhysicalDevice device){
  QueueFamilyIndices indices = FindQueueFamilies(device);
  bool extensionSupport = CheckDeviceExtensionSupport(device);
  bool swapChainAdequate = false;
  if(extensionSupport){
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, m_surface);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }
  return indices.IsComplete() && extensionSupport && swapChainAdequate;
}

}
