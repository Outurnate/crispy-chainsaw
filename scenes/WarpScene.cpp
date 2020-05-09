#include "WarpScene.hpp"

#include <OgreEntity.h>

WarpScene::WarpScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet)
  : Scene(displayName, name, optionSet),
    left(7, nullptr),
    right(7, nullptr)
{
}

void WarpScene::update(double delta)
{
}

void WarpScene::updateAudio(const FFTSpectrumData& audioFrame)
{
  left[0]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::SubBass,       SpectrumRange::SubBass,       Channel::Left));
  left[1]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Bass,          SpectrumRange::Bass,          Channel::Left));
  left[2]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::LowMidrange,   SpectrumRange::LowMidrange,   Channel::Left));
  left[3]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Midrange,      SpectrumRange::Midrange,      Channel::Left));
  left[4]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::UpperMidrange, SpectrumRange::UpperMidrange, Channel::Left));
  left[5]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Presence,      SpectrumRange::Presence,      Channel::Left));
  left[6]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Brilliance,    SpectrumRange::Brilliance,    Channel::Left));

  right[0]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::SubBass,       SpectrumRange::SubBass,       Channel::Right));
  right[1]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Bass,          SpectrumRange::Bass,          Channel::Right));
  right[2]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::LowMidrange,   SpectrumRange::LowMidrange,   Channel::Right));
  right[3]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Midrange,      SpectrumRange::Midrange,      Channel::Right));
  right[4]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::UpperMidrange, SpectrumRange::UpperMidrange, Channel::Right));
  right[5]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Presence,      SpectrumRange::Presence,      Channel::Right));
  right[6]->setScale(.5, .5, 0.01f + 200 * spectrumAverage(audioFrame, SpectrumRange::Brilliance,    SpectrumRange::Brilliance,    Channel::Right));
}

Ogre::SceneNode* WarpScene::buildColumn(Ogre::SceneNode& parent, float x)
{
  Ogre::Entity& entity = *sceneManager->createEntity("Cylinder.mesh");
  entity.setMaterialName("Solid/Red");

  Ogre::SceneNode* node = parent.createChildSceneNode();
  node->attachObject(&entity);
  node->pitch(Ogre::Degree(-90));
  node->scale(.5, .5, 0.01);
  node->translate(x, 0, 0);

  return node;
}

void WarpScene::initialize()
{
  sceneManager->setSkyBox(true, "skyboxes/space");
  camera = sceneManager->createCamera("Camera");
  camera->setNearClipDistance(1);

  Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
  cameraNode.setPosition(0, 0, 90);
  cameraNode.lookAt(Ogre::Vector3(0, 0, -300), Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Z);
  cameraNode.attachObject(camera);

  Ogre::SceneNode& mainNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
  mainNode.pitch(Ogre::Degree(190));

  int i = 1;
  Ogre::SceneNode& leftNode = *mainNode.createChildSceneNode();
  leftNode.yaw(Ogre::Degree(45));
  for (auto& node : left)
    node = buildColumn(leftNode, 1.25f * i++);

  i = 1;
  Ogre::SceneNode& rightNode = *mainNode.createChildSceneNode();
  rightNode.yaw(Ogre::Degree(-45));
  for (auto& node : right)
    node = buildColumn(rightNode, -1.25f * i++);

  sceneManager->setAmbientLight(Ogre::ColourValue(.5, .5, .5));
  Ogre::Light& light = *sceneManager->createLight("MainLight");
  light.setPosition(20, 80, 50);
}

Ogre::Camera& WarpScene::getCamera()
{
  return *camera;
}
