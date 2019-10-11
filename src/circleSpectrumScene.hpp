#ifndef CIRCLESPECTRUMSCENE_HPP
#define CIRCLESPECTRUMSCENE_HPP

#include <bgfx/bgfx.h>
#include "sceneManager.hpp"

class circleSpectrumScene : public scene
{
public:
  circleSpectrumScene();
  virtual ~circleSpectrumScene();

  void update(double delta, float width, float height) override;
private:
  bgfx::ProgramHandle mProgram;
  bgfx::VertexBufferHandle mVbh;
  bgfx::IndexBufferHandle mIbh;
  float mTime;
};

#endif
