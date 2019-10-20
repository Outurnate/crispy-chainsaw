#include "resourceManager.hpp"

#include <bimg/bimg.h>
#include <boost/format.hpp>
#include <exception>
#include <vector>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>

const std::string shaderTemplatePath = "assets/shaders/%3%/%2%_%1%.bin";
const std::string textureTemplatePath = "assets/textures/%1%.ktx";

resourceManager::resourceManager(bigg::Allocator& allocator)
  : allocator(allocator)
{
}

resourceManager::~resourceManager()
{
  for (auto& pair : programs)
    deleteHandle(pair.second);
}

std::vector<char> readFile(const fs::path& path)
{
  if (!is_regular_file(path))
    throw std::runtime_error((boost::format("%1% is not a regular file") % path).str());

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size))
    return buffer;
  else
    throw std::runtime_error((boost::format("%1% could not be read") % path).str());
}

bgfx::ProgramHandle resourceManager::getShader(const std::string& name)
{
  if (programs.contains(name))
    return programs[name];
  else
  {
    auto handle = bigg::loadProgram((boost::format(shaderTemplatePath) % name % "vs" % "glsl").str().c_str(), (boost::format(shaderTemplatePath) % name % "fs" % "glsl").str().c_str());
    programs.insert({ name, handle });
    return handle;
  }
}

static void imageReleaseCb(void* _ptr, void* _userData)
{
  bimg::imageFree((bimg::ImageContainer*)_userData);
}

bgfx::TextureHandle loadTexture(bigg::Allocator& allocator, const fs::path& path)
{
  uint64_t flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE;
  bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

  std::vector<char> file = readFile(path);
  bimg::ImageContainer* imageContainer(bimg::imageParseKtx(&allocator, file.data(), file.size() * sizeof(char), nullptr));

  if (NULL != imageContainer)
  {
    bimg::Orientation::Enum orientation = imageContainer->m_orientation;

    const bgfx::Memory* mem = bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer);

    if (imageContainer->m_cubeMap)
      handle = bgfx::createTextureCube(uint16_t(imageContainer->m_width), 1 < imageContainer->m_numMips, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), flags, mem);
    else if (1 < imageContainer->m_depth)
      handle = bgfx::createTexture3D(uint16_t(imageContainer->m_width), uint16_t(imageContainer->m_height), uint16_t(imageContainer->m_depth), 1 < imageContainer->m_numMips, bgfx::TextureFormat::Enum(imageContainer->m_format), flags, mem);
    else if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), flags))
      handle = bgfx::createTexture2D(uint16_t(imageContainer->m_width), uint16_t(imageContainer->m_height), 1 < imageContainer->m_numMips, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), flags, mem);

    if (bgfx::isValid(handle))
      bgfx::setName(handle, path.string().c_str());
  }

  return handle;
}

bgfx::TextureHandle resourceManager::getTexture(const std::string& name)
{
  if (textures.contains(name))
    return textures[name];
  else
  {
    auto handle = loadTexture(allocator, (boost::format(textureTemplatePath) % name).str().c_str());
    textures.insert({ name, handle });
    return handle;
  }
}
