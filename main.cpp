#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "renderPass.hpp"

int main(int ac, char **av) {
  Axiom::Init();

  Axiom::Window window(800, 600, "Axiom");

  Axiom::Instance instance;
  Axiom::Surface surface(instance.GetInstance(), window.GetHandler());
  Axiom::Device device(instance, surface.GetSurface());
  Axiom::SwapChain swapChain(device.GetPhysicalDevice(), device.GetDevice(), surface.GetSurface(), window.GetHandler());
  Axiom::RenderPass renderPass(device.GetDevice(), swapChain.GetFormat());
  Axiom::Pipeline pipeline(device.GetDevice(), swapChain.GetExtent(), renderPass.GetRenderPass());

  while (!window.ShouldClose()){
    window.PollEvents();
  }

  Axiom::Shutdown();
  return 0;
}
