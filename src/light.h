// light.h


#pragma once

#include "common.hpp"
#include "glm/vec3.hpp"

class Light
{
 public:
   bool isPointLight;
   vec3 position;
   vec3 color;
    // TODO probably should include attenuation specs in here

   Light() = delete;     // make sure we can't construct an empty light
   Light(glm::vec3 position, glm::vec3 color, bool isPointLight):
     position{position}, color{color}, isPointLight{isPointLight} {}
    //void initLight(glm::vec3 position, glm::vec3 color);
};
