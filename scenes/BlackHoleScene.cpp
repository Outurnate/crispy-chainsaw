#include "BlackHoleScene.hpp"

#include <algorithm>
#include <range/v3/algorithm/for_each.hpp>
#include <OgreSceneNode.h>

#include "Math.hpp"

BlackHoleScene::BlackHoleScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet)
  : Scene(displayName, name, optionSet),
    points(2 * (spectrumSize(SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange))),
    baseRadius(0.15f),
    circleWidth(0.01f)
{
  optionSet.registerOption(ConfigurationManager::Option("asdf", 0.5));
}

Ogre::Camera& BlackHoleScene::getCamera()
{
  return *camera;
}

void BlackHoleScene::initialize()
{
  camera = sceneManager->createCamera("Camera");
  blackHoleMesh = sceneManager->createManualObject("BlackHole");

  Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode& blackHoleNode = *sceneManager->getRootSceneNode()->createChildSceneNode();

  cameraNode.attachObject(camera);
  blackHoleNode.attachObject(blackHoleMesh);

  blackHoleMesh->setDynamic(true);
  blackHoleMesh->begin("material");
  for (unsigned i = 0; i < points; ++i)
  {
    uint16_t topLeft     = i;
    uint16_t bottomLeft  = points + i;
    uint16_t topRight    = (i + 1) % points;
    uint16_t bottomRight = points + ((i + 1) % points);

    blackHoleMesh->index(bottomLeft);
    blackHoleMesh->index(topRight);
    blackHoleMesh->index(topLeft);

    blackHoleMesh->index(bottomLeft);
    blackHoleMesh->index(bottomRight);
    blackHoleMesh->index(topRight);
  }
  for (unsigned i = 0; i < points; ++i)
  {
    blackHoleMesh->position(0, 0, 0);
  }
  for (unsigned i = 0; i < points; ++i)
  {
    blackHoleMesh->position(0, 0, 0);
  }
  blackHoleMesh->end();
}

void BlackHoleScene::updateAudio(const FFTSpectrumData& audioFrame)
{
  // begin lazy inefficient code TODO
  std::vector<float> left, right, combined;
  ranges::for_each(
      spectrumView(audioFrame, SpectrumRange::LowMidrange, SpectrumRange::UpperMidrange),
      [&left, &right](AudioPoint point) { left.push_back(point.getChannel(Channel::Left)); right.push_back(point.getChannel(Channel::Right)); });
  std::reverse(right.begin(), right.end());
  for (float x : right)
    combined.push_back(x);
  for (float x : left)
    combined.push_back(x);
  // end lazy inefficient code TODO

  blackHoleMesh->beginUpdate(0);
  for (unsigned i = 0; i < points; ++i)
  {
    float radius = baseRadius + (combined[i] / 7.0f);
    float theta = float(i) / float(points) * (2 * M_PI);
    theta += M_PI / 2;

    blackHoleMesh->position(radius * cos(theta), radius * sin(theta), 0.0);
  }
  for (unsigned i = 0; i < points; ++i)
  {
    float theta = float(i) / float(points) * (2 * M_PI);
    theta += M_PI / 2;

    blackHoleMesh->position(baseRadius * cos(theta), baseRadius * sin(theta), 0.0);
  }
  blackHoleMesh->end();
}

void BlackHoleScene::update(double delta)
{
  (void)delta;
}
