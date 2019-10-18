#include "gfxUtils.hpp"

bgfx::VertexDecl positionColorVertex::msLayout;
bgfx::VertexDecl positionTextureVertex::msLayout;
bgfx::VertexDecl positionColorTextureVertex::msLayout;

void init()
{
  positionColorVertex::init();
  positionTextureVertex::init();
  positionColorTextureVertex::init();
}
