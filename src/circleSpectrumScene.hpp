#ifndef CIRCLESPECTRUMSCENE_HPP
#define CIRCLESPECTRUMSCENE_HPP

#include "sceneManager.hpp"
#include "gfxUtils.hpp"
#include <vector>

class circleSpectrumScene : public scene
{
public:
  circleSpectrumScene();
  virtual ~circleSpectrumScene();

  void update(const audioAnalyzedFrame& audioFrame, double delta, float width, float height) override;
private:
  void updateBuffers(const audioAnalyzedFrame& audioFrame);

  unsigned points;
  std::vector<positionColorVertex> vertexBuffer;
  std::vector<uint16_t> indexBuffer;
  bgfx::ProgramHandle program;
  bgfx::DynamicVertexBufferHandle circleVBO;
  bgfx::IndexBufferHandle circleEBO;
};

#endif
