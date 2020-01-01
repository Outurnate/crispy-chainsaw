#ifndef CIRCLESPECTRUMSCENE_HPP
#define CIRCLESPECTRUMSCENE_HPP

#include "SceneManager.hpp"
#include <vector>

class BlackHoleScene : public Scene
{
public:
  BlackHoleScene();

  const std::string getDisplayName() const override;
  const std::string getName() const override;
  void show() override;
  void hide() override;
  void update(double delta) override;
  void updateAudio(const FFTSpectrumData& audioFrame) override;
private:
  unsigned points;
  float baseRadius;
  float circleWidth;
  /*std::vector<positionColorVertex> vertexBuffer;
  std::vector<uint16_t> indexBuffer;
  bgfx::ProgramHandle program;
  bgfx::DynamicVertexBufferHandle circleVBO;
  bgfx::IndexBufferHandle circleEBO;*/
};

#endif
