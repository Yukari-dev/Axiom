add_library(
  axiom_rhi STATIC 
  src/rhiContext.cpp
  src/instance.cpp
  src/device.cpp
  src/surface.cpp
  src/swapchain.cpp
  src/shaderModule.cpp
  src/pipeline.cpp
  src/renderPass.cpp
  src/framebuffer.cpp
  src/commandPool.cpp
  src/commandBuffer.cpp
  src/syncObjects.cpp
  src/buffer.cpp
  src/descriptorSetLayout.cpp
  src/descriptorSet.cpp
  src/texture.cpp
  src/stb_image.cpp
)
find_path(STB_INCLUDE_DIR stb_image.h)
if(STB_INCLUDE_DIR)
  target_include_directories(axiom_rhi PRIVATE ${STB_INCLUDE_DIR})
endif()
target_include_directories(axiom_rhi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(axiom_rhi PUBLIC Vulkan::Vulkan glm::glm)

