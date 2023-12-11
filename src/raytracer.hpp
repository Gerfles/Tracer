// Raytracer class

#pragma once
#include <string>
#include <vector>
#include "FreeImage.h"
#include "common.hpp"
#include "camera.hpp"
#include "glm/vec3.hpp"
#include "light.h"
#include "geometry.hpp"




class RayTracer
{
    // variables
 private:
    // general scene descriptions
   int width;
   int height;
   int  maxDepth = 5;
   vec3 currDiffuse{0,0,0};
   vec3 currSpecular{0,0,0};
   vec3 currEmission{0,0,0};
   float currShininess{0};
   std::string output;
   Camera mCamera;
    // scene objects
   std::vector<Surface*> objects;
    // Lights
   std::vector<Light> lights;
   float mConstAtten = 1;
   float mLinearAtten = 0;
   float mQuadAtten = 0;
    // TODO make sure brace initialization gives same results
    // TODO make sure to set defualt
    //TODO find out difference between rgbquad and RGBTRIPLE
   vec3 mCurrAmbient{0.2, 0.2, 0.2};// = vec3(0.2, 0.2, 0.2);

    // methods
 public:
    //friend class Triangle;
   enum class Materials
   {
     diffuse,
     specular,
     emission,
     shininess,
   };
   
   void init(char* filename);
    // SETer methods
   void setDimensions(int w, int h) { width = w; height = h; }
   void setDepth(int depth) { maxDepth = depth; }
   void setOutput(const std::string out) {output = out;}
   void initCamera(float* params) { mCamera.initCamera(params, width, height); }
   void addTriangle(vec3 v1, vec3 v2, vec3 v3);
   void addSphere(vec3 position, float radius, mat4 transform);
   void addLight(vec3 coordinates, vec3 color, bool isPointLight);
   void setAttenuation(float constAtten, float linearAtten, float quadAtten)
    	{ mConstAtten = constAtten; mLinearAtten = linearAtten; mQuadAtten = quadAtten; }
   void setAmbient(vec3 color) { mCurrAmbient = color; }
   void setMaterial(vec3& property, Materials type);
   bool castRay(Ray& currRay, Hit& hit);
   bool isInShadow(Ray& shadowRay, float t0, float t1);
   vec3 shadeRay(Ray& currRay, float t0, float t1, int iteration);
   void lightingModel(Ray& currRay, vec3& color, Hit& hit, int iteration);
   void run();
   ~RayTracer();
};


