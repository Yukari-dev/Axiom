#include "include/input.hpp"

namespace Axiom {

Input::Input(GLFWwindow* window) : m_window(window) {}

void Input::Update() {
  if (!m_window) return;

  m_previousKeyStates = m_currentKeyStates;
  m_previousMouseStates = m_currentMouseStates;
}

bool Input::IsKeyDown(Key key) const {
  if (!m_window) return false;
  return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_PRESS;
}

bool Input::IsKeyUp(Key key) const {
  return !IsKeyDown(key);
}

bool Input::IsKeyPressed(Key key) const {
  int keycode = static_cast<int>(key);
  bool current = IsKeyDown(key);
  auto it = m_previousKeyStates.find(keycode);
  bool previous = (it != m_previousKeyStates.end()) ? it->second : false;

  const_cast<Input*>(this)->m_currentKeyStates[keycode] = current;

  return current && !previous;
}

bool Input::IsKeyReleased(Key key) const {
  int keycode = static_cast<int>(key);
  bool current = IsKeyDown(key);
  auto it = m_previousKeyStates.find(keycode);
  bool previous = (it != m_previousKeyStates.end()) ? it->second : false;

  const_cast<Input*>(this)->m_currentKeyStates[keycode] = current;

  return !current && previous;
}

bool Input::IsMouseButtonDown(MouseButton button) const {
  if (!m_window) return false;
  return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS;
}

bool Input::IsMouseButtonUp(MouseButton button) const {
  return !IsMouseButtonDown(button);
}

bool Input::IsMouseButtonPressed(MouseButton button) const {
  int btnCode = static_cast<int>(button);
  bool current = IsMouseButtonDown(button);
  auto it = m_previousMouseStates.find(btnCode);
  bool previous = (it != m_previousMouseStates.end()) ? it->second : false;

  const_cast<Input*>(this)->m_currentMouseStates[btnCode] = current;

  return current && !previous;
}

bool Input::IsMouseButtonReleased(MouseButton button) const {
  int btnCode = static_cast<int>(button);
  bool current = IsMouseButtonDown(button);
  auto it = m_previousMouseStates.find(btnCode);
  bool previous = (it != m_previousMouseStates.end()) ? it->second : false;

  const_cast<Input*>(this)->m_currentMouseStates[btnCode] = current;

  return !current && previous;
}

glm::vec2 Input::GetMousePosition() const {
  if (!m_window) return {0.0f, 0.0f};
  double x, y;
  glfwGetCursorPos(m_window, &x, &y);
  return {static_cast<float>(x), static_cast<float>(y)};
}

}
