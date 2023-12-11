  
// TODO consider overloading this function to work with different shapes
#include "geometry.hpp"
#include "common.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <limits>



Ray operator*(mat4 const transform, const Ray ray)
{
  vec4 o;
  vec4 dir;

  o = transform * vec4(ray.origin.x, ray.origin.y, ray.origin.z, 1.0f);
  dir = transform * vec4(ray.direction.x, ray.direction.y, ray.direction.z, 0.0f);

  Ray transRay = ray;
  transRay.origin.x = o.x;
  transRay.origin.y = o.y;
  transRay.origin.z = o.z;

  transRay.direction.x = dir.x;
  transRay.direction.y = dir.y;
  transRay.direction.z = dir.z;

  transRay.direction = glm::normalize(transRay.direction);
  
  return transRay;
}



void Triangle::addVertices(const vec3 v1, const vec3 v2, const vec3 v3)
{
  mV1 = v1;
  mV2 = v2;
  mV3 = v3;

   // calculate the normal and store in triangle
  normal = glm::cross(mV2 - mV1, mV3 - mV1);
  normal = glm::normalize(normal);
}



bool Triangle::calcIntersect(Ray& ray, Hit& hit, float minT)
{
   // determine at what point (if any) the Ray intersects the plane containing the triangle first
  float denom = glm::dot(normal, ray.direction);

   // check before proceeding that the denominator isn't 0 -> meaning the ray is tangent to triangle
  if (denom == 0)
  {
    return false;
  }
    
  float numer = glm::dot(mV1, normal) - glm::dot(ray.origin, normal);
  float t = numer / denom;

   // terminate early if we get a negative distance or t is outside the interval (0, maxdistance)
  if (t > minT && t < hit.t)
  {
     // make sure the intersection is not just in the plane but actually within the triangle
    if (calcBarycentric2(ray, hit, t))
    {
      hit.normal = normal;
      hit.t = t;
      return true;
    }
  }

   // ray only hit the plane (but not within the triangle) or else didn't intersect with anything
  return false;
}


// TODO implement the solution for barycentric from scribe-lecture1.pdf
bool Triangle::calcBarycentric2(Ray& ray, Hit& hit, float t)
{
  
   // calculate the intersection point of ray and plane at time t
  vec3 iPoint = ray.evaluate(t);
  
   // solution from
  float detT = (mV1.x - mV3.x) * (mV2.y - mV3.y) - (mV2.x - mV3.x) * (mV1.y - mV3.y);

  if (detT == 0)
  {
    return false;
  }
  
  float oneOverDet = 1.0f / detT;
  float alpha = ((iPoint.x - mV3.x) * (mV2.y - mV3.y) - (iPoint.y - mV3.x) * (mV2.x - mV3.x)) * oneOverDet;

  if (alpha < 0 || alpha > 1) {
    return false;
  }
  
  float beta = ((iPoint.y - mV3.y) * (mV1.x - mV3.x) - (iPoint.x - mV3.x) * (mV1.y - mV3.y)) * oneOverDet;
  float gama = 1 - alpha - beta;

  if (beta < 0 || gama < 0) {
    return false;
  }

   // store the alpha, beta and gama into the normBary vector in hit
  hit.baryCentric.x = alpha;
  hit.baryCentric.y = beta;
  hit.baryCentric.z = gama;

  return true;  
}




//  
// calculate the barycentric coordinates of a point on a plane and whether
// within a triangle return false if point is outside of triangle
bool Triangle::calcBarycentric(Ray& ray, Hit& hit)
{
   // solution from Marschner & Shirley
  float eiMinusHf = (mV1.y - mV3.y) * ray.direction.z - (mV1.z - mV3.z) * ray.direction.y;
  float gfMinusDi = (mV1.z - mV3.z) * ray.direction.x - (mV1.x - mV3.x) * ray.direction.z;
  float dhMinusEg = (mV1.x - mV3.x) * ray.direction.y - (mV1.y - mV3.y) * ray.direction.x;
  float akMinusJb = (mV1.x - mV2.x) * (mV1.y - ray.origin.y) - (mV1.x - ray.origin.x) * (mV1.y - mV2.y);
  float jcMinusAl = (mV1.x - ray.origin.x) * (mV1.z - mV2.z) - (mV1.x - mV2.x) * (mV1.z - ray.origin.z);
  float blMinusKc = (mV1.y - mV2.y) * (mV1.z - ray.origin.z) - (mV1.y - ray.origin.y) * (mV1.z - mV2.z);

  float oneOverM = (mV1.x - mV2.x) * eiMinusHf + (mV1.y - mV2.y) * gfMinusDi + (mV1.z - mV2.z) * dhMinusEg;
  oneOverM = 1.0 / oneOverM;

   // can compute t in this algorithm to save computation but better to first find t and see it it's
   // worth it If the scene is particularly rich with triangles, this is probably the better
   // solution but not if its sparse
   // float t = (mV1.z - mV3.z) * akMinusJb + (mV1.y - mV3.y) * jcMinusAl + (mV1.x - mV3.x) * blMinusKc;
   // t = -t * oneOverM;

  float gama = (ray.direction.z * akMinusJb + ray.direction.y * jcMinusAl + ray.direction.x * blMinusKc) * oneOverM;

  if (gama < 0 || gama > 1) {
    return false;
  }

  float beta = ((mV1.x - ray.origin.x) * eiMinusHf + (mV1.y - ray.origin.y) * gfMinusDi
                + (mV1.z - ray.origin.z) * dhMinusEg) * oneOverM;

  if (beta < 0 || beta + gama > 1) {
    return false;
  }

   // store the alpha, beta and gama into the normBary vector in hit
  hit.baryCentric.x = 1 - beta - gama;
  hit.baryCentric.y = beta;
  hit.baryCentric.z = gama;
  
  return true;  
}



void Sphere::init(vec3 position, float radius, mat4 transform)
{
  mPosition = position;
  mRadius = radius;
  mTransform = transform;
  mInvTransform = glm::inverse(transform);
}


// intersection equation: (D . D)t^2 + [D . (e - C)]2*t +(e - C) . (e - C) - r^2 = 0
// where D = ray direction, e = ray origin, C = circle center, r = radius
bool Sphere::calcIntersect(Ray& ray, Hit& hit, float minT)
{
   // first transform the ray by the inverse of the sphere transform
  Ray transRay = mInvTransform * ray;

   // first find the coefficients for the quadratic equation
  float a = glm::dot(transRay.direction, transRay.direction);
  float b = 2.0f * glm::dot(transRay.direction, transRay.origin - mPosition);
  float c = glm::dot(transRay.origin - mPosition, transRay.origin - mPosition) - mRadius * mRadius;

   // if we have real roots and if not return (no intersection)
  float discr = b * b - 4.0f * a * c;
  if (discr < 0)
  { 
    return false;
  }

   // determine both roots of the quadratic equation
  float t1, t2;
  if (discr == 0)
  { // booth roots are the same so ray is tangential to the sphere
    t1 = -b / (2.0f * a);
    t2 = -1.0f;
  }
  else
  {
     // More robust version of the quadratic equation solves depending on the sign of b to avoid
     // subtracting nearly equal floating-point numbers (in order to avoid loss of precision)
    if (b > 0)
    {
      float negB_minusRad = -b - std::sqrt(discr);
      t1 = 2.0f * c / (negB_minusRad);
      t2 = (negB_minusRad) / (2.0f * a);
    }
    else
    {
      float negB_plusRad = -b + std::sqrt(discr);      
      t1 = 2.0f * c / (negB_plusRad);
      t2 = (negB_plusRad) / (2.0f * a);
    }
  }

  float closestT = std::numeric_limits<float>::infinity();
  
  if (t1 < 0)
  { 
    if (t2 > 0)
    { // t1 is negative and t2 isn't so that's our solution
      closestT = t2;
    }
    else
    { // both t1 & t2 negative so no solutions
      return false;
    }
  }
  else
  { // t2 is negative and t1 isn't so that's our solution
    if (t2 < 0)
    {
      closestT = t1;
    }
    else
    { // t1 & t2 are both positive so choose the lesser of the two
      closestT = (t1 < t2) ? t1 : t2;
    }
  }
   // now must transform the point derived by instersecting a transformed ray with a "pre-transformed" sphere
  vec3 intersectPoint = transRay.evaluate(closestT);

   // calculate the normal before transforming the intersection point, since we want to transform the normal later
  vec3 trxNormal = intersectPoint - mPosition;

   // now transform that point into "real world" coordinates and solve for T in the original equation
  transformPoint(intersectPoint, mTransform);
  closestT = (intersectPoint.x - ray.origin.x) / ray.direction.x;

   // only return true if un-transformed point is within range parameters
  if (closestT < minT || closestT > hit.t)
  {
    return false;
  }
  else
  {
     // transform it but since it may contain non-uniform scale must transform appropriately
    mat4 invTranspose = glm::transpose(mInvTransform);
    transformVec3(trxNormal, invTranspose);

     // update hit and return true (intersection)
    hit.normal = glm::normalize(trxNormal);
    hit.t = closestT;
    return true;
  }
}

