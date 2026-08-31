#pragma once
#include "window.hpp"
#include "input.hpp"
#include "rhiContext.hpp"
#include "renderer2d.hpp"
#include "fontRenderer.hpp"

namespace Axiom{

struct ContextGuard{
  ContextGuard() { Init(); }
  ~ContextGuard() { Shutdown(); }
};

}
