
#include "camera.hpp"

#include "glm/glm.hpp"
#include <iostream>

void Camera::initCamera(float* params, int width, int height)
{
  mPosition = vec3(params[0], params[1], params[2]);
  mRay.origin = mPosition;

  vec3 target = vec3(params[3], params[4], params[5]);
  vec3 up = vec3(params[6], params[7], params[8]);

  mFOVY = params[9];

   //TODO develop beter basis vector names
   // construct an ortho-normal basis for the camera
   //?? why wouldn't the direction vector of the camera be target - position
  w = glm::normalize(mPosition - target); //position / sqrt(glm::dot(position, position));
  u = glm::normalize(glm::cross(up, w));
  v = glm::normalize(glm::cross(w, u));
  //?? do we need to normalize v? NO!
  // since the vectors U and W are now orthogonal to each other, there cross product is also a unit vector
  //v = glm::cross(w, u);

   // calculate the ray geometry given the ray params
   // TODO eliminate the degrees to radians confusion () by writting an atan that takes degrees
  float fovX = std::atan(width * std::tan(mFOVY * pi / 360.0) / height) * 360.0f / pi;
  
  a2 = -std::tan(fovX * pi / 360.0f);
  a1 = -2.0f * a2 / width;
  b2 = std::tan(mFOVY * pi / 360.0f);
  b1 = -2.0f * b2 / height;
}


// TODO could save the x and y offset in the ray given the scene dimensions and
// just return multiple of that offset starting from pixel 0;
 // TODO better to not return by value--should check rest of functions too
Ray& Camera::calcRay(int xPixCoord, int yPixCoord)
{
   // first find direction offset based on which pixel we're examining
   // given by Alpha * u + Beta * v - w (where -w is the direction the camera points)
  float alpha = a1 * (xPixCoord + 0.5) + a2;
  float beta = b1 * (yPixCoord - 0.5) + b2;

   // find ray direction (ray equation given by P(t) = e + td: e=origin, d=direction)
  mRay.direction = glm::normalize(alpha * u + beta * v - w);

   //?? this seems bad to return the address to an internal private variable
  return mRay;
}
