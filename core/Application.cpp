#include "Application.hpp"

#include <spdlog/spdlog.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreRenderWindow.h>
#include <OgreTextureManager.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreGL3PlusPlugin.h>
#include <OgreShaderGenerator.h>
#include <OgreMaterialManager.h>
#include <OgreSGTechniqueResolverListener.h>
#include <Bites/OgreWindowEventUtilities.h>

struct sceneManagerDeleter
{
  void operator()(Ogre::SceneManager* sceneManager) const
  {
    Ogre::Root::getSingleton().destroySceneManager(sceneManager);
  }
};

typedef std::unique_ptr<Ogre::SceneManager, sceneManagerDeleter> ScopedSceneManager;

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

  {
    sceneManager.setScene(0);

    ScopedSceneManager sceneManager(root.createSceneManager("DefaultSceneManager"));

    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        Ogre::RTShader::ShaderGenerator::getSingletonPtr()->addSceneManager(sceneManager.get());
        Ogre::RTShader::ShaderGenerator::getSingletonPtr()->setShaderCachePath("");

        Ogre::MaterialManager::getSingleton().addListener(new OgreBites::SGTechniqueResolverListener(Ogre::RTShader::ShaderGenerator::getSingletonPtr()));
    }
    else
    {
      spdlog::get("ogre")->critical("Failed to initialize ShaderGenerator");
      return;
    }

    sceneManager->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

    Ogre::SceneNode& cameraNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
    Ogre::Camera& camera = *sceneManager->createCamera("Main");
    Ogre::Viewport& viewport = *window.addViewport(&camera);

    viewport.setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    camera.setNearClipDistance(5);
    camera.setAspectRatio(Ogre::Real(viewport.getActualWidth()) / Ogre::Real(viewport.getActualHeight()));
    cameraNode.setPosition(0, 0, 80);
    cameraNode.lookAt(Ogre::Vector3(0, 0, -300), Ogre::Node::TS_WORLD, Ogre::Vector3::NEGATIVE_UNIT_Z );
    cameraNode.attachObject(&camera);

    Ogre::SceneNode& ogreNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity& ogreEntity = *sceneManager->createEntity("ogrehead.mesh");

    ogreNode.attachObject(&ogreEntity);

    Ogre::SceneNode& lightNode = *sceneManager->getRootSceneNode()->createChildSceneNode();
    Ogre::Light& light = *sceneManager->createLight("MainLight");

    lightNode.setPosition(20, 80, 50);
    lightNode.attachObject(&light);

    while(true)
    {
      Ogre::WindowEventUtilities::messagePump();

      if(window.isClosed()) return;// false;

      if(!root.renderOneFrame()) return;// false;
      window.windowMovedOrResized(); // TODO THIS BAD
    }
  }
}
