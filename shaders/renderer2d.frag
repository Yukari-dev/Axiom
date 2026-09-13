#version 450 

layout(location = 0) in vec4 aColor;
layout(location = 0) out vec4 outColor;

vec3 srgbNonlinearToLinear(vec3 compressed) {
  bvec3 cutoff = lessThan(compressed, vec3(0.04045));
  vec3 higher = pow((compressed + vec3(0.055)) / vec3(1.055), vec3(2.4));
  vec3 lower = compressed / vec3(12.92);
  return mix(higher, lower, cutoff);
}

void main(){
  vec3 linearColor = srgbNonlinearToLinear(aColor.rgb);
  outColor = vec4(linearColor, aColor.a);
}
