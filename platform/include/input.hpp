#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Axiom {

enum class Key {
  UNKNOWN = GLFW_KEY_UNKNOWN,

  SPACE = GLFW_KEY_SPACE,
  APOSTROPHE = GLFW_KEY_APOSTROPHE,
  COMMA = GLFW_KEY_COMMA,
  MINUS = GLFW_KEY_MINUS,
  PERIOD = GLFW_KEY_PERIOD,
  SLASH = GLFW_KEY_SLASH,

  NUM_0 = GLFW_KEY_0,
  NUM_1 = GLFW_KEY_1,
  NUM_2 = GLFW_KEY_2,
  NUM_3 = GLFW_KEY_3,
  NUM_4 = GLFW_KEY_4,
  NUM_5 = GLFW_KEY_5,
  NUM_6 = GLFW_KEY_6,
  NUM_7 = GLFW_KEY_7,
  NUM_8 = GLFW_KEY_8,
  NUM_9 = GLFW_KEY_9,

  A = GLFW_KEY_A,
  B = GLFW_KEY_B,
  C = GLFW_KEY_C,
  D = GLFW_KEY_D,
  E = GLFW_KEY_E,
  F = GLFW_KEY_F,
  G = GLFW_KEY_G,
  H = GLFW_KEY_H,
  I = GLFW_KEY_I,
  J = GLFW_KEY_J,
  K = GLFW_KEY_K,
  L = GLFW_KEY_L,
  M = GLFW_KEY_M,
  N = GLFW_KEY_N,
  O = GLFW_KEY_O,
  P = GLFW_KEY_P,
  Q = GLFW_KEY_Q,
  R = GLFW_KEY_R,
  S = GLFW_KEY_S,
  T = GLFW_KEY_T,
  U = GLFW_KEY_U,
  V = GLFW_KEY_V,
  W = GLFW_KEY_W,
  X = GLFW_KEY_X,
  Y = GLFW_KEY_Y,
  Z = GLFW_KEY_Z,

  ESCAPE = GLFW_KEY_ESCAPE,
  ENTER = GLFW_KEY_ENTER,
  TAB = GLFW_KEY_TAB,
  BACKSPACE = GLFW_KEY_BACKSPACE,
  RIGHT = GLFW_KEY_RIGHT,
  LEFT = GLFW_KEY_LEFT,
  DOWN = GLFW_KEY_DOWN,
  UP = GLFW_KEY_UP,
  CAPS_LOCK = GLFW_KEY_CAPS_LOCK,

  LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
  LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
  LEFT_ALT = GLFW_KEY_LEFT_ALT,
  LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
  RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
  RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
  RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
  RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
};

enum class MouseButton {
  LEFT = GLFW_MOUSE_BUTTON_LEFT,
  RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
  MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
  BUTTON_4 = GLFW_MOUSE_BUTTON_4,
  BUTTON_5 = GLFW_MOUSE_BUTTON_5,
  BUTTON_6 = GLFW_MOUSE_BUTTON_6,
  BUTTON_7 = GLFW_MOUSE_BUTTON_7,
  BUTTON_8 = GLFW_MOUSE_BUTTON_8
};

class Input {
public:
  explicit Input(GLFWwindow* window);

  void Update();

  bool IsKeyDown(Key key) const;
  bool IsKeyUp(Key key) const;
  bool IsMouseButtonDown(MouseButton button) const;
  bool IsMouseButtonUp(MouseButton button) const;

  bool IsKeyPressed(Key key) const;
  bool IsKeyReleased(Key key) const;
  bool IsMouseButtonPressed(MouseButton button) const;
  bool IsMouseButtonReleased(MouseButton button) const;

  glm::vec2 GetMousePosition() const;

private:
  GLFWwindow* m_window = nullptr;

  std::unordered_map<int, bool> m_currentKeyStates;
  std::unordered_map<int, bool> m_previousKeyStates;

  std::unordered_map<int, bool> m_currentMouseStates;
  std::unordered_map<int, bool> m_previousMouseStates;
};

}
