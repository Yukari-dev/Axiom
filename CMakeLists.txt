cmake_minimum_required(VERSION 3.20)
project(Axiom VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(GLSLC glslc REQUIRED)

file(GLOB SHADER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/meshes/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/meshes/*.frag"
)

foreach(SHADER ${SHADER_SOURCES})
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
    DEPENDS ${SHADER}
    COMMENT "Compiling shader ${SHADER_NAME} to SPIR-V"
  )
  list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach()

add_custom_target(axiom_shaders DEPENDS ${SPIRV_BINARIES})

add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(renderer2d)

add_dependencies(axiom_rhi axiom_shaders)
add_dependencies(axiom_renderer2d axiom_shaders)

target_compile_definitions(axiom_rhi PRIVATE
  AXIOM_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
  AXIOM_TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/textures/"
)

add_library(axiom STATIC 
  include/axiom.h
)

target_link_libraries(axiom PUBLIC 
  axiom_platform 
  axiom_rhi 
  axiom_renderer2d
)

target_include_directories(axiom PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/platform>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/renderer2d/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rhi/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/platform>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/renderer2d>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/rhi>
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

install(DIRECTORY shaders/ DESTINATION ${CMAKE_INSTALL_DATADIR}/axiom/shaders FILES_MATCHING PATTERN "*.spv")
