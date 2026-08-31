#pragma once
#include <string>

class GLFWwindow;

namespace Axiom {

void Init();
void Shutdown();

class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();

  bool ShouldClose();
  GLFWwindow *GetHandler() const { return m_window; }
  void PollEvents();
private:
  void Create();
  GLFWwindow *m_window;
  int m_width, m_height;
  std::string m_title;
};

}
