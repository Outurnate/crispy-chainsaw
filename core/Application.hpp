#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <OgreLog.h>

class Application : public Ogre::LogListener
{
public:
  Application();

  void run();
  void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage) override;
};

#endif
