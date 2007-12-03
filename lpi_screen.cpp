/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_screen.h"

#include <iostream>

namespace lpi
{

void initBuiltInFontTextures(); //link time dependency to init the built in font textures
namespace gui { void initBuiltInGuiTextures(); } //link time dependency to init the built in gui textures

namespace
{
  SDL_Surface *scr; //the single SDL surface used
  int w; //width of the screen
  int h; //height of the screen
  //these values are in OpenGL viewport coordinates, that is NOT the same as pixel coordinates, use setScissor to properly set these
  std::vector<int> clipLeft;
  std::vector<int> clipTop;
  std::vector<int> clipRight;
  std::vector<int> clipBottom;
  bool fullscreenMode; //if true, it's fullscreen
}

/*
This function sets up an SDL window ready for OpenGL graphics.
You can choose the resolution, whether or not it's fullscreen, and a caption for the window.
*/
void screen(int width, int height, bool fullscreen, const char* text)
{
  int colorDepth = 32;
  w = width;
  h = height;

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    printf("Unable to init SDL: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }
  atexit(SDL_Quit);
  
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  if(fullscreen)
  {
    scr = SDL_SetVideoMode(width,height,colorDepth,SDL_FULLSCREEN|SDL_OPENGL);
    lock();
    fullscreenMode = 1;
  }
  else
  {
    scr = SDL_SetVideoMode(width,height,colorDepth,SDL_OPENGL);
    fullscreenMode = 0;
  }
  if(scr == 0)
  {
    printf("Unable to set video: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }
  SDL_WM_SetCaption(text, NULL);
  
  initGL();
  
  SDL_EnableUNICODE(1); //for the text input things
  
  cls();
  
  //initialize the scissor area (the values at position std::vector.size() - 1 in the std::vectors must ALWAYS be set to the values below and may never be changed!)
  clipLeft.clear();
  clipLeft.push_back(0);
  clipTop.clear();
  clipTop.push_back(0);
  clipRight.clear();
  clipRight.push_back(w);
  clipBottom.clear();
  clipBottom.push_back(h);
  
  //plane.create(RGB_Black, w, h);
  
  initBuiltInFontTextures();
  gui::initBuiltInGuiTextures();
}

int screenWidth()
{
  return w;
}

int screenHeight()
{
  return h;
}

int screenMode = -1;
double lastNear, lastFar;

void set2DScreen()
{
  if(screenMode == 0) return;
  screenMode = 0;
  
  //the official code for "Setting Your Raster Position to a Pixel Location" (i.e. set up an oldskool 2D screen)
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  
  enableTwoSided(); //important, without this, 2D stuff might not be drawn if only one side is enabled
  disableZBuffer();
  disableSmoothing();
}

/*
The modelview matrix isn't touched by this function, if you want to reset that one too, do

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

before or after calling this function.
*/
void set3DScreen(double near, double far)
{
  if(screenMode == 1 && near == lastNear && far == lastFar) return;
  screenMode = 1;
  
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
 
  /*
  multiplying with matrix glup does the same as "gluPerspective(90, double(w)/double(h), near, far);"
  to support other fovy values than 90, replace the value 1.0 in elements 0 and 5 to "1.0 / tan(fovy / 2)", where fovy is in radians
  */
  double aspect = double(w) / double(h);
  double glup[16] = {1.0 / aspect,   0,                               0,  0,
                                0, 1.0,                               0,  0,
                                0,   0,     (far + near) / (near - far), -1,
                                0,   0, (2 * far * near) / (near - far),  0};
  glMultMatrixd(glup);
  
  //flip the z axis because my camera code assumes the z axis flipped compared to opengl
  double flipz[16] = {1,  0,  0,  0,
                      0,  1,  0,  0,
                      0,  0, -1,  0,
                      0,  0,  0,  1};
  glMultMatrixd(flipz);
  
  lastNear = near;
  lastFar = far;
}

//Initialize OpenGL: set up the camera and settings to emulate 2D graphics
void initGL()
{
  set2DScreen();
  
  //glShadeModel(GL_FLAT); //shading, don't do the GL_FLAT thing or gradient rectangles don't work anymore
  //glCullFace(GL_BACK); //culling
  //glFrontFace(GL_CCW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  
  glEnable(GL_SCISSOR_TEST); //scissoring is used to, for example, not draw parts of textures that are scrolled away, and is always enabled (but by default the scissor area is as big as the screen)
  
}

void enableOneSided()
{
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);
}

void enableTwoSided()
{
  glDisable(GL_CULL_FACE);
}


void enableZBuffer()
{
  glEnable(GL_DEPTH_TEST);
}

void disableZBuffer()
{
  glDisable(GL_DEPTH_TEST);
}

bool smoothing_gl = false;
void enableSmoothing()
{
  smoothing_gl = true;
}

void disableSmoothing()
{
  smoothing_gl = false;
}

bool enabledSmoothing()
{
  return smoothing_gl;
}

//set new scissor area (limited drawing area on the screen)
void setScissor(int left, int top, int right, int bottom)
{
  if(right < left) right = left;
  if(bottom < top) bottom = top;
  
  //the values stored in the std::vectors are transformed to opengl viewport coordinates, AND right and bottom actually contain the size instead of coordinates (because OpenGL works that way)
  clipLeft.insert(clipLeft.begin(), left);
  clipTop.insert(clipTop.begin(), top);
  clipRight.insert(clipRight.begin(), right);
  clipBottom.insert(clipBottom.begin(), bottom);
  
  setOpenGLScissor();
}

void setSmallestScissor(int left, int top, int right, int bottom)
{
  int smallestLeft = left;
  int smallestTop = top;
  int smallestRight = right;
  int smallestBottom = bottom;
  
  
  if(clipLeft[0] > smallestLeft) smallestLeft = clipLeft[0]; //de meest rechtse van de linkerzijden
  if(clipTop[0] > smallestTop) smallestTop = clipTop[0]; //de laagste van de top zijden
  if(clipRight[0] < smallestRight) smallestRight = clipRight[0]; //de meest linkse van de rechtse zijden
  if(clipBottom[0] < smallestBottom) smallestBottom = clipBottom[0]; //de hoogste van de bodem zijden
  
  //if(smallestLeft < smallestRight) smallestLeft = 0, smallestRight = 1;
  //if(smallestTop < smallestBottom) smallestTop = 0, smallestBottom = 1;
  
  
  setScissor(smallestLeft, smallestTop, smallestRight, smallestBottom);
}

//uses the extern scissor area variables to set the scissoring area of OpenGL
void setOpenGLScissor()
{
  glScissor(clipLeft[0], h - clipBottom[0], clipRight[0] - clipLeft[0], clipBottom[0] - clipTop[0]);
}

//reset the scissor area back to the previous coordinates before your last setScissor call (works like a stack)
void resetScissor()
{
  if(clipLeft.size() > 1)
  {
    clipLeft.erase(clipLeft.begin());
    clipTop.erase(clipTop.begin());
    clipRight.erase(clipRight.begin());
    clipBottom.erase(clipBottom.begin());
  }
  
  setOpenGLScissor();
}

//Locks the screen
void lock()
{
  if(SDL_MUSTLOCK(scr))
  if(SDL_LockSurface(scr) < 0)
  return;
}

//Unlocks the screen
void unlock()
{
  if(SDL_MUSTLOCK(scr))
  SDL_UnlockSurface(scr);
}

//make the content that you drew to the backbuffer visible by swapping the buffers
void redraw()
{
  SDL_GL_SwapBuffers();
}

//clear the screen to black again
void cls(const ColorRGB& color)
{
  glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, 0);  //the clear color
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

bool onScreen(int x, int y)
{
  return (x >= 0 && y >= 0 && x < w && y < h);
}

} //end of namespace lpi
