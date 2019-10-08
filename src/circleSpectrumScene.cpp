#include "circleSpectrumScene.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

const char* vertexShaderSource = R"GLSL(
#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec3 vertexColor;

out vec3 ex_Color;

void main()
{
  gl_Position.xy = vertexPosition;
  gl_Position.zw = vec2(0.0, 1.0);

  ex_Color = vertexColor;
}
)GLSL";
const char* fragmentShaderSource = R"GLSL(
#version 330 core
precision highp float;

in  vec3 ex_Color;
out vec4 fragColor;

void main()
{
  fragColor = vec4(ex_Color, 1.0);
}
)GLSL";

circleSpectrumScene::circleSpectrumScene()
  : simpleVBOPosition(0), simpleVBOColor(0), simpleShader(vertexShaderSource, fragmentShaderSource), circlePoints(0)
{
  glGenVertexArrays(1, &simpleVAO);
}

circleSpectrumScene::~circleSpectrumScene()
{
}

void circleSpectrumScene::update(const audioStream& stream)
{
  glBindVertexArray(simpleVAO);

  const audioAnalyzer::fftSpectrumData& data = stream.getLatestFrame().spectrum;

  circlePoints = audioAnalyzer::rangedSize(spectrumRange::lowMidrange) + audioAnalyzer::rangedSize(spectrumRange::midrange) + audioAnalyzer::rangedSize(spectrumRange::upperMidrange);
  std::vector<float> points;
  std::vector<float> pointsColors;

  auto startIter = audioAnalyzer::rangedBegin(data, spectrumRange::lowMidrange);

  for (unsigned i = 0; i < circlePoints; ++i)
  {
    float radius = 0.25f + (startIter->magnitude / 150.0f);
    float theta = (2 * M_PI) / float(circlePoints) * i;
    points.push_back(radius * cos(theta)); //x
    points.push_back(radius * sin(theta)); //y
    pointsColors.push_back(1.0f);
    pointsColors.push_back(0.0f);
    pointsColors.push_back(0.0f);
    ++startIter;
  }

  glGenBuffers(1, &simpleVBOPosition);
  glBindBuffer(GL_ARRAY_BUFFER, simpleVBOPosition);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), &points[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glGenBuffers(1, &simpleVBOColor);
  glBindBuffer(GL_ARRAY_BUFFER, simpleVBOColor);
  glBufferData(GL_ARRAY_BUFFER, pointsColors.size() * sizeof(float), &pointsColors[0], GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void circleSpectrumScene::render()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  simpleShader.use();
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glLineWidth(4.0f);
  glDrawArrays(GL_LINE_LOOP, 0, circlePoints);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
