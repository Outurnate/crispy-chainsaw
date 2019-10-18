#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include "gfxUtils.hpp"
#include <bigg.hpp>
#include <string>
#include <unordered_map>
#include <boost/core/noncopyable.hpp>

class resourceManager : private boost::noncopyable
{
public:
  resourceManager(bigg::Allocator& allocator);
  virtual ~resourceManager();

  bgfx::ProgramHandle getShader(const std::string& name);
  bgfx::TextureHandle getTexture(const std::string& name);
private:
  bigg::Allocator& allocator;
  std::unordered_map<std::string, bgfx::ProgramHandle> programs;
  std::unordered_map<std::string, bgfx::TextureHandle> textures;
};

#endif
