#ifndef WARPSCENE_HPP
#define WARPSCENE_HPP

#include "sceneManager.hpp"

class warpScene : public scene
{
public:
  warpScene();
  virtual ~warpScene();

  void update(double delta, float width, float height) override;
  void updateAudio(const fftSpectrumData& audioFrame) override;
  void onReset(uint32_t width, uint32_t height) override;
private:
  struct star
  {
    float radius = 0.0f;
    float theta = 0.0f;
    float length = 1.0f;
    float speed = 0.1f;

    inline glm::vec2 cartesian() const { return polarToRect(radius, theta); }
  };
  void renderStar(const star& obj);
  void renderStaticStar(const glm::vec2& coord);
  static inline void resetStar(star& obj);

  bgfx::ProgramHandle textures;
  bgfx::ProgramHandle colors;
  bgfx::VertexBufferHandle quadVBO;
  bgfx::IndexBufferHandle quadEBO;
  bgfx::VertexBufferHandle cubeVBO;
  bgfx::IndexBufferHandle cubeEBO;
  bgfx::TextureHandle spotTexture;
  bgfx::TextureHandle pewTexture;
  bgfx::UniformHandle s_texColor;
  bgfx::UniformHandle blendColor;
  //std::unordered_map<spectrumRange, float>
  std::array<star, 100> stars;
  std::array<glm::vec2, 500> staticStars;
};

#endif
