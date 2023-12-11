// A raytracer program that uses the following syntax:
//
//
#include <iostream>
#include <FreeImage.h>
#include "raytracer.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: hw3 scenefile\n";
    exit(-1);
  }

  RayTracer rayTracer;
  
   // initilize the raytracer scenefile
  rayTracer.init(argv[1]);
  
   // compute the image 
  rayTracer.run();

  return 0;
}
