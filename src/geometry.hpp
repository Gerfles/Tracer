#pragma once

#include "FreeImage.h"
#include <glm/vec3.hpp>
#include "common.hpp"
#include <vector>

using vec3 = glm::vec3;

class Hit;

struct Ray
{
    //Doesn't seem to be needed
   vec3 origin;
   vec3 direction;
   Ray() = default;
   Ray(vec3 origin, vec3 direction) : origin{origin}, direction{direction} {}
   // void setDir(vec3 direction) { this->direction = direction; }
   // void setOrigin(vec3 origin) { this->origin = origin; }
   vec3 evaluate(float t) { return origin + direction * t; };
   friend Ray operator*(const mat4 transform, const Ray ray);
};

 // base class for object primitives
class Surface
{
 public:
   float size; // ?? Need?
    // Lighting characteristics
   vec3 mAmbient;
    // materials
   vec3 mDiffuse;
   vec3 mSpecular;
   vec3 mEmission;
   float mShininess;

    // TODO clean up and get rid of getter functions if all public
// public:
   Surface(vec3 ambient, vec3 diffuse, vec3 specular, vec3 emission, float shininess)
     : mAmbient{ambient}, mDiffuse{diffuse}, mSpecular{specular}
     , mEmission{emission}, mShininess{shininess} {};
   vec3* ambient() { return &mAmbient; }
   vec3* diffuse() { return &mDiffuse; }
   vec3* specular() { return &mSpecular; }
   vec3* emission() { return &mEmission; }
   float shininess() { return mShininess; }
   virtual bool calcIntersect(Ray& ray, Hit& hit, float minT) = 0; // pure virtual
   virtual ~Surface() {};
   
};



class Triangle : public Surface
{
 private:
   
   vec3 normal;
   vec3 mV1, mV2, mV3;
   using Surface::Surface;
    // !! Note we don't need the below since we are using a using declaration!!
    // Triangle(vec3 diffuse, vec3 specular, vec3 emission, float shininess)
    //   : Surface(diffuse, specular, emission, shininess){};
 public:
   void addVertices(const vec3 v1, const vec3 v2, const vec3 v3);
   bool calcIntersect(Ray& ray, Hit& hit, float minT) override;
   bool calcBarycentric(Ray& ray, Hit& hit);
   bool calcBarycentric2(Ray& ray, Hit& hit, float t);
    ~Triangle() {};
};


class Sphere : public Surface
{
 private:
   vec3 mPosition;
   float mRadius;
   mat4 mTransform;
   mat4 mInvTransform;
   using Surface::Surface; //?? Not sure why this is in the private section (as above)
 public:
   bool calcIntersect(Ray& ray, Hit& hit, float minT) override;
   void init(vec3 position, float radius, mat4 transform);
   ~Sphere() {};
};



struct Hit
{
   Surface* surface;
   vec3 location;
    //?? Might not need the t variable if we have a location
   float t; // parameterization variable where ray intersects surface
   vec3 normal; // the normal at the point or the barycentric coordinates of hit
   vec3 baryCentric;
    // ?? Not sure if glm::vec can be initialzed the way normal is?
   Hit() : surface{nullptr}, t{0}, normal{0} {};
};

