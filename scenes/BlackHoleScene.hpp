#ifndef CIRCLESPECTRUMSCENE_HPP
#define CIRCLESPECTRUMSCENE_HPP

#include "SceneManager.hpp"
#include <vector>
#include <OgreManualObject.h>

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
  Ogre::ManualObject* blackHoleMesh;
  unsigned points;
  float baseRadius;
  float circleWidth;
};

#endif