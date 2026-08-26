#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "surface.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(800, 600, "Axiom");

  Axiom::Instance instance;
  Axiom::Surface surface(instance.GetInstance(), window.GetHandler());
  Axiom::Device device(instance, surface.GetSurface());

  while (!window.ShouldClose()){
    window.PollEvents();
  }

  Axiom::Shutdown();
  return 0;
}
