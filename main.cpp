#include "fontRenderer.hpp"
#include "rhiContext.hpp"
#include "renderer2d.hpp"
#include "window.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(1200, 600, "Axiom");
  Axiom::RhiContext rhi(window.GetHandler(), 1200, 600);
  Axiom::Renderer2D renderer(rhi);

  Axiom::FontRenderer fontRenderer(renderer);
  Axiom::FontHandle font = fontRenderer.LoadFont("fonts/JetBrainsMono-Regular.ttf", 64);
  Axiom::FontHandle font2 = fontRenderer.LoadFont("fonts/BetaniaPatmos-Regular.ttf", 64);
  

  while (!window.ShouldClose()){
    window.PollEvents();
    rhi.BeginFrame();
    renderer.Begin();
    fontRenderer.DrawText("Hello world", {130.0f, 400.0f}, font2, {1.0f, 1.0f, 1.0f}, 1);
    fontRenderer.DrawText("Axiom", {130.0f, 140.0f}, font, {1.0f, 1.0f, 1.0f}, 1);
    // renderer.DrawRect({0.0f, 0.0f}, {1200.0f, 600.0f}, {1.0f, 1.0f, 1.0f});
    // renderer.DrawRect({350.0f, 100.0f}, {500.0f, 400.0f}, {1.0f, 1.0f, 1.0f}, handle);
    // renderer.DrawSprite({350.0f, 100.0f}, {500.0f, 400.0f}, {1.0f, 1.0f, 1.0f});
    // renderer.DrawRoundedRect({500.0f, 225.0f}, {200.0f, 150.0f}, 25.0f, {1.0f, 1.0f, 0.0f});
    // renderer.DrawLine({0.0f, 0.0f}, {1200.0f, 600.0f}, 10, {1.0f, 1.0f, 1.0f});
    renderer.End();
    rhi.EndFrame();
  }

  Axiom::Shutdown();
  return 0;
}
