
#include "raytracer.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "readfile.hpp"
#include "glm/glm.hpp"
#include "FreeImage.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <cstddef>
#include <limits>
#include <algorithm>

#include "glm/gtx/vector_angle.hpp"



// read in the scene specifics into the raytracer
void RayTracer::init(char* filename)
{
  std::cout << "\n\nReading File Input: " << filename << std::endl;

  readFile(filename, *this);

  if (output.length() == 0)
  {
    output = changeFileExt(filename, ".png");
  }

  std::cout << "output filename: " << output << std::endl;
  std::cout << "number of objects in scene: " << objects.size() << std::endl;
  std::cout << "number of light in scene: " << lights.size() << std::endl;
  std::cout << "Max recursion depth: " << maxDepth << std::endl;
  std::cout << "Scene Initialized!\n";
}



void RayTracer::addTriangle(vec3 v1, vec3 v2, vec3 v3)
{
   // add the currently set light specs to the object
   //TODO make sure delete is implemented correctly
  Triangle* triangle = new Triangle{mCurrAmbient, currDiffuse, currSpecular, currEmission, currShininess};  
  
   // add all vertices (indexes into vert vector) specified in counter clockwise direction
  triangle->addVertices(v1, v2, v3);

   // finally add the triangle to scene objects 
  objects.push_back(triangle);
}



void RayTracer::addSphere(vec3 position, float radius, mat4 transform)
{
  Sphere* sphere = new Sphere{mCurrAmbient, currDiffuse, currSpecular, currEmission, currShininess};

  sphere->init(position, radius, transform);
  objects.push_back(sphere);
}



void RayTracer::addLight(vec3 coordinates, vec3 color, bool isPointLight)
{
  Light light{coordinates, color, isPointLight};

  lights.push_back(light);
}


void RayTracer::setMaterial(vec3& property, Materials type)
{
  switch (type)
  {
      case Materials::diffuse:
        currDiffuse = property;
        break;
        
      case Materials::specular:
        currSpecular = property;
        break;

      case Materials::emission:
        currEmission = property;
        break;

      case Materials::shininess:
        currShininess = property.r;
        break;

      default:
        break;
  }
}



void RayTracer::run()
{
  FreeImage_Initialise();
  FIBITMAP* bitmap = FreeImage_Allocate(width, height, 24);
  vec3 currColor;
  RGBQUAD rgbFormat;
  Hit hit; // record the hit details where the ray intersects the object

  //TODO check that freeimage is initialised etc.
  
   // for each pixel - trace the primary (eye) ray and find intersection
   //TODO better probably to precalc progress marks and then conditionally output progress when we hit a mark
  int progress{0};
  std::cout << "\n\t\t\t - Running Raytracer -" << std::endl;
  std::cout << "|_________________________________________________"
            <<"___________________________________________________|\n ";  

  Ray currRay;
  
   // iterate over all columns of pixels
  for (int column = 0; column < width; column++)
  {
     // show the progress of the raytracer since it can sometimes tage forever
    if (progress < (column * 100.0 / width))
    {
      std::cout << "*" << std::flush;
      progress++;
    }

     // iterate over each individual pixel in the column
    for (int pixel = 0; pixel < height; pixel++)
    {
       // generate the ray from the camera throught the pixel and see if it intersects scene objects
      currRay = mCamera.calcRay(column, pixel);
      currColor = shadeRay(currRay, 0, std::numeric_limits<float>::infinity(), 0);

       // convert the vec3 to rgb and color the actual image pixel by pixel
      rgbFormat = vec3toRGB(currColor);
      FreeImage_SetPixelColor(bitmap, column, height - pixel, &rgbFormat);
    }
  }
  std::cout << std::endl;

   // output the bitmap as a png image  
  FreeImage_Save(FIF_PNG, bitmap, output.c_str());

   // Make sure to free up freeImage resources
  FreeImage_DeInitialise();
}


vec3 RayTracer::shadeRay(Ray& currRay, float t0, float t1, int iterCount)
{
   //?? why define color as 0 here, wouldn't want to add
  vec3 color{0.0f, 0.0f, 0.0f};
  
   // first check if we are beyond the max Iterations allowed
  if (iterCount > maxDepth)
  {
    return color;
  }

  Hit hit;
  hit.t = t1;
  
   // iterate of all objects in scene to see if the ray intersects with any of them
  for (Surface* surf : objects)
  {
     // check to see if we hit ANY object during the scene intersection tests 
    if(surf->calcIntersect(currRay, hit, t0))
    {
       // hit.t is auto updated within calcIntersect()
      hit.surface = surf;
    }
  }

   // check to see if we actually hit any objects, if so update location & color
  if (hit.t < t1)
  { 
    hit.location = currRay.evaluate(hit.t);
    lightingModel(currRay, color, hit, iterCount);
  }

  return color;
}



// similar to shade Ray but in this case we immediately terminate the loop w/ any intersection
bool RayTracer::isInShadow(Ray& shadowRay, float t0, float t1)
{
  Hit dummyHit;
  dummyHit.t = t1;
  
   // iterate of all objects in scene to see if the ray intersects with any of them
  for (Surface* surf : objects)
  {
    if(surf->calcIntersect(shadowRay, dummyHit, t0))
    {
      return true;
    }
  }
   // else, ray doesn't collide with any objects so we are not in shadow 
  return false;
}



//TODO try to eliminate some of the normalization calls
void RayTracer::lightingModel(Ray& currRay, vec3& color, Hit& hit, int iterCount)
{
  // first set color
  color += hit.surface->mAmbient + hit.surface->mEmission;

   // the amount to move the ray origin toward the light, etc in order to avoid inprecision
  float epsilon = .001f;
  
  for (auto light : lights)
  {
    vec3 eyeDir = glm::normalize(currRay.origin - hit.location);
    vec3 lightDir;
    float lightDist;
    float lightAtten;
    
     // first determine if light is directional or point light
    if (light.isPointLight)
    { // point Light
      lightDir = (light.position - hit.location);
      lightDist = glm::length(lightDir);
      lightAtten = 1.0f / (mConstAtten + mLinearAtten * lightDist + mQuadAtten * lightDist * lightDist);
      lightDir = glm::normalize(lightDir);
    }
    else
    {  // light is directional light (infinite distance away)
      lightAtten = 1;
      lightDist = std::numeric_limits<float>::infinity();
      lightDir = glm::normalize(light.position);
    }

     // first cast a ray from intersection point to the light to determine if any objects in the way
    Ray shadowRay;
     // I don't understand why the the following doesn't work...
     // shadowRay.origin = hit.location;
     // ... if (!isInShadow(shadowRay, epsilon, lightDist))
    shadowRay.origin = hit.location + epsilon * lightDir;
    shadowRay.direction = lightDir;
    
    if (!isInShadow(shadowRay, epsilon, lightDist))
    {
       // compute the half-angle vector
      vec3 halfAngle = glm::normalize(lightDir + eyeDir);
      float NdotH = glm::dot(hit.normal, halfAngle);
      float NdotL = glm::dot(hit.normal, lightDir);
      
       // calculate the lambertian  and phong contribution
       color += light.color * lightAtten * hit.surface->mDiffuse * std::max(NdotL, 0.0f);
      
       // calculate the phong contribution
      if (hit.surface->specular() != 0)
      {
        color += light.color * lightAtten * hit.surface->mSpecular
                 * std::pow(std::max(NdotH, 0.0f), hit.surface->mShininess);
      }
    }
  }
   // after all lights accounted for, recursively cast rays to include "mirror-like" reflectivity
  if (hit.surface->specular() != 0)
  {
    Ray reflectedRay;
    reflectedRay.direction = currRay.direction
                             - (2.0f * hit.normal * glm::dot(currRay.direction, hit.normal));
    reflectedRay.direction = glm::normalize(reflectedRay.direction);
     // I don't understand why the the following doesn't work...
     // reflectedRay.origin = hit.location;
     // color += hit.surface->mSpecular * shadeRay(reflectedRay, epsilon, ...
    reflectedRay.origin = hit.location + epsilon * reflectedRay.direction;
         
    color += hit.surface->mSpecular * shadeRay(reflectedRay, 0
                                               , std::numeric_limits<float>::infinity(), ++iterCount);
  }
}



RayTracer::~RayTracer()
{
  for (auto obj : objects)
  {
    delete obj;
  }
}
