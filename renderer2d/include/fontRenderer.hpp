#pragma once
#include "renderer2d.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

namespace Axiom {

struct Glyph {
  glm::vec2 uvMin{0.0f};
  glm::vec2 uvMax{1.0f};
  glm::vec2 size{0.0f};
  glm::vec2 bearing{0.0f};
  float advance{0.0f};
};

struct Font {
  TextureHandle atlasTexture{};
  std::unordered_map<char, Glyph> glyphs;
  float lineHeight{0.0f};
};

struct FontHandle {
  uint32_t index{0};
};

class FontRenderer {
public:
  explicit FontRenderer(Renderer2D& renderer);
  ~FontRenderer() = default;

  FontHandle LoadFont(const std::string& fontPath, float fontSize = 48.0f);

  void DrawText(
    const std::string& text, 
    glm::vec2 position, 
    FontHandle fontHandle, 
    glm::vec3 color = glm::vec3(1.0f), 
    float scale = 1.0f
  );

  glm::vec2 MeasureText(const std::string& text, FontHandle fontHandle, float scale = 1.0f);

private:
  Font& GetFontFromHandle(FontHandle handle);

private:
  Renderer2D& m_renderer;
  PipelineHandle m_sdfPipeline{};
  std::vector<Font> m_fonts;
  std::unordered_map<std::string, uint32_t> m_fontLookup;
  Font m_defaultFont;
};

}
