#include "input.hpp"

namespace Axiom {

Input::Input(GLFWwindow* window) : m_window(window) {}

void Input::Update() {
  if (!m_window) return;

  m_previousKeyStates = m_currentKeyStates;
  m_previousMouseStates = m_currentMouseStates;

  for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
    m_currentKeyStates[key] = (glfwGetKey(m_window, key) == GLFW_PRESS);
  }

  for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
    m_currentMouseStates[button] = (glfwGetMouseButton(m_window, button) == GLFW_PRESS);
  }
}

bool Input::IsKeyDown(Key key) const {
  auto it = m_currentKeyStates.find(static_cast<int>(key));
  return (it != m_currentKeyStates.end()) ? it->second : false;
}

bool Input::IsKeyUp(Key key) const {
  return !IsKeyDown(key);
}

bool Input::IsKeyPressed(Key key) const {
  int keycode = static_cast<int>(key);

  auto currIt = m_currentKeyStates.find(keycode);
  bool current = (currIt != m_currentKeyStates.end()) ? currIt->second : false;

  auto prevIt = m_previousKeyStates.find(keycode);
  bool previous = (prevIt != m_previousKeyStates.end()) ? prevIt->second : false;

  return current && !previous;
}

bool Input::IsKeyReleased(Key key) const {
  int keycode = static_cast<int>(key);

  auto currIt = m_currentKeyStates.find(keycode);
  bool current = (currIt != m_currentKeyStates.end()) ? currIt->second : false;

  auto prevIt = m_previousKeyStates.find(keycode);
  bool previous = (prevIt != m_previousKeyStates.end()) ? prevIt->second : false;

  return !current && previous;
}

bool Input::IsMouseButtonDown(MouseButton button) const {
  auto it = m_currentMouseStates.find(static_cast<int>(button));
  return (it != m_currentMouseStates.end()) ? it->second : false;
}

bool Input::IsMouseButtonUp(MouseButton button) const {
  return !IsMouseButtonDown(button);
}

bool Input::IsMouseButtonPressed(MouseButton button) const {
  int btnCode = static_cast<int>(button);

  auto currIt = m_currentMouseStates.find(btnCode);
  bool current = (currIt != m_currentMouseStates.end()) ? currIt->second : false;

  auto prevIt = m_previousMouseStates.find(btnCode);
  bool previous = (prevIt != m_previousMouseStates.end()) ? prevIt->second : false;

  return current && !previous;
}

bool Input::IsMouseButtonReleased(MouseButton button) const {
  int btnCode = static_cast<int>(button);

  auto currIt = m_currentMouseStates.find(btnCode);
  bool current = (currIt != m_currentMouseStates.end()) ? currIt->second : false;

  auto prevIt = m_previousMouseStates.find(btnCode);
  bool previous = (prevIt != m_previousMouseStates.end()) ? prevIt->second : false;

  return !current && previous;
}

glm::vec2 Input::GetMousePosition() const {
  if (!m_window) return {0.0f, 0.0f};
  double x, y;
  glfwGetCursorPos(m_window, &x, &y);
  return {static_cast<float>(x), static_cast<float>(y)};
}

}
