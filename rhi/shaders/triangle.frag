#version 450

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 aColor;
layout(location = 1) in vec2 aTexCoord;

void main(){
  outColor = texture(texSampler, aTexCoord);
}
