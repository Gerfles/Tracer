#pragma once

#include "glm/vec3.hpp"
#include "common.hpp"
#include "geometry.hpp"

class Camera
{
 private:
    // used to computer the ray direction in terms of pixels
   float a1, a2; // coofficients for X offset of ray
   float b1, b2; // coofficients for Y offset of ray
   vec3 mPosition;
    //TODO don't think we need the below 3 once we have basis vectors and FOV
   // vec3 mDirection;
   // vec3 mTarget;
   // vec3 mUp;
   Ray mRay; // the ray originating at the camera and looking at target (+/- offset)
   vec3 u, v, w; // the orthonormal basis for the camera
   float mFOVY; //?? Note that this is only fov in y direction
 public:
   void initCamera(float* params, int screeWidth, int screeHeight);
   vec3& origin() { return mPosition; }
   vec3& direction(int xOffset, int yOffset);
   Ray& calcRay(int xPixCoord, int yPixCoord);
};
