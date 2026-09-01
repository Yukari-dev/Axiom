#include <axiom.h>

int main(int ac, char **av) {
  Axiom::ContextGuard axiom;

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::Input input(window.GetHandler());
  Axiom::RhiContext rhi(window.GetHandler(), 1200, 600);
  Axiom::Renderer2D renderer(rhi);

  while (!window.ShouldClose()){
    window.PollEvents();
    input.Update();

    if(input.IsKeyReleased(Axiom::Key::ESCAPE))
      window.CloseWindow();

    rhi.BeginFrame();
    renderer.Begin();
      
    glm::vec3 color = {1, 1, 1};
    if(input.IsMouseButtonDown(Axiom::MouseButton::LEFT))
      color = {0, 0, 0};
    renderer.DrawRect({0, 0}, {1200, 600}, color);
    renderer.End();
    rhi.EndFrame();
  }

  rhi.WaitIdle();

  return 0;
}
