#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom {

class Instance{
public:
  Instance();
  ~Instance();

  VkInstance GetInstance() const { return m_instance; }
  const std::vector<const char *>& GetValidationLayers() const { return m_validationLayers; }
  bool IsValidationEnabled() const { return m_enableValidationLayers; }
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
  #ifdef NDEBUG
    const bool m_enableValidationLayers = false;
  #else
    const bool m_enableValidationLayers = true;
  #endif
};

}
