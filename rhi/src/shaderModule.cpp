#include "shaderModule.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Axiom{

ShaderModule::ShaderModule(const std::string& shaderPath, VkDevice device) : m_device(device){
  Load(shaderPath);
}

ShaderModule::~ShaderModule(){
  vkDestroyShaderModule(m_device, m_module, nullptr);
}

void ShaderModule::Load(const std::string& shaderPath){
  auto sourceCode = LoadShaderSource(shaderPath);
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = sourceCode.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(sourceCode.data());
  VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, &m_module);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Shader Module.");
}

std::vector<char> ShaderModule::LoadShaderSource(const std::string& shaderPath){
  std::ifstream shader(shaderPath, std::ios::ate | std::ios::binary);

  if(!shader.is_open())
    throw std::runtime_error("Failed to open shader: " + shaderPath);

  size_t shaderSize = (size_t)shader.tellg();
  std::vector<char> buffer(shaderSize);
  shader.seekg(0);
  shader.read(buffer.data(), shaderSize);
  shader.close();
  return buffer;
}

}
