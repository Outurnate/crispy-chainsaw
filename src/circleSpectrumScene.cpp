#include "circleSpectrumScene.hpp"

#include <bigg.hpp>
#include <bx/string.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

struct PosColorVertex
{
  float x;
  float y;
  float z;
  uint32_t abgr;

  static void init()
  {
    msLayout
      .begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
      .end();
  };

  static bgfx::VertexDecl msLayout;
};
bgfx::VertexDecl PosColorVertex::msLayout;

static PosColorVertex s_cubeVertices[] =
{
  {-1.0f,  1.0f,  1.0f, 0xff000000 },
  { 1.0f,  1.0f,  1.0f, 0xff0000ff },
  {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
  { 1.0f, -1.0f,  1.0f, 0xff00ffff },
  {-1.0f,  1.0f, -1.0f, 0xffff0000 },
  { 1.0f,  1.0f, -1.0f, 0xffff00ff },
  {-1.0f, -1.0f, -1.0f, 0xffffff00 },
  { 1.0f, -1.0f, -1.0f, 0xffffffff },
};
static const uint16_t s_cubeTriList[] = { 2, 1, 0, 2, 3, 1, 5, 6, 4, 7, 6, 5, 4, 2, 0, 6, 2, 4, 3, 5, 1, 3, 7, 5, 1, 4, 0, 1, 5, 4, 6, 3, 2, 7, 3, 6 };

circleSpectrumScene::circleSpectrumScene()
{
  PosColorVertex::init();

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

  std::cout << vsName << std::endl;
  std::cout << fsName << std::endl;

  mProgram = bigg::loadProgram(vsName, fsName);
  mVbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::msLayout);
  mIbh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));
  bgfx::setDebug(BGFX_DEBUG_TEXT);
  mTime = 0.0f;
}

circleSpectrumScene::~circleSpectrumScene()
{
}

void circleSpectrumScene::update(double delta, float width, float height)
{
  mTime += delta;
  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 100.0f);
  bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
  bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
  bgfx::touch(0);
  for (uint32_t yy = 0; yy < 11; ++yy)
  {
    for (uint32_t xx = 0; xx < 11; ++xx)
    {
      glm::mat4 mtx = glm::identity<glm::mat4>();
      mtx = glm::translate(mtx, glm::vec3(15.0f - float(xx) * 3.0f, -15.0f + float(yy) * 3.0f, 0.0f));
      mtx *= glm::yawPitchRoll(mTime + xx * 0.21f, mTime + yy * 0.37f, 0.0f);
      bgfx::setTransform(&mtx[0][0]);
      bgfx::setVertexBuffer(0, mVbh);
      bgfx::setIndexBuffer(mIbh);
      bgfx::setState(BGFX_STATE_DEFAULT);
      bgfx::submit(0, mProgram);
    }
  }
}

/*  glBindVertexArray(simpleVAO);

  const audioAnalyzer::fftSpectrumData& data = stream.getLatestFrame().spectrum;

  circlePoints = audioAnalyzer::rangedSize(spectrumRange::lowMidrange) + audioAnalyzer::rangedSize(spectrumRange::midrange) + audioAnalyzer::rangedSize(spectrumRange::upperMidrange);
  std::vector<float> points;
  std::vector<float> pointsColors;

  std::vector<float> rawData;

  std::for_each(audioAnalyzer::rangedBegin(data, spectrumRange::lowMidrange), audioAnalyzer::rangedEnd(data, spectrumRange::upperMidrange), [&rawData](audioPoint point) { rawData.push_back(point.getLeft()); });
  for (unsigned i = 0; i < circlePoints; ++i)
  {
    float radius = 0.25f + (rawData[i] / 15.0f);
    float theta = (M_PI / 2) + (M_PI / float(circlePoints) * i);
    points.push_back(radius * cos(theta)); //x
    points.push_back(radius * sin(theta)); //y
    pointsColors.push_back(1.0f);
    pointsColors.push_back(0.0f);
    pointsColors.push_back(0.0f);
  }
  std::for_each(audioAnalyzer::rangedBegin(data, spectrumRange::lowMidrange), audioAnalyzer::rangedEnd(data, spectrumRange::upperMidrange), [&rawData](audioPoint point) { rawData.push_back(point.getRight()); });
  for (unsigned i = 0; i < circlePoints; ++i)
  {
    float radius = 0.25f + (rawData[circlePoints - 1 - i] / 15.0f);
    float theta = (M_PI / 2) + M_PI + (M_PI / float(circlePoints) * i);
    points.push_back(radius * cos(theta)); //x
    points.push_back(radius * sin(theta)); //y
    pointsColors.push_back(1.0f);
    pointsColors.push_back(0.0f);
    pointsColors.push_back(0.0f);
  }

  glGenBuffers(1, &simpleVBOPosition);
  glBindBuffer(GL_ARRAY_BUFFER, simpleVBOPosition);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glGenBuffers(1, &simpleVBOColor);
  glBindBuffer(GL_ARRAY_BUFFER, simpleVBOColor);
  glBufferData(GL_ARRAY_BUFFER, pointsColors.size() * sizeof(float), &pointsColors[0], GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);*/

//void circleSpectrumScene::render()
//{
/*  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  simpleShader.use();
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glLineWidth(4.0f);
  glDrawArrays(GL_LINE_LOOP, 0, circlePoints * 2);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);*/
//}
