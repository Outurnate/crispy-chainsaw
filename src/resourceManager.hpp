#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include "gfxUtils.hpp"

#include <string>
#include <unordered_map>

class resourceManager
{
public:
  resourceManager();
  virtual ~resourceManager();

  resourceManager(const resourceManager&) = delete;
  resourceManager& operator=(const resourceManager&) = delete;

  resourceManager(resourceManager&&) = default;
  resourceManager& operator=(resourceManager&&) = default;

  bgfx::ProgramHandle getShader(const std::string& name);
  bgfx::TextureHandle getTexture(const std::string& name);
private:
  std::unordered_map<std::string, bgfx::ProgramHandle> programs;
  std::unordered_map<std::string, bgfx::TextureHandle> textures;
};

#endif
