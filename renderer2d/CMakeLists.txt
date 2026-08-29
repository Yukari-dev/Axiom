add_library(axiom_renderer2d STATIC
  src/renderer2d.cpp
  src/mesh.cpp
)

target_include_directories(axiom_renderer2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(axiom_renderer2d PUBLIC axiom_rhi)
