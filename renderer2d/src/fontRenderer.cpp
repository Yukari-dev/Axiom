#include "fontRenderer.hpp"
#include <iostream>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#include "sdf_font.frag.spv.h"

namespace Axiom {

FontRenderer::FontRenderer(Renderer2D& renderer) : m_renderer(renderer) {
  m_sdfPipeline = m_renderer.LoadFragShader("sdf_font.frag", g_sdf_font_frag, sizeof(g_sdf_font_frag));
}

FontHandle FontRenderer::LoadFont(const std::string& fontPath, float fontSize) {
  auto it = m_fontLookup.find(fontPath);
  if (it != m_fontLookup.end()) {
    return FontHandle{ it->second };
  }

  std::string fullPath = fontPath;
  std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open font file: " + fullPath);
  }

  size_t fileSize = file.tellg();
  std::vector<uint8_t> fontBuffer(fileSize);
  file.seekg(0);
  file.read(reinterpret_cast<char*>(fontBuffer.data()), fileSize);
  file.close();

  stbtt_fontinfo fontInfo;
  if (!stbtt_InitFont(&fontInfo, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
    throw std::runtime_error("Failed to initialize font info: " + fullPath);
  }

  constexpr uint32_t atlasWidth = 1024;
  constexpr uint32_t atlasHeight = 1024;
  std::vector<uint8_t> atlasBitmap(atlasWidth * atlasHeight, 0);

  Font font{};
  font.lineHeight = fontSize;

  float fontScale = stbtt_ScaleForPixelHeight(&fontInfo, fontSize);

  int padding = 5;
  uint8_t onedgeValue = 128;
  float pixelDistScale = 32.0f;

  uint32_t currentX = padding;
  uint32_t currentY = padding;
  uint32_t rowHeight = 0;

  for (char c = 32; c < 127; ++c) {
    int advance, lsb;
    stbtt_GetCodepointHMetrics(&fontInfo, c, &advance, &lsb);

    int width = 0, height = 0, xoff = 0, yoff = 0;
    unsigned char* sdf = stbtt_GetCodepointSDF(
      &fontInfo,
      fontScale,
      c,
      padding,
      onedgeValue,
      pixelDistScale,
      &width, &height,
      &xoff, &yoff
    );

    if (currentX + width + padding >= atlasWidth) {
      currentX = padding;
      currentY += rowHeight + padding;
      rowHeight = 0;
    }

    if (sdf && width > 0 && height > 0) {
      for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
          atlasBitmap[(currentY + row) * atlasWidth + (currentX + col)] = sdf[row * width + col];
        }
      }
    }

    Glyph g{};
    g.size = glm::vec2(width, height);
    g.bearing = glm::vec2(xoff, -yoff);
    g.advance = advance * fontScale;

    g.uvMin = glm::vec2((float)currentX / atlasWidth, (float)currentY / atlasHeight);
    g.uvMax = glm::vec2((float)(currentX + width) / atlasWidth, (float)(currentY + height) / atlasHeight);

    font.glyphs[c] = g;

    if (sdf) {
      stbtt_FreeSDF(sdf, nullptr);
    }

    currentX += width + padding;
    rowHeight = std::max(rowHeight, static_cast<uint32_t>(height));
  }

  font.atlasTexture = m_renderer.CreateTextureFromData(atlasBitmap.data(), atlasWidth, atlasHeight);

  uint32_t newId = static_cast<uint32_t>(m_fonts.size());
  m_fonts.push_back(std::move(font));
  m_fontLookup[fontPath] = newId;

  return FontHandle{ newId };
}

void FontRenderer::DrawText(
  const std::string& text, 
  glm::vec2 position, 
  FontHandle fontHandle, 
  glm::vec3 color, 
  float scale
) {
  Font& font = GetFontFromHandle(fontHandle);
  glm::vec2 cursor = position;

  for (char c : text) {
    if (c == '\n') {
      cursor.x = position.x;
      cursor.y += font.lineHeight * scale;
      continue;
    }

    auto it = font.glyphs.find(c);
    if (it == font.glyphs.end()) continue;

    const Glyph& glyph = it->second;

    glm::vec2 charPos = {
      cursor.x + glyph.bearing.x * scale,
      cursor.y - glyph.bearing.y * scale
    };
    glm::vec2 charSize = glyph.size * scale;

    m_renderer.DrawRect(
      Rectangle{ charPos.x, charPos.y, charSize.x, charSize.y },
      glyph.uvMin,
      glyph.uvMax,
      color,
      font.atlasTexture,
      m_sdfPipeline
    );

    cursor.x += glyph.advance * scale;
  }
}

glm::vec2 FontRenderer::MeasureText(const std::string& text, FontHandle fontHandle, float scale) {
  Font& font = GetFontFromHandle(fontHandle);
  glm::vec2 size{0.0f, font.lineHeight * scale};
  float currentLineWidth = 0.0f;

  for (char c : text) {
    if (c == '\n') {
      size.x = std::max(size.x, currentLineWidth);
      currentLineWidth = 0.0f;
      size.y += font.lineHeight * scale;
      continue;
    }

    auto it = font.glyphs.find(c);
    if (it != font.glyphs.end()) {
      currentLineWidth += it->second.advance * scale;
    }
  }

  size.x = std::max(size.x, currentLineWidth);
  return size;
}

Font& FontRenderer::GetFontFromHandle(FontHandle handle) {
  if (handle.index < m_fonts.size()) {
    return m_fonts[handle.index];
  }
  return m_defaultFont;
}

}
