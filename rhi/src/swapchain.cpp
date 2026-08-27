#include "swapchain.hpp"
#include <GLFW/glfw3.h>
#include <optional>
#include <stdexcept>

namespace Axiom{

struct SwapChainSupportDetails{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface){
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

struct QueueFamilyIndices{
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface){
  QueueFamilyIndices indices;
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
  int i = 0;
  for(const auto& family : queueFamilies){
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
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

SwapChain::SwapChain(VkPhysicalDevice pDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow *window) :
  m_physicalDevice(pDevice), m_device(device), m_surface(surface), m_window(window){
  Create();
  CreateImageViews();
}

SwapChain::~SwapChain(){
  for(auto imageView : m_swapChainImageViews){
    vkDestroyImageView(m_device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void SwapChain::Create(){
  SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice, m_surface);

  VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = ChooseSwapChainPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

  m_swapChainFormat = surfaceFormat.format;
  m_swapChainExtent = extent;

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if(swapChainSupport.capabilities.maxImageCount > 0 &&
     imageCount > swapChainSupport.capabilities.maxImageCount)
    imageCount = swapChainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice, m_surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  if(indices.graphicsFamily != indices.presentFamily){
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else{
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create swap chain.");
  
  vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
  m_swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
}

void SwapChain::CreateImageViews(){
  m_swapChainImageViews.resize(m_swapChainImages.size());

  for(size_t i = 0; i < m_swapChainImages.size(); i++){
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapChainFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    VkResult result = vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]);
    if(result != VK_SUCCESS)
      throw std::runtime_error("Failed to create Image View.");
  }
}

VkSurfaceFormatKHR SwapChain::ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
  for(const auto& format : availableFormats){
    if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return format;
  }
  return availableFormats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes){
  for(const auto& mode : availablePresentModes){
    if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
      return mode;
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
  if(capabilities.currentExtent.width != UINT32_MAX)
    return capabilities.currentExtent;
  else{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    VkExtent2D extent = {
      .width = static_cast<uint32_t>(width),
      .height= static_cast<uint32_t>(height)
    };
    extent.width = std::clamp(
      extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width
    );
    extent.height = std::clamp(
      extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height
    );

    return extent;
  }
}

}
