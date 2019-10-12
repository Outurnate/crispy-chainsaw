#include "circleSpectrumScene.hpp"

#include "audioEngine.hpp"

#include <algorithm>
#include <iostream>

circleSpectrumScene::circleSpectrumScene()
  : points(2 * (rangedSize(spectrumRange::lowMidrange) + rangedSize(spectrumRange::midrange) + rangedSize(spectrumRange::upperMidrange))),
    vertexBuffer(2 * points, positionColorVertex { 0.0f, 0.0f, 0.0f, 0xff000000 }),
    indexBuffer(6 * points, 0)
{
  char vsName[64];
  char fsName[64];

  const char* shaderPath = "???";

  switch (bgfx::getRendererType())
  {
    case bgfx::RendererType::Noop:
    case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
    case bgfx::RendererType::Gnm:                                       break;
    case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
    case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
    case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
    case bgfx::RendererType::Vulkan:                                    break;
    case bgfx::RendererType::Count:                                     break;
  }

  shaderPath = "assets/shaders/glsl/";

  bx::strCopy(vsName, BX_COUNTOF(vsName), shaderPath);
  bx::strCat(vsName, BX_COUNTOF(vsName), "vs_cubes.bin");

  bx::strCopy(fsName, BX_COUNTOF(fsName), shaderPath);
  bx::strCat(fsName, BX_COUNTOF(fsName), "fs_cubes.bin");

  program = bigg::loadProgram(vsName, fsName);

  unsigned c = 0;
  for (unsigned i = 0; i < points; ++i)
  {
    uint16_t topLeft     = i;
    uint16_t bottomLeft  = points + i;
    uint16_t topRight    = (i + 1) % points;
    uint16_t bottomRight = points + ((i + 1) % points);

    indexBuffer[c++] = bottomLeft;
    indexBuffer[c++] = topRight;
    indexBuffer[c++] = topLeft;

    indexBuffer[c++] = bottomLeft;
    indexBuffer[c++] = bottomRight;
    indexBuffer[c++] = topRight;
  }

  circleVBO = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()), positionColorVertex::msLayout);
  circleEBO = bgfx::createIndexBuffer(bgfx::makeRef(indexBuffer.data(), sizeof(uint16_t) * indexBuffer.size()));

  bgfx::setDebug(BGFX_DEBUG_TEXT);
}

circleSpectrumScene::~circleSpectrumScene()
{
}

void circleSpectrumScene::updateBuffers(const audioAnalyzedFrame& audioFrame)
{
  // begin lazy inefficient code TODO
  std::vector<float> left, right, combined;
  std::for_each(rangedBegin(audioFrame.spectrum, spectrumRange::lowMidrange), rangedEnd(audioFrame.spectrum, spectrumRange::upperMidrange), [&left, &right](audioPoint point) { left.push_back(point.getLeft()); right.push_back(point.getRight()); });
  std::reverse(right.begin(), right.end());
  for (float x : left)
    combined.push_back(x);
  for (float x : right)
    combined.push_back(x);
  // end lazy inefficient code TODO

  float baseRadius = 0.15f;
  float width = 0.01f;
  for (unsigned i = 0; i < points; ++i)
  {
    float radius = baseRadius + (combined[i] / 15.0f);
    float theta = float(i) / float(points) * (2 * M_PI);
    theta += M_PI / 2;

    glm::vec2 outerPoint = polarToRect(radius + width, theta);
    glm::vec2 innerPoint = polarToRect(baseRadius,     theta);
    vertexBuffer[i         ] = { outerPoint.x, outerPoint.y, 0.0f, 0xff00ffff };
    vertexBuffer[i + points] = { innerPoint.x, innerPoint.y, 0.0f, 0xff0055ff };
  }

  bgfx::update(circleVBO, 0, bgfx::copy(vertexBuffer.data(), sizeof(positionColorVertex) * vertexBuffer.size()));
}

void circleSpectrumScene::update(const audioAnalyzedFrame& audioFrame, double delta, float width, float height)
{
  float aspect = width / height;
  updateBuffers(audioFrame);

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::ortho(-1 * aspect, aspect, -1.0f, 1.0f, -50.0f, 50.0f);
  bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::touch(0);
  for (uint32_t yy = 0; yy < 11; ++yy)
  {
    for (uint32_t xx = 0; xx < 11; ++xx)
    {
      glm::mat4 mtx = glm::identity<glm::mat4>();
      mtx *= glm::scale(mtx, glm::vec3(aspect, aspect, 0.0f));
      bgfx::setTransform(&mtx[0][0]);
      bgfx::setVertexBuffer(0, circleVBO);
      bgfx::setIndexBuffer(circleEBO);
      bgfx::setState(BGFX_STATE_DEFAULT);
      bgfx::submit(0, program);
    }
  }
}
