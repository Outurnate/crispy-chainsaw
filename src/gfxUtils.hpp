#ifndef GFXUTILS_HPP
#define GFXUTILS_HPP

#include <bgfx/bgfx.h>
#include <glm/vec2.hpp>
#include <math.h>

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

struct positionTextureVertex
{
  float m_x;
  float m_y;
  float m_z;
  float m_u;
  float m_v;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  }

  static bgfx::VertexDecl msLayout;
};

struct positionColorTextureVertex
{
  float m_x;
  float m_y;
  float m_z;
  uint32_t abgr;
  float m_u;
  float m_v;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  }

  static bgfx::VertexDecl msLayout;
};

inline glm::vec2 polarToRect(float radius, float theta)
{
  return { radius * cos(theta), radius * sin(theta) };
}

void init();

template<typename T>
void deleteHandle(T obj)
{
  if (bgfx::isValid(obj))
    bgfx::destroy(obj);
}

#endif
