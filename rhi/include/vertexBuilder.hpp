#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

namespace Axiom{

class VertexBuilder{
public:
  void PushFloat(float v){
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&v);
    m_data.insert(m_data.end(), bytes, bytes + sizeof(float));
  }

  void PushFloat2(float x, float y){
    PushFloat(x);
    PushFloat(y);
  }

  void PushFloat3(float x, float y, float z){
    PushFloat(x);
    PushFloat(y);
    PushFloat(z);
  }

  void PushVec2(glm::vec2 v){
    PushFloat(v.x);
    PushFloat(v.y);
  }

  void PushVec3(glm::vec3 v){
    PushFloat(v.x);
    PushFloat(v.y);
    PushFloat(v.z);
  }

  const void* Data() const { return m_data.data(); }
  size_t Size() const { return m_data.size(); }
  void Clear() { m_data.clear(); }
private:
  std::vector<uint8_t> m_data;
};

}
