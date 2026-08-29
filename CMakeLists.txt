set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.20)
project(Axiom)

add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(renderer2d)

find_program(GLSLC glslc REQUIRED)

file(GLOB SHADER_SOURCES
  ${CMAKE_SOURCE_DIR}/shaders/*.vert
  ${CMAKE_SOURCE_DIR}/shaders/*.frag
)

foreach(SHADER ${SHADER_SOURCES})
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  set(SPIRV ${CMAKE_SOURCE_DIR}/shaders/${SHADER_NAME}.spv)
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
    DEPENDS ${SHADER}
  )
  list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

add_custom_target(axiom_shaders DEPENDS ${SPIRV_BINARIES})
add_dependencies(axiom_rhi axiom_shaders)
add_dependencies(axiom_renderer2d axiom_shaders)

target_compile_definitions(axiom_rhi PRIVATE
    AXIOM_SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/"
    AXIOM_TEXTURE_DIR="${CMAKE_SOURCE_DIR}/textures/"
)
target_compile_definitions(axiom_renderer2d PRIVATE
    AXIOM_SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/"
)

add_executable(Axiom main.cpp)
target_link_libraries(Axiom PRIVATE axiom_platform axiom_rhi axiom_renderer2d)
