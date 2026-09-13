#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 localPos;
layout(location = 2) in vec2 rectSize;
layout(location = 3) in float radius;

layout(location = 0) out vec4 outColor;

float sdRoundedBox(vec2 p, vec2 b, float r) {
  vec2 d = abs(p) - b + vec2(r);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
}

vec3 srgbNonlinearToLinear(vec3 compressed) {
  bvec3 cutoff = lessThan(compressed, vec3(0.04045));
  vec3 higher = pow((compressed + vec3(0.055)) / vec3(1.055), vec3(2.4));
  vec3 lower = compressed / vec3(12.92);
  return mix(higher, lower, cutoff);
}

void main() {
  vec2 halfSize = rectSize * 0.5;

  float dist = sdRoundedBox(localPos, halfSize, radius);
  float edgeSmoothing = fwidth(dist);
  float alpha = 1.0 - smoothstep(-edgeSmoothing, 0.0, dist);

  if (alpha < 0.01) discard;
  vec3 linearColor = srgbNonlinearToLinear(fragColor.rgb);
  outColor = vec4(linearColor, alpha * fragColor.a);
}
