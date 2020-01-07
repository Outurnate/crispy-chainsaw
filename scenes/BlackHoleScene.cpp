#include "BlackHoleScene.hpp"

#include <algorithm>
#include <range/v3/algorithm/for_each.hpp>
#include <OgreSceneNode.h>

#include "Math.hpp"

BlackHoleScene::BlackHoleScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet)
  : Scene(displayName, name, optionSet),
    points(2 * (spectrumSize(SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange))),
    audioData(points, 0.0f),
    baseRadius(0.15f),
    circleWidth(0.01f)
{
  optionSet.registerOption(ConfigurationManager::Option("asdf", 0.5));
}

Ogre::Camera& BlackHoleScene::getCamera()
{
  return *camera;
}

Ogre::Vector2 polarToRect(Ogre::Real radius, Ogre::Real theta)
{
  return Ogre::Vector2(radius * cos(theta), radius * sin(theta));
}

void BlackHoleScene::initialize()
{
  sceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f));

  camera = sceneManager->createCamera("Camera");
  blackHoleMesh = sceneManager->createManualObject("BlackHole");

  Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode& blackHoleNode = *sceneManager->getRootSceneNode()->createChildSceneNode();

  camera->setNearClipDistance(5);

  cameraNode.setPosition(0, 0, 10);
  cameraNode.lookAt(Ogre::Vector3(0, 0, -300), Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Z);
  cameraNode.attachObject(camera);

  blackHoleNode.scale(10, 10, 10);
  blackHoleNode.setPosition(0, 0, 0);
  blackHoleNode.attachObject(blackHoleMesh);

  blackHoleMesh->setDynamic(true);
  blackHoleMesh->begin("Red");
  unsigned c = 0;
  for (unsigned i = 0; i < points; ++i)
  {
    blackHoleMesh->position(0, 0, 0); // 0
    blackHoleMesh->position(0, 0, 0); // 1
    blackHoleMesh->position(0, 0, 0); // 2
    blackHoleMesh->position(0, 0, 0); // 3
    blackHoleMesh->triangle(c + 0, c + 1, c + 2);
    blackHoleMesh->triangle(c + 1, c + 3, c + 2);
    c += 4;
  }
  blackHoleMesh->end();
}

void BlackHoleScene::updateAudio(const FFTSpectrumData& audioFrame)
{
  std::vector<float> left, right;
  ranges::for_each(
      spectrumView(audioFrame, SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange),
      [&left, &right](AudioPoint point) { left.push_back(point.getChannel(Channel::Left)); right.push_back(point.getChannel(Channel::Right)); });
  std::reverse(right.begin(), right.end()); // TODO range?
  unsigned i = 0;
  for (float x : right)
    audioData[i++] = x;
  for (float x : left)
    audioData[i++] = x;
}

void BlackHoleScene::update(double delta)
{
  (void)delta;

  blackHoleMesh->beginUpdate(0);
  unsigned c = 0;
  for (unsigned i = 0; i < points; ++i)
  {
    float currentVal = audioData[i];
    float nextVal = audioData[(i + 1) % points];
    Ogre::Vector2 currentInner = polarToRect(baseRadius,                                     float(i)     / float(points) * (2 * M_PI));
    Ogre::Vector2 currentOuter = polarToRect(baseRadius + circleWidth + (currentVal / 7.0f), float(i)     / float(points) * (2 * M_PI));
    Ogre::Vector2 nextInner    = polarToRect(baseRadius,                                     float(i + 1) / float(points) * (2 * M_PI));
    Ogre::Vector2 nextOuter    = polarToRect(baseRadius + circleWidth + (nextVal / 7.0f),    float(i + 1) / float(points) * (2 * M_PI));
    blackHoleMesh->position(currentInner.x, currentInner.y, 0); // 0
    blackHoleMesh->position(currentOuter.x, currentOuter.y, 0); // 1
    blackHoleMesh->position(nextInner.x,    nextInner.y,    0); // 2
    blackHoleMesh->position(nextOuter.x,    nextOuter.y,    0); // 3
    blackHoleMesh->triangle(c + 0, c + 1, c + 2);
    blackHoleMesh->triangle(c + 1, c + 3, c + 2);
    c += 4;
  }
  blackHoleMesh->end();
}
