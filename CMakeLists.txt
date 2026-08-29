set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.20)
project(Axiom)

add_subdirectory(platform)
add_subdirectory(rhi)
add_subdirectory(renderer2d)

add_executable(Axiom main.cpp)
target_link_libraries(Axiom PRIVATE axiom_platform axiom_rhi axiom_renderer2d)
