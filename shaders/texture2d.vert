#version 450

layout(binding = 0) uniform UBO {
  mat4 projection;
} ubo;

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inCol;
layout(location = 2) in float inAlpha;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 aCol;
layout(location = 1) out vec2 aUV;

void main(){
  gl_Position = ubo.projection * vec4(inPos, 0.0, 1.0);
  aCol = vec4(inCol, inAlpha);
  aUV = inUV;
}
