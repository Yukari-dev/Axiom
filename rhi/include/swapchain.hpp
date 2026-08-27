#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class SwapChain{
public:
  SwapChain(VkPhysicalDevice pDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow *window);
  ~SwapChain();

  VkSwapchainKHR GetSwapChain() const { return m_swapChain; }
  VkFormat GetFormat() const { return m_swapChainFormat; }
  VkExtent2D GetExtent() const { return m_swapChainExtent; }
private:
  void Create();
  void CreateImageViews();
  VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
private:
  VkSwapchainKHR m_swapChain;
  VkPhysicalDevice m_physicalDevice;
  VkDevice m_device;
  VkSurfaceKHR m_surface;
  std::vector<VkImage> m_swapChainImages;
  std::vector<VkImageView> m_swapChainImageViews;
  VkFormat m_swapChainFormat;
  VkExtent2D m_swapChainExtent;
  GLFWwindow *m_window;
};

}
