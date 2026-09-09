#pragma once
#include <chrono>

namespace Axiom{

class Clock{
public:
  Clock() = delete;

  static void Tick(){
    auto now = std::chrono::high_resolution_clock::now();
    
    if(!s_initialized){
      s_lastTime = now;
      s_initialized = true;
      s_deltaTime = 0.0f;
      return;
    }
    s_deltaTime = std::chrono::duration<float>(now - s_lastTime).count();
    s_lastTime = now;
  }

  static float GetDeltaTime() { return s_deltaTime; }
private:
  using TimePoint = std::chrono::high_resolution_clock::time_point;
  static inline TimePoint s_lastTime{};
  static inline float s_deltaTime{0.0f};
  static inline float s_initialized{false};
};

}
