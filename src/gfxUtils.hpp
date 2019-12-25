#ifndef GFXUTILS_HPP
#define GFXUTILS_HPP

#include <bgfx/bgfx.h>
#include <glm/vec2.hpp>
#include <math.h>
#include <limits>

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

  static bgfx::VertexLayout msLayout;
};

struct positionTextureVertex
{
  float x;
  float y;
  float z;
  float u;
  float v;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  }

  static bgfx::VertexLayout msLayout;
};

struct positionColorNormalVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;
  float r;
  float s;
  float t;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
      .add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
      .end();
  }

  static bgfx::VertexLayout msLayout;
};

struct positionColorTextureVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;
  float u;
  float v;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  }

  static bgfx::VertexLayout msLayout;
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

class fastRNG
{
public:
  float operator()()
  {
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return float(z) / float(std::numeric_limits<unsigned long>::max());
  }
private:
  unsigned long x = 123456789, y = 362436069, z = 521288629;
};

static fastRNG rng;

#endif
