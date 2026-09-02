add_library(axiom_platform STATIC src/window.cpp src/input.cpp)
target_include_directories(axiom_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(axiom_platform PUBLIC glfw)
