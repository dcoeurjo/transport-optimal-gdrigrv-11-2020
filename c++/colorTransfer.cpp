/*
 Copyright (c) 2020 CNRS
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <string>
#include <random>
#include <vector>
//Command-line parsing
#include "CLI11.hpp"

//Image filtering and I/O
#define cimg_display 0
#include "CImg.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//Global flag to silent verbose messages
bool silent;


int main(int argc, char **argv)
{
  CLI::App app{"colorTransfer"};
  std::string sourceImage;
  app.add_option("-s,--source", sourceImage, "Source image")->required()->check(CLI::ExistingFile);;
  std::string targetImage;
  app.add_option("-t,--target", targetImage, "Target image")->required()->check(CLI::ExistingFile);;
  std::string outputImage= "output.png";
  app.add_option("-o,--output", outputImage, "Output image")->required();
  unsigned int nbSteps = 3;
  app.add_option("-n,--nbsteps", nbSteps, "Number of sliced steps (3)");
  silent = false;
  app.add_flag("--silent", silent, "No verbose messages");
  CLI11_PARSE(app, argc, argv);
  
  //Image loading
  int width,height, nbChannels;
  unsigned char *source = stbi_load(sourceImage.c_str(), &width, &height, &nbChannels, 0);
  if (!silent) std::cout<< "Source image: "<<width<<"x"<<height<<"   ("<<nbChannels<<")"<< std::endl;
  int width_target,height_target, nbChannels_target;
  unsigned char *target = stbi_load(targetImage.c_str(), &width_target, &height_target, &nbChannels_target, 0);
  if (!silent) std::cout<< "Target image: "<<width_target<<"x"<<height_target<<"   ("<<nbChannels_target<<")"<< std::endl;
  if ((width*height) != (width_target*height_target))
  {
    std::cout<< "Image sizes do not match. "<<std::endl;
    exit(1);
  }
  if (nbChannels < 3)
  {
    std::cout<< "Input images must be RGB images."<<std::endl;
    exit(1);
  }
   
  //Main computation
  std::vector<unsigned char> output(width*height*nbChannels);
  
  //As an example, we just scan the pixels of the source image
  //and swap the color channels.
  for(auto i = 0 ; i < width ; ++i)
  {
    for(auto j = 0; j < height; ++j)
    {
      auto indexPixel = nbChannels*(width*j+i);
      unsigned char r = source[ indexPixel ];
      unsigned char g = source[ indexPixel + 1];
      unsigned char b = source[ indexPixel + 2];
      //Swapping the channels
      output[ indexPixel ] = b;
      output[ indexPixel + 1 ] = g;
      output[ indexPixel + 2 ] = r;
      if (nbChannels == 4) //just copying the alpha value if any
        output[ indexPixel + 3] = source[ indexPixel + 3];
    }
  }
  
  //Final export
  if (!silent) std::cout<<"Exporting.."<<std::endl;
  int errcode = stbi_write_png(outputImage.c_str(), width, height, nbChannels, output.data(), nbChannels*width);
  if (!errcode)
  {
    std::cout<<"Error while exporting the resulting image."<<std::endl;
    exit(errcode);
  }
  
  stbi_image_free(source);
  stbi_image_free(target);
  exit(0);
}
