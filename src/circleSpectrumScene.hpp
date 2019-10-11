#ifndef CIRCLESPECTRUMSCENE_H_
#define CIRCLESPECTRUMSCENE_H_

#include "gl.hpp"
#include "audioStream.h"

class circleSpectrumScene
{
public:
  circleSpectrumScene();
  virtual ~circleSpectrumScene();

  void render();
  void update(const audioStream& stream);
private:
  /*GLuint simpleVBOPosition;
  GLuint simpleVBOColor;
  GLuint simpleVAO;
  GLProgram simpleShader;*/
  unsigned circlePoints;
};

#endif
