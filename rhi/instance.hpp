#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom {

class Instance{
public:
  Instance();
  ~Instance();

private:
  void Create();
  void SetupDebugMessenger();

  bool CheckValidationLayerSupport();
  std::vector<const char *> GetRequiredExtensions();
private:
  VkInstance m_instance;
  VkDebugUtilsMessengerEXT m_debugMessenger;
  std::vector<const char *> m_validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };
  #ifdef DEBUG
    const bool m_enableValidationLayers = false;
  #else
    const bool m_enableValidationLayers = true;
  #endif
};

}
