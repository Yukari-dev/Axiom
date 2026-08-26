add_library(axiom_rhi STATIC instance.cpp device.cpp surface.cpp)
target_include_directories(axiom_rhi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(axiom_rhi PUBLIC vulkan)
