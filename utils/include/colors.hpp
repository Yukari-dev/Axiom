#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

namespace Axiom{

inline glm::vec3 HexToRgb(uint32_t hex){
  float red = static_cast<float>((hex >> 16) & 0xFF);
  float green = static_cast<float>((hex >> 8) & 0xFF);
  float blue = static_cast<float>(hex & 0xFF);

  return {red / 255, green / 255, blue / 255};
}

inline glm::vec3 HexToRgb(std::string hex){
  if (hex.rfind("#", 0) == 0)
    hex = hex.substr(1);
  else if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0)
    hex = hex.substr(2);

  uint32_t value = std::stoul(hex, nullptr, 16);

  return HexToRgb(value);
}

}
