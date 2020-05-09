#ifndef SOLARSPIRALSCENE_HPP
#define SOLARSPIRALSCENE_HPP

#include "SceneManager.hpp"

class SolarSpiralScene : public Scene
{
public:
  SolarSpiralScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet);

  void update(double delta) override;
  void updateAudio(const FFTSpectrumData& audioFrame) override;
  void initialize() override;
  Ogre::Camera& getCamera() override;
private:
  Ogre::Camera* camera;
  Ogre::SceneNode* particleEmitter;
};

#endif
