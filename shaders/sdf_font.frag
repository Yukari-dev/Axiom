#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 aCol;
layout(location = 1) in vec2 aUV;

layout(location = 0) out vec4 outColor;

void main() {
  float dist = texture(texSampler, aUV).r;
  float smoothing = fwidth(dist) * 0.85;
  float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
  outColor = vec4(aCol, alpha);
}
