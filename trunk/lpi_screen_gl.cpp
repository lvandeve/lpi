/*
Copyright (c) 2005-2010 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lpi_screen_gl.h"
#include "lpi_os.h"

#include <GL/gl.h>
#include <iostream>
#include <cstdlib> //std::exit

namespace lpi
{

/*
This function sets up an SDL window ready for OpenGL graphics.
You can choose the resolution, whether or not it's fullscreen, and a caption for the window.
This also inits everything else of the lpi application, so lpi::screen currently acts as init function.
*/
ScreenGL::ScreenGL(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages)
: screenMode(-1)
, print_debug_messages(print_debug_messages)
, glsmoothing(false)
{
  w = width;
  h = height;

  if(print_debug_messages) std::cout<<"info: initing SDL"<<std::endl;
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cout << "Error: Unable to init SDL: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(1);
  }
  if(print_debug_messages) std::cout<<"info: SDL inited"<<std::endl;
  
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  if(!setVideoMode(width, height, fullscreen, enable_fsaa, resizable, text, print_debug_messages))
  {
    SDL_Quit();
    std::exit(1);
  }
  
  if(print_debug_messages)
  {
    const SDL_version* sdl_version = SDL_Linked_Version();

    std::cout<< "info: SDL Version: " << (int)sdl_version->major << "." << (int)sdl_version->minor << "." << (int)sdl_version->patch << std::endl;
    std::cout<< "info: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout<< "info: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout<< "info: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
  }
  
  if(print_debug_messages) std::cout<<"info: initing GL"<<std::endl;
  initGL();
  
  GLenum err = glGetError();
  if(err == GL_NO_ERROR && print_debug_messages) std::cout<<"info: GL inited"<<std::endl;
  while(err != GL_NO_ERROR)
  {
    std::cout<<"Error initing GL: OpenGL glGetError() = " << err << std::endl;
    err = glGetError();
  }
  
  
  SDL_EnableUNICODE(1); //for the text input things //TODO: move this to SDL input class
  
  cls();
  
  //plane.create(RGB_Black, w, h);
}

ScreenGL::~ScreenGL()
{
  if(print_debug_messages) std::cout << "info: Quitting SDL" << std::endl;
  SDL_Quit();
  if(print_debug_messages) std::cout << "info: SDL Quit" << std::endl;
}

bool ScreenGL::setVideoMode(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages)
{
  int colorDepth = 32;
  
  //FSAA
  if(enable_fsaa)
  {
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 ) ;
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 ) ;
  }
  else
  {
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0) ;
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 0) ;
  }
  
  Uint32 flags = SDL_OPENGL;
  if(resizable) flags |= SDL_RESIZABLE;
  if(fullscreen) flags |= SDL_FULLSCREEN;
  

  if(print_debug_messages) std::cout<<"info: initing screen"<<std::endl;
  scr = SDL_SetVideoMode(width, height, colorDepth, flags);
  if(scr == 0 && enable_fsaa)
  {
    std::cout << "Warning: FSAA failed. Trying without." << std::endl;
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0) ;
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 0) ;
    scr = SDL_SetVideoMode(width, height, colorDepth, flags);
  }
  if(scr == 0)
  {
    w = 800;
    h = 600;
    scr = SDL_SetVideoMode(800, 600, colorDepth, flags);
    if(scr != 0) std::cout << "Warning: Creating screen with desired resolution failed, created with 800*600 instead." << std::endl;
  }
  if(scr == 0)
  {
    std::cout << "Error: Unable to set video. SDL error message: " << SDL_GetError() << std::endl;
    return false;
  }
  if(print_debug_messages) std::cout<<"info: screen inited"<<std::endl;
  if(fullscreen) lock();
  SDL_WM_SetCaption(text, NULL);

  if(!context.isActive())
  {
    context.onNewGLContext();
  }
  else
  {
#if defined(LPI_OS_WINDOWS) //if the opengl context is destroyed by SDL setVideoMode
  //since SDL version 1.2.14, this is not needed anymore and the behaviour is the same as on Linux: GL context not destroyed by calling SDL_SetVideoMode after a resize event!
  //context.onNewGLContext();
#endif
  }
  
  return true;
}

void ScreenGL::changeResolution(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages)
{
  w = width;
  h = height;
  
  setVideoMode(width, height, fullscreen, enable_fsaa, resizable, text, print_debug_messages);
  
  if(print_debug_messages) std::cout<<"info: initing GL"<<std::endl;
  glViewport(0, 0, w, h);
  screenMode = -1;
  initGL();
  if(print_debug_messages) std::cout<<"info: GL inited"<<std::endl;
  
  //cls();
}

//Locks the screen
void ScreenGL::lock()
{
  if(SDL_MUSTLOCK(scr))
  if(SDL_LockSurface(scr) < 0)
  return;
}

//Unlocks the screen
void ScreenGL::unlock()
{
  if(SDL_MUSTLOCK(scr))
  SDL_UnlockSurface(scr);
}

//clear the screen to given color again
void ScreenGL::cls(const ColorRGB& color)
{
  glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, 0);  //the clear color
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//make the content that you drew to the backbuffer visible by swapping the buffers
void ScreenGL::redraw()
{
  SDL_GL_SwapBuffers();
}

int ScreenGL::screenWidth()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[2] contains the width in pixels of the viewport
  return array[2];
}

int ScreenGL::screenHeight()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[3] contains the height in pixels of the viewport
  return array[3];
}

void ScreenGL::set2DScreen(int w, int h, bool filledGeometry)
{
  //Displacemenet for exact pixelation. This requires some more experimentation.
  //Different values may be needed for thin geometry, filled geometry and textures
  //Values to try: 0.0; 0.5; 0.375
  static const double TWIDDLEX = 0.375;
  static const double TWIDDLEY = 0.375;

  if(filledGeometry)
  {
    if(screenMode == 1) return;
    if(screenMode == 0) { glTranslated(-TWIDDLEX, -TWIDDLEY, 0); screenMode = 1; return; }
    screenMode = 1;
  }
  else
  {
    if(screenMode == 0) return;
    if(screenMode == 1) { glTranslated(TWIDDLEX, TWIDDLEY, 0); screenMode = 0; return; }
    screenMode = 0;
  }

  //the official code for "Setting Your Raster Position to a Pixel Location" (i.e. set up an oldskool 2D screen)
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if(!filledGeometry)
  {
    glTranslated(TWIDDLEX, TWIDDLEY, 0);
  }

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
void ScreenGL::set3DScreen(double near, double far, int w, int h)
{
  if(screenMode == 2 && near == lastNear && far == lastFar) return;
  screenMode = 2;
  
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
  
  glMatrixMode(GL_MODELVIEW); //make sure nothing else changed the projection matrix, which may be used only for the projection, not the camera.
}

//Initialize OpenGL: set up the camera and settings to emulate 2D graphics
void ScreenGL::initGL()
{
  set2DScreen(false);
  
  //glShadeModel(GL_FLAT); //shading, don't do the GL_FLAT thing or gradient rectangles don't work anymore
  //glCullFace(GL_BACK); //culling
  //glFrontFace(GL_CCW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  
  glEnable(GL_SCISSOR_TEST); //scissoring is used to, for example, not draw parts of textures that are scrolled away, and is always enabled (but by default the scissor area is as big as the screen)
  
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  
  //initialize the scissor area (the values at position std::vector.size() - 1 in the std::vectors must ALWAYS be set to the values below and may never be changed!)
  clipLeft.clear();
  clipLeft.push_back(0);
  clipTop.clear();
  clipTop.push_back(0);
  clipRight.clear();
  clipRight.push_back(array[2]);
  clipBottom.clear();
  clipBottom.push_back(array[3]);
}

void ScreenGL::set2DScreen(bool filledGeometry)
{
  /*
  This check is an optimization to avoid the glGetIntegerv call, but, it's very strange:
  in Windows, it uses a LOT more CPU if this check is done. So disable it. This check
  is also done in the other set2DScreen that's being called, and there it increases
  performance. But why does calling glGetIntegerv here make it faster on Windows even
  though the returned info isn't actually used??
  */
  //if(filledGeometry && screenMode == 1) return;
  //if(!filledGeometry && screenMode == 0) return;

  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  set2DScreen(array[2], array[3], filledGeometry);
}

void ScreenGL::set3DScreen(double near, double far)
{
  if(screenMode == 2) return;
  
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  set3DScreen(near, far, array[2], array[3]);
}

void ScreenGL::enableOneSided()
{
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);
}

void ScreenGL::enableTwoSided()
{
  glDisable(GL_CULL_FACE);
}


void ScreenGL::enableZBuffer()
{
  glEnable(GL_DEPTH_TEST);
}

void ScreenGL::disableZBuffer()
{
  glDisable(GL_DEPTH_TEST);
}

bool ScreenGL::onScreen(int x, int y)
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  return (x >= 0 && y >= 0 && x < array[2] && y < array[3]);
}

//set new scissor area (limited drawing area on the screen)
void ScreenGL::setScissor(int left, int top, int right, int bottom)
{
  if(right < left) right = left;
  if(bottom < top) bottom = top;
  
  //the values stored in the std::vectors are transformed to opengl viewport coordinates, AND right and bottom actually contain the size instead of coordinates (because OpenGL works that way)
  clipLeft.push_back(left);
  clipTop.push_back(top);
  clipRight.push_back(right);
  clipBottom.push_back(bottom);
  
  setOpenGLScissor();
}

void ScreenGL::setSmallestScissor(int left, int top, int right, int bottom)
{
  int smallestLeft = left;
  int smallestTop = top;
  int smallestRight = right;
  int smallestBottom = bottom;
  
  
  if(clipLeft.back() > smallestLeft) smallestLeft = clipLeft.back(); //de meest rechtse van de linkerzijden
  if(clipTop.back() > smallestTop) smallestTop = clipTop.back(); //de laagste van de top zijden
  if(clipRight.back() < smallestRight) smallestRight = clipRight.back(); //de meest linkse van de rechtse zijden
  if(clipBottom.back() < smallestBottom) smallestBottom = clipBottom.back(); //de hoogste van de bodem zijden
  
  //if(smallestLeft < smallestRight) smallestLeft = 0, smallestRight = 1;
  //if(smallestTop < smallestBottom) smallestTop = 0, smallestBottom = 1;
  
  
  setScissor(smallestLeft, smallestTop, smallestRight, smallestBottom);
}

//uses the extern scissor area variables to set the scissoring area of OpenGL
void ScreenGL::setOpenGLScissor()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[3] contains the height in pixels of the viewport
  glScissor(clipLeft.back(), array[3] - clipBottom.back(), clipRight.back() - clipLeft.back(), clipBottom.back() - clipTop.back());
}

//reset the scissor area back to the previous coordinates before your last setScissor call (works like a stack)
void ScreenGL::resetScissor()
{
  if(clipLeft.size() > 1)
  {
    clipLeft.pop_back();
    clipTop.pop_back();
    clipRight.pop_back();
    clipBottom.pop_back();
  }
  
  setOpenGLScissor();
}

void ScreenGL::enableSmoothing()
{
  glsmoothing = true;
}

void ScreenGL::disableSmoothing()
{
  glsmoothing = false;
}

bool ScreenGL::isSmoothingEnabled()
{
  return glsmoothing;
}

GLContext* ScreenGL::getGLContext()
{
  return &context;
}

} //end of namespace lpi
