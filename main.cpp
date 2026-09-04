#include <axiom.h>

int main(int ac, char **av) {
  Axiom::ContextGuard axiom;

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::Input input(window.GetHandler());
  Axiom::RhiContext rhi(window.GetHandler(), window.GetWidth(), window.GetHeight());
  Axiom::Renderer2D renderer(rhi);
  renderer.SetDefaultFont("fonts/JetBrainsMono-Regular.ttf", 64.0f);

  while (!window.ShouldClose()){
    window.PollEvents();
    input.Update();

    if(input.IsKeyReleased(Axiom::Key::ESCAPE))
      window.CloseWindow();

    rhi.BeginFrame();
    renderer.Begin();
      
    glm::vec3 color = {0, 0, 0};
    glm::vec3 textColor = {1, 1, 1};
    if(input.IsMouseButtonDown(Axiom::MouseButton::LEFT)){
      color = {1, 1, 1};
      textColor = {0, 0, 0};
    }
    renderer.DrawRect({0, 0}, {1200, 600}, color);
    
    glm::vec2 textSize = renderer.MeasureText("Hello world", 3);
    renderer.DrawText("Hello world", {600 - (textSize.x / 2), 300 - (textSize.y / 2)}, textColor, 3);
    renderer.End();
    rhi.EndFrame();
  }

  rhi.WaitIdle();

  return 0;
}
