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

  void update(double delta, float width, float height) override;
  void updateAudio(const fftSpectrumData& audioFrame) override;
  void onReset(uint32_t width, uint32_t height) override;
private:
  unsigned points;
  float baseRadius;
  float circleWidth;
  std::vector<positionColorVertex> vertexBuffer;
  std::vector<uint16_t> indexBuffer;
  bgfx::ProgramHandle program;
  bgfx::DynamicVertexBufferHandle circleVBO;
  bgfx::IndexBufferHandle circleEBO;
};

#endif
