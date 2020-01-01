#include "Application.hpp"

#include <OgreRenderWindow.h>
#include <OgreTextureManager.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <Bites/OgreWindowEventUtilities.h>

void Application::run()
{
  root.reset(new Ogre::Root);
  root->showConfigDialog(nullptr);
  /*if(!(root->restoreConfig() || root->showConfigDialog(nullptr)))
    return;// false;*/

  std::unique_ptr<Ogre::RenderWindow> window(root->initialise(true, "Death by cold fries"));

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  std::unique_ptr<Ogre::SceneManager> sceneManager(root->createSceneManager(Ogre::ST_GENERIC));
  std::unique_ptr<Ogre::Camera> camera(sceneManager->createCamera("Main"));

  camera->setPosition(0, 0, 80);
  camera->lookAt(0, 0, -300);
  camera->setNearClipDistance(5);

  std::unique_ptr<Ogre::Viewport> viewport(window->addViewport(camera.get()));

  viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

  camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

  std::unique_ptr<Ogre::Entity> ogreEntity(sceneManager->createEntity("ogrehead.mesh"));

  std::unique_ptr<Ogre::SceneNode> ogreNode(sceneManager->getRootSceneNode()->createChildSceneNode());
  ogreNode->attachObject(ogreEntity.get());

  sceneManager->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

  std::unique_ptr<Ogre::Light> light(sceneManager->createLight("MainLight"));
  light->setPosition(20, 80, 50);

  while(true)
  {
    Ogre::WindowEventUtilities::messagePump();

    if(window->isClosed()) return;// false;

    if(!root->renderOneFrame()) return;// false;
  }
}
