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

find_program(GLSLC glslc REQUIRED)
set(
  SHADER_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag
)

foreach(SHADER ${SHADER_SOURCES})
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  set(SPIRV ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv)
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
    DEPENDS ${SHADER}
  )
  list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

add_custom_target(axiom_shaders DEPENDS ${SPIRV_BINARIES})
add_dependencies(axiom_rhi axiom_shaders)
target_compile_definitions(
  axiom_rhi PRIVATE
  AXIOM_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
  AXIOM_TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/textures/"
)
