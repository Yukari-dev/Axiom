#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;

layout(push_constant) uniform PushConstants {
  mat4 projection;
  vec2 rectSize;
  float radius;
} push;

layout(location = 0) out vec4 outColor;

float sdRoundedBox(vec2 p, vec2 b, float r) {
  vec2 d = abs(p) - b + vec2(r);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
}

void main() {
  vec2 halfSize = push.rectSize * 0.5;
  vec2 localPos = (fragUV - 0.5) * push.rectSize;

  float dist = sdRoundedBox(localPos, halfSize, push.radius);
  float edgeSmoothing = fwidth(dist);
  float alpha = 1.0 - smoothstep(-edgeSmoothing, 0.0, dist);

  if (alpha < 0.01) discard;
  outColor = vec4(fragColor, alpha);
}
