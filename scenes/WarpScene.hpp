#ifndef WARPSCENE_HPP
#define WARPSCENE_HPP

#include "SceneManager.hpp"

#include <vector>
#include <OgreSceneNode.h>

class WarpScene : public Scene
{
public:
  WarpScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet);

  void update(double delta) override;
  void updateAudio(const FFTSpectrumData& audioFrame) override;
  void initialize() override;
  Ogre::Camera& getCamera() override;
private:
  Ogre::SceneNode* buildColumn(Ogre::SceneNode& parent, float x);

  Ogre::Camera* camera;
  std::vector<Ogre::SceneNode*> left;
  std::vector<Ogre::SceneNode*> right;
};

#endif
