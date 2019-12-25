#include "gfxUtils.hpp"

bgfx::VertexLayout positionColorVertex::msLayout;
bgfx::VertexLayout positionTextureVertex::msLayout;
bgfx::VertexLayout positionColorNormalVertex::msLayout;
bgfx::VertexLayout positionColorTextureVertex::msLayout;

void init()
{
  positionColorVertex::init();
  positionTextureVertex::init();
  positionColorNormalVertex::init();
  positionColorTextureVertex::init();
}
