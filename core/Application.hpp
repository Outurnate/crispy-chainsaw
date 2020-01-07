#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <OgreLog.h>
#include <OgreFrameListener.h>

#include "ConfigurationManager.hpp"
#include "SceneManager.hpp"

class Application : public Ogre::LogListener, public Ogre::FrameListener
{
public:
  Application();

  void run();
  void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage) override;
  bool frameStarted(const Ogre::FrameEvent& event) override;
private:
  SceneManager sceneManager;
  ConfigurationManager configurationManager;
};

#endif
