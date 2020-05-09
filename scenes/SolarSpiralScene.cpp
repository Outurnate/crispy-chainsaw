#include "SolarSpiralScene.hpp"

#include <OgreParticleSystem.h>
#include <OgreTextureManager.h>
#include <OgreRenderTexture.h>
#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRectangle2D.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreMeshManager.h>
#include <OgreMovablePlane.h>
#include <OgreEntity.h>

SolarSpiralScene::SolarSpiralScene(const std::string& displayName, const std::string& name, ConfigurationManager::OptionSet& optionSet)
  : Scene(displayName, name, optionSet)
{
}

double p = 0;

void SolarSpiralScene::update(double delta)
{
  p += 0.005;
  particleEmitter->setPosition(sin(p) * 20, cos(p) * 20, 0);
}

void SolarSpiralScene::updateAudio(const FFTSpectrumData& audioFrame)
{
}

void SolarSpiralScene::initialize()
{
  sceneManager->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
  sceneManager->getRootSceneNode()
    ->createChildSceneNode(Ogre::Vector3(20, 80, 50))
    ->attachObject(sceneManager->createLight());
  
  camera = sceneManager->createCamera("Camera");

  Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();

  camera->setNearClipDistance(5);

  cameraNode.setPosition(0, 0, 200);
  cameraNode.lookAt(Ogre::Vector3(0, 0, -300), Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Z);
  cameraNode.attachObject(camera);

  Ogre::ParticleSystem::setDefaultNonVisibleUpdateTimeout(5);

  Ogre::ParticleSystem& ps = *sceneManager->createParticleSystem("Nova", "SolarSpiral/Nova");
  particleEmitter = sceneManager->getRootSceneNode()->createChildSceneNode();
  particleEmitter->attachObject(&ps);

  Ogre::TexturePtr rttTexture = Ogre::TextureManager::getSingleton().createManual(
    "RttTex",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_2D,
    256, 256,
    0,
    Ogre::PF_R8G8B8A8,
    Ogre::TU_RENDERTARGET
  );

  Ogre::RenderTexture& renderTexture = *rttTexture->getBuffer()->getRenderTarget();
  
  renderTexture.addViewport(camera);
  renderTexture.getViewport(0)->setClearEveryFrame(true);
  renderTexture.getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
  renderTexture.getViewport(0)->setOverlaysEnabled(false);

  renderTexture.update();
  renderTexture.setAutoUpdated(true);

  Ogre::MaterialPtr renderMaterial = Ogre::MaterialManager::getSingleton().create("RttMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
  renderMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
  renderMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
  renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex");

  Ogre::MovablePlane& plane = *(new Ogre::MovablePlane("Plane"));
  plane.d = 0;
  plane.normal = Ogre::Vector3::UNIT_Z;

  Ogre::MeshManager::getSingleton().createPlane(
    "PlaneMesh",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    plane,
    200, 200, 1, 1,
    true,
    1, 1, 1,
    Ogre::Vector3::UNIT_Y);
  Ogre::Entity& planeEntity = *sceneManager->createEntity("PlaneMesh");
  planeEntity.setMaterial(renderMaterial);
  //planeEntity.setMaterialName("Solid/Red");

  Ogre::SceneNode& planeNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
  planeNode.attachObject(&planeEntity);
}

Ogre::Camera& SolarSpiralScene::getCamera()
{
  return *camera;
}
