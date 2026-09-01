#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Axiom {

class ShaderModule {
public:
  ShaderModule(const std::string &shaderPath, VkDevice device);
  ShaderModule(const uint32_t* code, size_t sizeInBytes, VkDevice device);
  ~ShaderModule();

  VkShaderModule GetShaderModule() const { return m_module; }
private:
  void Load(const std::string& shaderPath);
  void CreateFromData(const uint32_t *code, size_t size);
  std::vector<uint32_t> LoadShaderSource(const std::string& shaderPath);
private:
  VkShaderModule m_module;
  VkDevice m_device;
};

}
