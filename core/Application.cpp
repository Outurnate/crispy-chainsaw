#include "Application.hpp"

#include <spdlog/spdlog.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreRenderWindow.h>
#include <OgreTextureManager.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneNode.h>
#include <OgreGL3PlusPlugin.h>
#include <OgreShaderGenerator.h>
#include <Bites/OgreWindowEventUtilities.h>

Application::Application()
  : sceneManager(),
    configurationManager(sceneManager.getOptionSet())
{
  configurationManager.flush();
}

void Application::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage)
{
  (void)maskDebug;
  (void)logName;
  (void)skipThisMessage;

  switch(lml)
  {
  case Ogre::LML_TRIVIAL:
    spdlog::get("ogre")->debug(message);
    break;
  case Ogre::LML_NORMAL:
    spdlog::get("ogre")->info(message);
    break;
  case Ogre::LML_WARNING:
    spdlog::get("ogre")->warn(message);
    break;
  case Ogre::LML_CRITICAL:
    spdlog::get("ogre")->error(message);
    break;
  }
}

void Application::run()
{
  Ogre::LogManager log;
  log.createLog("", true, false, true);
  log.getDefaultLog()->addListener(this);

  Ogre::Root root("", "", "");

  Ogre::GL3PlusPlugin gl3plus;
  root.installPlugin(&gl3plus);
  root.setRenderSystem(root.getRenderSystemByName("OpenGL 3+ Rendering Subsystem"));

  Ogre::RenderWindow& window = *root.initialise(true, "Death by cold fries");

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./RTShaderLib", "FileSystem");
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./RTShaderLib/GLSL", "FileSystem");
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./RTShaderLib/HLSL_Cg", "FileSystem");
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  Ogre::SceneManager& nullSceneManager = *root.createSceneManager("DefaultSceneManager");
  Ogre::Camera& nullCamera = *nullSceneManager.createCamera("NullCamera");
  Ogre::Viewport& viewport = *window.addViewport(&nullCamera);
  Ogre::SceneNode& cameraNode = *nullSceneManager.getRootSceneNode()->createChildSceneNode();

  viewport.setBackgroundColour(Ogre::ColourValue(0, 0, 0));
  cameraNode.attachObject(&nullCamera);

  sceneManager.setRoot(root, viewport);
  sceneManager.setScene(0);

  while(true)
  {
    Ogre::WindowEventUtilities::messagePump();

    if(window.isClosed()) return;// false;

    if(!root.renderOneFrame()) return;// false;

    sceneManager.frame();
    window.windowMovedOrResized(); // TODO THIS BAD
  }
}
