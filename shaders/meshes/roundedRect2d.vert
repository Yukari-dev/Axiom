#version 450

layout(binding = 0) uniform UBO { mat4 projection; } ubo;

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inLocalPos;
layout(location = 3) in vec2 inRectSize;
layout(location = 4) in float inRadius;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 localPos;
layout(location = 2) out vec2 rectSize;
layout(location = 3) out float radius;

void main() {
  gl_Position = ubo.projection * vec4(inPos, 0.0, 1.0);
  fragColor = inColor;
  localPos = inLocalPos;
  rectSize = inRectSize;
  radius = inRadius;
}
