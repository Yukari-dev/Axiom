#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 aCol;
layout(location = 1) in vec2 aUV;

layout(location = 0) out vec4 outColor;

void main(){
  vec4 texColor = texture(texSampler, aUV);

  if(texColor.a < 0.1) discard;
  outColor = vec4(texColor.rgb * aCol, 1.0);
}
