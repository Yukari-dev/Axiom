#include "rhiContext.hpp"
#include "renderer2d.hpp"
#include "window.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::RhiContext rhi(window.GetHandler(), 1200, 600);
  Axiom::Renderer2D renderer(rhi);

  while (!window.ShouldClose()){
    window.PollEvents();
    rhi.BeginFrame();
    renderer.Begin();
    renderer.DrawRect({100.0f, 100.0f}, {200.0f, 150.0f}, {1.0f, 0.0f, 0.0f});
    renderer.DrawRect({500.0f, 225.0f}, {200.0f, 150.0f}, {1.0f, 1.0f, 0.0f});
    renderer.End();
    rhi.EndFrame();
  }

  Axiom::Shutdown();
  return 0;
}
