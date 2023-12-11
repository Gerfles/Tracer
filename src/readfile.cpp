
// libraries
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// classes
#include "FreeImage.h"
#include "readfile.hpp"
#include "raytracer.hpp"
#include "common.hpp"
#include "transform.hpp"


// function to read & parse the input data
bool readVals(std::stringstream &s, const int numVals, float* values)
{
  for (int i = 0; i < numVals; i++)
  {
    s >> values[i];
    if (s.fail())
    {
      std::cout << "Failed reading value " << i << "will skip\n";
      return false;
    }
  }

  return true;
}

// function to read & parse the input data for integers 
bool readVals(std::stringstream &s, const int numVals, int* values)
{
  for (int i = 0; i < numVals; i++)
  {
    s >> values[i];
    if (s.fail())
    {
      std::cout << "Failed reading value " << i << "will skip\n";
      return false;
    }
  }

  return true;
}


void readFile(const char* filename, RayTracer& rt)
{
  
  std::ifstream in;
  in.open(filename);
  if (in.is_open())
  {
     // matrix stack to store transforms.  
    std::stack <mat4> transfstack;
     // place to store any added vertices (storing the post-transformed vertices in triangles for instance)
    std::vector <vec4> vertices;
    
    transfstack.push(mat4(1.0));  // identity

    std::string str, cmd;
    std::getline(in, str);
     // TODO I don't think this is a good way of parsing a file... check 
    while (in)
    {
       // ignore comments and blank lines
      if ((str.find_first_not_of(" \t\r\n") != std::string::npos) && (str[0] != '#'))
      {
        std::stringstream s(str);
        s >> cmd;
         //std::cout << "Command: " << cmd << std::endl;
        float values[10];
        int ints[3];
        int i;
        bool isValidInput;
        int maxVerts;
        
         /*--- Scene Initialization  ---*/
         //------------------------------------------------        
         // screen image size 
        if (cmd == "size")
        {

          isValidInput = readVals(s, 2, ints);
          if (isValidInput)
          {
            rt.setDimensions(ints[0], ints[1]);
          }

          std::cout << "Image size: " << ints[0] << "(width) x " << ints[1] << "(height)\n";
        }
         // maximum depth the raytracer recurses
        else if (cmd == "maxdepth")
        {
          isValidInput = readVals(s, 1, ints);
          if (isValidInput)
          {
            rt.setDepth(ints[0]);
          }
        }

         // TODO should create more robust string proceedures with error check
         // file to save the resulting picture
        else if (cmd == "output")
        {
          std::string outputFile;
          s >> outputFile;
          // ?? might need to add a token take

          rt.setOutput(outputFile);
        }

        
         // the camera with which to view the scene
        else if (cmd == "camera")
        {
          isValidInput = readVals(s, 10, values);
          if (isValidInput)
          {
            rt.initCamera(values);
            transfstack.push(transfstack.top());
            std::cout << "Camera Initialized" << std::endl;
          }
        }
         /*--- Geometry  ---*/
         //------------------------------------------------

         // the maximum amount of vertices (optional)
        else if (cmd == "maxverts")
        {
          isValidInput = readVals(s, 1, ints);
          if (isValidInput)
          {
            vertices.reserve(ints[0]); 
          }
        }
         // vertices
        else if (cmd == "vertex")
        {
          isValidInput = readVals(s, 3, values);
          if (isValidInput)
          {
             //?? Not sure if this should be adding a w component
            vertices.push_back(vec4(values[0], values[1], values[2], 1.0));
          }
        }
         // triangle (defined by indices into vertex vector)
        else if (cmd == "tri")
        {
          isValidInput = readVals(s, 3, ints);
          if (isValidInput)
          {
             // transform triangle and simply store the transformed vertices
            mat4 transform = transfstack.top();

            vec3 v1 = transform * vertices[ints[0]];
            vec3 v2 = transform * vertices[ints[1]];
            vec3 v3 = transform * vertices[ints[2]];
                        
            rt.addTriangle(v1, v2, v3);
          }
        }

         // sphere (defined by a position and radius)
        else if (cmd == "sphere")
        {
          isValidInput = readVals(s, 4, values);
          if (isValidInput)
          {
            rt.addSphere(vec3(values[0], values[1], values[2]), values[3], transfstack.top());
          }
        }

        
         /*--- Transforms  ---*/
         //------------------------------------------------
        else if (cmd == "translate")
        {
          isValidInput = readVals(s,3,values);
          if (isValidInput)
          {
             // do any transforms on the matrix
            mat4 translateMat = Transform::translate(values[0], values[1], values[2]);
            rightmultiply(translateMat, transfstack);
          }
        }
        else if (cmd == "scale")
        {
          isValidInput = readVals(s,3,values); 
          if (isValidInput) {

             // MY CODE FOR HW 2.  
            mat4 scaleMat = Transform::scale(values[0], values[1], values[2]);
            rightmultiply(scaleMat, transfstack);
          }
        }
        else if (cmd == "rotate") {
          isValidInput = readVals(s,4,values); 
          if (isValidInput) {
             // values[0..2] are the axis, values[3] is the angle.  
             // See how the stack is affected, as above.  
             // Note that rotate returns a mat3. 
             // Also keep in mind what order your matrix is!
            vec3 rotationAxis = vec3(values[0], values[1], values[2]);
            mat3 rotationMat = Transform::rotate(values[3], rotationAxis);
            mat4 homoRotationMat = mat4(rotationMat);
            rightmultiply(homoRotationMat, transfstack);
          }
        }

         // I include the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform")
        {
          transfstack.push(transfstack.top());
        }
        else if (cmd == "popTransform")
        {
          if (transfstack.size() <= 1)
          {
            std::cerr << "Stack has no elements.  Cannot Pop\n"; 
          }
          else
          {
            transfstack.pop(); 
          }
        }


        
         /*--- Lights  ---*/
         //------------------------------------------------
        else if (cmd == "directional" || cmd == "point")
        {
          isValidInput = readVals(s, 6, values);
          if (isValidInput)
          {
            bool isPointLight = false;
            if (cmd == "point") isPointLight = true;
            
            rt.addLight(vec3(values[0], values[1], values[2]),
                        vec3(values[3], values[4], values[5]),
                        isPointLight);
          }
        }
        else if (cmd == "attenuation")
        {
          isValidInput = readVals(s, 3, values);
          if (isValidInput)
          {
            rt.setAttenuation(values[0], values[1], values[2]);
          }
        }
        else if (cmd == "ambient")
        {
          isValidInput = readVals(s, 3, values);
          if (isValidInput)
          {
            // RGBQUAD color;
            // color.rgbRed = floatToByte(values[0]);
            // color.rgbGreen = floatToByte(values[1]);
            // color.rgbBlue = floatToByte(values[2]);
            vec3 color{values[0], values[1], values[2]};
            rt.setAmbient(color);
          }
        }

         /*--- Materials  ---*/
         //------------------------------------------------
        else if (cmd == "diffuse")
        {
          isValidInput = readVals(s, 3, values);
          {
            vec3 diffuseLt = vec3(values[0], values[1], values[2]);
            rt.setMaterial(diffuseLt, RayTracer::Materials::diffuse);
          }
        }
        else if (cmd == "specular")
        {
          isValidInput = readVals(s, 3, values);
          {
            vec3 specularLt = vec3(values[0], values[1], values[2]);
            rt.setMaterial(specularLt, RayTracer::Materials::specular);
          }
        }
        else if (cmd == "emission")
        {
          isValidInput = readVals(s, 3, values);
          {
            vec3 emissionLt = vec3(values[0], values[1], values[2]);
            rt.setMaterial(emissionLt, RayTracer::Materials::emission);
          }
        }
        else if (cmd == "shininess")
        {
          isValidInput = readVals(s, 1, values);
          {
            vec3 shininessLt = vec3(values[0], values[1], values[2]);
            rt.setMaterial(shininessLt, RayTracer::Materials::shininess);
          }
        }
      }

      std::getline(in, str);
    }
  }
}
