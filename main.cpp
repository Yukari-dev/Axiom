#include <axiom.h>
#include <iostream>
#include <string>

int main(int ac, char **av) {
  Axiom::ContextGuard axiom;

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::Input input(window.GetHandler());
  Axiom::RhiContext rhi(window.GetHandler(), window.GetWidth(), window.GetHeight());
  Axiom::Renderer2D renderer(rhi);
  float fontSize{64.0f};
  renderer.SetDefaultFont("fonts/JetBrainsMono-Regular.ttf", fontSize);

  float maxFps = 0.0f;
  while (!window.ShouldClose()){
    Axiom::Clock::Tick();
    window.PollEvents();
    input.Update();

    if(input.IsKeyReleased(Axiom::Key::ESCAPE))
      window.CloseWindow();

    rhi.BeginFrame();
    renderer.Begin();
      
    glm::vec3 color{0};
    glm::vec3 textColor{Axiom::HexToRgb("#FFFFFF")};
    if(input.IsMouseButtonDown(Axiom::MouseButton::LEFT)){
      color = {1, 1, 1};
      textColor = Axiom::HexToRgb("0x13F1AC");
    }
    renderer.DrawRect({0, 0}, {1200, 600}, color);
  
    if(input.IsKeyReleased(Axiom::Key::A)){
      fontSize -= 12.0f;
      renderer.ChangeDefautlFont(fontSize);
    }
    if(input.IsKeyReleased(Axiom::Key::S)){
      fontSize += 12.0f;
      renderer.ChangeDefautlFont(fontSize);
    }

    float dt = Axiom::Clock::GetDeltaTime();
    float currentFps = (dt > 0.00001f) ? (1.0f / dt) : 0.0f;
    std::string text = std::to_string(currentFps) + "fps";
    glm::vec2 textSize = renderer.MeasureText(text, 1);
    renderer.DrawText(text, {600 - (textSize.x / 2), 300 - (textSize.y / 2)}, textColor, 1);
    if(currentFps > maxFps)
      maxFps = currentFps;
  
    std::string maxFpsText = "MAX FPS: " + std::to_string(maxFps) + "fps";
    glm::vec2 fpsTextSize = renderer.MeasureText(maxFpsText, 0.4f);
    renderer.DrawText(maxFpsText, {600 - (fpsTextSize.x / 2), 350}, textColor, 0.4f);

    renderer.End();
    rhi.EndFrame();
  }

  rhi.WaitIdle();

  return 0;
}
