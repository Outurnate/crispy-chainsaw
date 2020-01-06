#ifndef CIRCLESPECTRUMSCENE_HPP
#define CIRCLESPECTRUMSCENE_HPP

#include "SceneManager.hpp"
#include <vector>

class BlackHoleScene : public Scene
{
public:
  BlackHoleScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet);

  void update(double delta) override;
  void updateAudio(const FFTSpectrumData& audioFrame) override;
  void initialize() override;
  Ogre::Camera& getCamera() override;
private:
  Ogre::Camera* camera;
  unsigned points;
  float baseRadius;
  float circleWidth;
  std::vector<positionColorVertex> vertexBuffer;
  std::vector<uint16_t> indexBuffer;
  /*bgfx::ProgramHandle program;
  bgfx::DynamicVertexBufferHandle circleVBO;
  bgfx::IndexBufferHandle circleEBO;*/
};

#endif
