#include "window.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(800, 600, "Axiom");

  while (!window.ShouldClose()){
    window.PollEvents();
  }

  Axiom::Shutdown();
  return 0;
}
