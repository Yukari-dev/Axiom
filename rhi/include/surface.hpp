#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Axiom{

class Surface{
public:
  Surface(VkInstance instance, GLFWwindow *window);
  ~Surface();
  
  VkSurfaceKHR GetSurface() const { return m_surface; }
private:
  VkInstance m_instance;
  VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};

}
