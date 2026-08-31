add_library(axiom_platform STATIC window.cpp input.cpp)
target_include_directories(axiom_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(axiom_platform PUBLIC glfw)
