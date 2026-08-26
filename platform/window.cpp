#include "window.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Axiom {

void Init() { 
  if (!glfwInit())
    std::cerr << "Failed to initialize GLFW\n";
}

void Shutdown() { 
  glfwTerminate(); 
}

Window::Window(int width, int height, const std::string &title)
    : m_width(width), m_height(height), m_title(title) {
  Create();
}

Window::~Window() { 
  if (m_window)
    glfwDestroyWindow(m_window); 
}

void Window::Create() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
  if (!m_window) {
    std::cerr << "Failed to create GLFW window\n";
    return;
  }
}

bool Window::ShouldClose() { 
  return m_window ? glfwWindowShouldClose(m_window) : true; 
}

void Window::PollEvents() { 
  glfwPollEvents(); 
}

} // namespace Axiom
