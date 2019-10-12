#ifndef GFXUTILS_HPP
#define GFXUTILS_HPP

#include <bgfx/bgfx.h>
#include <bigg.hpp>
#include <bx/math.h>
#include <bx/string.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>

struct positionColorVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
      .end();
  };

  static bgfx::VertexDecl msLayout;
};

inline glm::vec2 polarToRect(float radius, float theta)
{
  return { radius * cos(theta), radius * sin(theta) };
}

void init();

#endif
