#include "window.hpp"
#include "instance.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(800, 600, "Axiom");

  Axiom::Instance instance;

  while (!window.ShouldClose()){
    window.PollEvents();
  }

  Axiom::Shutdown();
  return 0;
}
