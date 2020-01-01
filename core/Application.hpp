#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <OgreRoot.h>
#include <memory>

class Application
{
public:
  void run();
private:
  std::unique_ptr<Ogre::Root> root;
};

#endif
