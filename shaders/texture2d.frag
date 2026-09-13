#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 aCol;
layout(location = 1) in vec2 aUV;

layout(location = 0) out vec4 outColor;

vec3 srgbNonlinearToLinear(vec3 compressed) {
  bvec3 cutoff = lessThan(compressed, vec3(0.04045));
  vec3 higher = pow((compressed + vec3(0.055)) / vec3(1.055), vec3(2.4));
  vec3 lower = compressed / vec3(12.92);
  return mix(higher, lower, cutoff);
}

void main(){
  vec4 texColor = texture(texSampler, aUV);

  vec3 linearTextureColor = srgbNonlinearToLinear(texColor.rgb);
  vec3 linearTintColor = srgbNonlinearToLinear(aCol.rgb);
  outColor = vec4(linearTextureColor * linearTintColor, texColor.a * aCol.a);
}
