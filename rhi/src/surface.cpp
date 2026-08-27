#include "surface.hpp"
#include <stdexcept>

namespace Axiom{

Surface::Surface(VkInstance instance, GLFWwindow *window) : m_instance(instance){
  if(glfwCreateWindowSurface(instance, window, nullptr, &m_surface) != VK_SUCCESS){
    throw std::runtime_error("Failed to create Window Surface");
  }
}

Surface::~Surface(){
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

}
