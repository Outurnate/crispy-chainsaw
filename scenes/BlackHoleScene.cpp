#include "BlackHoleScene.hpp"

#include <algorithm>
#include <range/v3/algorithm/for_each.hpp>
#include <fmt/format.h>
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

  Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();

  camera->setNearClipDistance(5);

  cameraNode.setPosition(0, 0, 10);
  cameraNode.lookAt(Ogre::Vector3(0, 0, -300), Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Z);
  cameraNode.attachObject(camera);

  for (unsigned j = 0; j < blackHoleMeshes.size(); ++j)
  {
    Ogre::SceneNode& blackHoleNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
    blackHoleNode.scale(10, 10, 10);
    blackHoleNode.roll(Ogre::Degree(90));
    blackHoleNode.setPosition(0, 0, float((blackHoleMeshes.size() - 1) - j) / 1000.0f);

    blackHoleMeshes[j] = sceneManager->createManualObject(fmt::format("BlackHole{}", j));
    blackHoleMeshes[j]->setDynamic(true);
    blackHoleNode.attachObject(blackHoleMeshes[j]);

    blackHoleMeshes[j]->begin("2DAmbientOnly");
    unsigned c = 0;
    for (unsigned i = 0; i < points; ++i)
    {
      blackHoleMeshes[j]->position(0, 0, 0); blackHoleMeshes[j]->colour(0.0, 0.0, 0.0, 0.0); // 0
      blackHoleMeshes[j]->position(0, 0, 0); blackHoleMeshes[j]->colour(0.0, 0.0, 0.0, 0.0); // 1
      blackHoleMeshes[j]->position(0, 0, 0); blackHoleMeshes[j]->colour(0.0, 0.0, 0.0, 0.0); // 2
      blackHoleMeshes[j]->position(0, 0, 0); blackHoleMeshes[j]->colour(0.0, 0.0, 0.0, 0.0); // 3
      blackHoleMeshes[j]->triangle(c + 0, c + 1, c + 2);
      blackHoleMeshes[j]->triangle(c + 1, c + 3, c + 2);
      c += 4;
    }
    blackHoleMeshes[j]->end();
  }
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

  Ogre::ColourValue inner[] { Ogre::ColourValue(1.0, 1.0, 1.0, 1.0), Ogre::ColourValue(1.0, 0.5, 0.0, 1.0), Ogre::ColourValue(1.0, 0.0, 0.0, 1.0) };
  Ogre::ColourValue outer[] { Ogre::ColourValue(1.0, 1.0, 1.0, 0.0), Ogre::ColourValue(1.0, 0.5, 0.0, 0.0), Ogre::ColourValue(1.0, 0.0, 0.0, 0.0) };
  for (unsigned j = 0; j < blackHoleMeshes.size(); ++j)
  {
    blackHoleMeshes[j]->beginUpdate(0);
    unsigned c = 0;
    for (unsigned i = 0; i < points; ++i)
    {
      float currentVal = audioData[i] * float(j + 1);
      float nextVal = audioData[(i + 1) % points] * float(j + 1);
      float innerSize = baseRadius;
      float outerSize = baseRadius + (circleWidth * (j + 1));
      Ogre::Vector2 currentInner = polarToRect(innerSize,                       float(i)     / float(points) * (2 * M_PI));
      Ogre::Vector2 currentOuter = polarToRect(outerSize + (currentVal / 4.0f), float(i)     / float(points) * (2 * M_PI));
      Ogre::Vector2 nextInner    = polarToRect(innerSize,                       float(i + 1) / float(points) * (2 * M_PI));
      Ogre::Vector2 nextOuter    = polarToRect(outerSize + (nextVal / 4.0f),    float(i + 1) / float(points) * (2 * M_PI));
      blackHoleMeshes[j]->position(currentInner.x, currentInner.y, 0); blackHoleMeshes[j]->colour(inner[j]); // 0
      blackHoleMeshes[j]->position(currentOuter.x, currentOuter.y, 0); blackHoleMeshes[j]->colour(outer[j]); // 1
      blackHoleMeshes[j]->position(nextInner.x,    nextInner.y,    0); blackHoleMeshes[j]->colour(inner[j]); // 2
      blackHoleMeshes[j]->position(nextOuter.x,    nextOuter.y,    0); blackHoleMeshes[j]->colour(outer[j]); // 3
      blackHoleMeshes[j]->triangle(c + 0, c + 1, c + 2);
      blackHoleMeshes[j]->triangle(c + 1, c + 3, c + 2);
      c += 4;
    }
    blackHoleMeshes[j]->end();
  }
}
