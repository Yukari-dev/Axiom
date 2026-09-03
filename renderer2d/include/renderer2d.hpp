#pragma once
#include <cstdint>
#include <memory>
#include <string>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Axiom {

class RhiContext;
struct FontHandle;

struct Rectangle {
  float x, y, width, height;
};

struct PipelineHandle {
  uint32_t index{0};
};

struct TextureHandle {
  uint32_t index{0};
};

class Renderer2D {
public:
  explicit Renderer2D(RhiContext& rhi);
  ~Renderer2D();

  Renderer2D(const Renderer2D&) = delete;
  Renderer2D& operator=(const Renderer2D&) = delete;
  Renderer2D(Renderer2D&&) noexcept;
  Renderer2D& operator=(Renderer2D&&) noexcept;

  void Begin();

  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
  void DrawRect(Rectangle rec, glm::vec3 color);
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, PipelineHandle shaderHandle);
  void DrawRect(Rectangle rec, glm::vec3 color, PipelineHandle shaderHandle);
  void DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, TextureHandle textureHandle);
  void DrawRect(Rectangle rec, glm::vec3 color, TextureHandle textureHandle);
  void DrawRect(Rectangle rec, glm::vec2 uvMin, glm::vec2 uvMax, glm::vec3 color, TextureHandle textureHandle);
  void DrawRect(Rectangle rec, glm::vec2 uvMin, glm::vec2 uvMax, glm::vec3 color, TextureHandle textureHandle, PipelineHandle pipelineHandle);
  void DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec3 tint);
  void DrawSprite(Rectangle rec, glm::vec3 tint);

  void DrawRoundedRect(glm::vec2 pos, glm::vec2 size, float roundness, glm::vec3 color);
  void DrawRoundedRect(Rectangle rec, float roundness, glm::vec3 color);

  void DrawLine(glm::vec2 from, glm::vec2 to, glm::vec3 color);
  void DrawLine(glm::vec2 from, glm::vec2 to, float thick, glm::vec3 color);

  void DrawText(
    const std::string& text, glm::vec2 position, FontHandle fontHandle, 
    glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f
  );

  void DrawText(
    const std::string& text, glm::vec2 position, 
    glm::vec3 color = glm::vec3(1.0f), float scale = 1.0f
  );

  glm::vec2 MeasureText(const std::string& text, FontHandle fontHandle, float scale = 1.0f);
  glm::vec2 MeasureText(const std::string& text, float scale = 1.0f);

  void End();

  PipelineHandle LoadFragShader(const std::string& shaderName);
  PipelineHandle LoadFragShader(const std::string& shaderName, const uint32_t *code, size_t size);
  TextureHandle LoadTexture(const std::string& textureName);
  TextureHandle CreateTextureFromData(const uint8_t* pixels, uint32_t width, uint32_t height);
  FontHandle SetDefaultFont(const std::string& name, float fontSize = 48.f);
private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

}
