#include "rhiContext.hpp"
#include "window.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::RhiContext rhi(window.GetHandler(), 1200, 600);

  while (!window.ShouldClose()){
    window.PollEvents();
    rhi.DrawFrame();
  }

  Axiom::Shutdown();
  return 0;
}
