set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.20)
project(Axiom)

add_subdirectory(platform)

add_executable(Axiom main.cpp)
target_link_libraries(Axiom PUBLIC axiom_platform)
