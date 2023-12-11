// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "transform.hpp"
// #include "glm/detail/type_mat.hpp"
// #include "glm/detail/type_vec.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <iostream>



// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis)
{
   vec3 rotAxis = glm::normalize(axis);
   // Method 1
   float c = glm::cos(degrees * pi / 180.0);
   float s = glm::sin(degrees * pi / 180.0);
   float oneMinusC = 1 - c;

   mat3 rot( c + oneMinusC * rotAxis.x * rotAxis.x
             , oneMinusC * rotAxis.x * rotAxis.y + s * rotAxis.z
             , oneMinusC * rotAxis.x * rotAxis.z - s * rotAxis.y
             , oneMinusC * rotAxis.x * rotAxis.y - s * rotAxis.z
             , c + oneMinusC * rotAxis.y * rotAxis.y
             , oneMinusC * rotAxis.y * rotAxis.z + s * rotAxis.x
             , oneMinusC * rotAxis.x * rotAxis.z + s * rotAxis.y
             , oneMinusC * rotAxis.y * rotAxis.z - s * rotAxis.x
             , c + oneMinusC * rotAxis.z * rotAxis.z);

  return rot;
}

void Transform::left(float degrees, vec3& eye, vec3& up)
{
   // get the matrix to rotate the object (or framework)
  mat3 transform = rotate(degrees, up);

   // transform the eye vector (up doesnt need transform since it's the axis of rotation)
  eye = transform * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up)
{
   // determine axis of rotation
  vec3 axis = glm::cross(eye, up);

   // get the matrix to rotate the object (or framework)
  mat3 transform = rotate(degrees, axis);

   // transform the camera vectors
  eye = transform * eye;
  up = transform * up;
   //  printVec(axis, "axis");
}

mat4 Transform::lookAt(const vec3& eye, const vec3& center, const vec3& up)
{
   // implementation 1 
  vec3 w = eye / sqrt(glm::dot(eye, eye));
  vec3 u = glm::cross(up, w);
   // same as glm::normalize(u);
  u = u / sqrt(glm::dot(u, u));
  vec3 v = glm::cross(w, u);

   // Method 1
  mat4 lookat = glm::mat4( u.x, v.x, w.x, 0,
                           u.y, v.y, w.y, 0,
                           u.z, v.z, w.z, 0,
                           -u.x * eye.x - u.y * eye.y - u.z * eye.z,
                           -v.x * eye.x - v.y * eye.y - v.z * eye.z,
                           -w.x * eye.x - w.y * eye.y - w.z * eye.z,
                           1);

  // // Method 2
  // mat4 lookat = glm::mat4( u.x, v.x, w.x, 0,
  //                          u.y, v.y, w.y, 0,
  //                          u.z, v.z, w.z, 0,
  //                          - glm::dot(u, eye),
  //                          - glm::dot(v, eye),
  //                          - glm::dot(w, eye),
  //                          1);

   // return the OpenGL lookat matrix 
  return lookat;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
   // default matrix inits to Identity so make sure to initialize with 0
  mat4 perspectiveMatrix(0);

  // ?? not sure how best to compute cotangent but converting to radians (pi / 180 degrees)
  float zoom = 1.0 / tan(fovy * pi / 360.0);
  float denom = - 1.0 / (zFar - zNear);
  
  perspectiveMatrix[0][0] = zoom / aspect;
  perspectiveMatrix[1][1] = zoom;
  perspectiveMatrix[2][2] = (zFar + zNear) * denom;
  perspectiveMatrix[2][3] = -1.0;
  perspectiveMatrix[3][2] = 2.0 * zNear * zFar * denom;

  return perspectiveMatrix;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
   // first create the identity matrix
  glm::mat4 scale(1.0);

   // now set only the important elements
  scale[0][0] = sx;
  scale[1][1] = sy;
  scale[2][2] = sz;

  return scale;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
   // initialize the translate matrix to identity
  glm::mat4 translate(1.0);

  translate[3][0] = tx;
  translate[3][1] = ty;
  translate[3][2] = tz;

  return translate;
}

// To normalize the up direction and construct a coordinate frame.
// As discussed in the lecture.  May be relevant to create a properly
// orthogonal and normalized up.
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.
vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
