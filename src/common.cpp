#include "common.hpp"
#include "FreeImage.h"
#include "camera.hpp"


void printVec(const char* title, const glm::vec3& vector)
{
  std::cout << title <<" vector:\t[";

  for (int i = 0; i < 3; i++)
  {
    std::cout << vector[i] << ", ";

    if (i == 2)
    {
      std::cout << "\b\b]";
    }
  }
  std::cout << std::endl;
}

// TODO determine which method of transform is better
void transformPoint (vec3& vec, mat4& transform)
{
  vec4 temp = vec4(vec.x, vec.y, vec.z, 1.0f);
  temp = transform * temp;
  vec.x = temp.x;
  vec.y = temp.y;
  vec.z = temp.z;
}

void transformVec3 (vec3& vec, mat4& transform)
{
  vec4 temp = vec4(vec.x, vec.y, vec.z, 0.0f);
  temp = transform * temp;
  vec.x = temp.x;
  vec.y = temp.y;
  vec.z = temp.z;
}


void rightmultiply(const mat4 & M, std::stack<mat4> &transfstack) 
{
  mat4 &T = transfstack.top();
  T = T * M;
}


std::string changeFileExt(char* filename, std::string newExt)
{
  std::string fileLessExt;

  for (int i = 0; ; i++)
  {
    char c = filename[i];

    if (c == '.' || c == '\0')
      break;
    
    fileLessExt += c;
  }

  return fileLessExt + newExt;
}


BYTE floatToByte(float num)
{
  return BYTE(num * 255);
}


RGBQUAD vec3toRGB(vec3& color)
{
  RGBQUAD rgb;

  rgb.rgbRed = floatToByte(color.r);
  rgb.rgbGreen = floatToByte(color.g);
  rgb.rgbBlue = floatToByte(color.b);

  return rgb;
}


// RGBQUAD operator+=(RGBQUAD& color1, vec3& color2)
// {
//   color1.rgbGreen += color2.r;
  
// }
