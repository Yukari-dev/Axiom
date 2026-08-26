#include "instance.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Axiom{

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  void *pUserData
){
  std::cerr << "Validation layer: " << pCallbackData->pMessage << '\n';
  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkDebugUtilsMessengerEXT *pDebugMessenger
){
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if(func != nullptr)
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(
  VkInstance instance, 
  VkDebugUtilsMessengerEXT debugMessnger,
  const VkAllocationCallbacks *pAllocator
){
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
    func(instance, debugMessnger, pAllocator);
}

void PopulateDebugMessengerDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = 
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = 
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
}

Instance::Instance(){
  Create();
  SetupDebugMessenger();
}

Instance::~Instance(){
  if (m_enableValidationLayers)
    DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
  vkDestroyInstance(m_instance, nullptr);
}

void Instance::Create(){
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Axiom";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  
  auto extensions = GetRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if(m_enableValidationLayers && !CheckValidationLayerSupport())
    throw std::runtime_error("validation layers requested, but not available.");

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if(m_enableValidationLayers){
    createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
    createInfo.ppEnabledLayerNames = m_validationLayers.data();
    PopulateDebugMessengerDebugCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  } else{
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
  if (result != VK_SUCCESS)
    throw std::runtime_error("Failed to create Instance.");

  std::cout << "Instance is created!.\n";
}

void Instance::SetupDebugMessenger(){
  if (!m_enableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  PopulateDebugMessengerDebugCreateInfo(createInfo);

  VkResult result = CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to setup debug messenger.");
}

bool Instance::CheckValidationLayerSupport(){
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : m_validationLayers){
    bool found = false;
    for(const auto& availableLayer : availableLayers){
      if(strcmp(layerName, availableLayer.layerName) == 0){
        found = true;
        break;
      }
    }
    if(!found)
      return false;
  }
    
  return true;
}

std::vector<const char *> Instance::GetRequiredExtensions(){
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  if(m_enableValidationLayers)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  return extensions;
}

}
