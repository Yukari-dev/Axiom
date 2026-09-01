cmake_minimum_required(VERSION 3.20)
project(Axiom VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(GLSLC glslc REQUIRED)

set(GEN_SHADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_shaders")
file(MAKE_DIRECTORY ${GEN_SHADER_DIR})

file(GLOB SHADER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/meshes/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/meshes/*.frag"
)

set(EMBEDDED_HEADERS "")

foreach(SHADER ${SHADER_SOURCES})
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  string(MAKE_C_IDENTIFIER "${SHADER_NAME}" SHADER_VAR_NAME)
  set(HEADER_OUTPUT "${GEN_SHADER_DIR}/${SHADER_NAME}.spv.h")

  add_custom_command(
    OUTPUT ${HEADER_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" > ${HEADER_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "#include <cstdint>" >> ${HEADER_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "static const uint32_t g_${SHADER_VAR_NAME}[] = {" >> ${HEADER_OUTPUT}
    COMMAND ${GLSLC} -mfmt=num ${SHADER} -o - >> ${HEADER_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "};" >> ${HEADER_OUTPUT}
    DEPENDS ${SHADER}
    VERBATIM
  )
  list(APPEND EMBEDDED_HEADERS ${HEADER_OUTPUT})
endforeach()

add_custom_target(axiom_shaders DEPENDS ${EMBEDDED_HEADERS})

file(GLOB_RECURSE AXIOM_SOURCES
  "platform/*.cpp"
  "rhi/*.cpp"
  "renderer2d/*.cpp"
)

add_library(axiom STATIC 
  include/axiom.h
  ${AXIOM_SOURCES}
)

add_dependencies(axiom axiom_shaders)

target_compile_definitions(axiom PRIVATE
  AXIOM_TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/textures/"
)

target_include_directories(axiom 
  PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/renderer2d/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rhi/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/platform>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/renderer2d>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/rhi>
  PRIVATE
    ${GEN_SHADER_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer2d/src
    ${CMAKE_CURRENT_SOURCE_DIR}/rhi/src
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(axiom PUBLIC
  Vulkan::Vulkan
  glfw
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
add_executable(axiom_sandbox main.cpp)
target_link_libraries(axiom_sandbox PRIVATE axiom)
endif()

include(GNUInstallDirs)

install(TARGETS axiom
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY platform/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/platform FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
install(DIRECTORY renderer2d/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/renderer2d)
install(DIRECTORY rhi/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rhi)
