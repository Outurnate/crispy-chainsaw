#ifndef WARPSCENE_HPP
#define WARPSCENE_HPP

#include "sceneManager.hpp"

class warpScene : public scene
{
public:
  warpScene(resourceManager& resources);
  virtual ~warpScene();

  void update(double delta, float width, float height) override;
  void updateAudio(const audioAnalyzedFrame& audioFrame) override;
  void onReset(uint32_t width, uint32_t height) override;
private:
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle testVBO;
  bgfx::IndexBufferHandle testEBO;
  bgfx::TextureHandle spotTexture;
  bgfx::UniformHandle s_texColor;
};

#endif
