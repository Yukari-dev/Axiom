#include "shaderModule.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Axiom{

ShaderModule::ShaderModule(const std::string& shaderPath, VkDevice device) : m_device(device){
  Load(shaderPath);
}

ShaderModule::ShaderModule(const uint32_t* code, size_t sizeInBytes, VkDevice device) : m_device(device){
  CreateFromData(code, sizeInBytes);
}

ShaderModule::~ShaderModule(){
  vkDestroyShaderModule(m_device, m_module, nullptr);
}

void ShaderModule::Load(const std::string& shaderPath){
  auto sourceCode = LoadShaderSource(shaderPath);
  CreateFromData(sourceCode.data(), sourceCode.size() * sizeof(uint32_t));
}

void ShaderModule::CreateFromData(const uint32_t *code, size_t size){
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size;
  createInfo.pCode = code;

  if(vkCreateShaderModule(m_device, &createInfo, nullptr, &m_module) != VK_SUCCESS)
    throw std::runtime_error("Failed to create Shader Module.");
}

std::vector<uint32_t> ShaderModule::LoadShaderSource(const std::string& shaderPath) {
  std::ifstream shader(shaderPath, std::ios::ate | std::ios::binary);

  if (!shader.is_open()) {
    throw std::runtime_error("Failed to open shader: " + shaderPath);
  }

  size_t fileSize = static_cast<size_t>(shader.tellg());

  if (fileSize % sizeof(uint32_t) != 0) {
    throw std::runtime_error("Shader file size is not correct for SPRIV: " + shaderPath);
  }

  std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
  shader.seekg(0);
  shader.read(reinterpret_cast<char*>(buffer.data()), fileSize);
  shader.close();

  return buffer;
}

}
