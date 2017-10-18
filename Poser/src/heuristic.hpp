#ifndef _HEUR_H_
#define _HEUR_H_
#include "macros.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "createShader.hpp"
#include "render.hpp"
#include "model.hpp"
#include "initialCalibration.hpp"

void boxfocal(  point original,
              point &rect,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 M,
              glm::mat4 V,
              glm::mat4 P,
              glm::mat4 initialT,
              glm::mat4 initialRot,
              glm::mat4 &initialS,
              int width,
              int height,
              std::vector< unsigned char >& frameImage
            );

#endif // _HEUR_H_
