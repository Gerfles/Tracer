
#pragma once

#include <iostream>
#include <stack>
#include "FreeImage.h"
#include "glm/glm.hpp"

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;


const float pi = 3.14159265;
// helper function for debugging
void printVec(const char* str, const glm::vec3& vec);
void rightmultiply(const mat4& M, std::stack<mat4>& transfstack);
BYTE floatToByte(float num);
RGBQUAD vec3toRGB(vec3& color);
void transformVec3(vec3& vec, mat4& transform);
void transformPoint (vec3& vec, mat4& transform);
std::string changeFileExt(char* filename, std::string newExt);

//RGBQUAD operator+=(RGBQUAD& color1, vec3& color2);
