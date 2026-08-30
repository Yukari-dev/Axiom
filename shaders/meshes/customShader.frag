#version 450 

layout(location = 0) in vec3 aColor;
layout(location = 1) in float aIntensity;
layout(location = 2) in vec2 aUV;

layout(location = 0) out vec4 outColor;

void main(){
  vec2 centerOffset = aUV - vec2(0.5);
  float dist = length(centerOffset);
  float sigma = 0.25;
  float blur = exp(-(dist * dist) * aIntensity) / (2.0f * sigma * sigma);
  float edgeMask = smoothstep(0.5, 0.1, dist);
  float finalAlpha = blur * edgeMask;
  outColor = vec4(aColor, finalAlpha);
}
