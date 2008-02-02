/*
Copyright (c) 2005-2008 Lode Vandevenne
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

#include "lpi_gl.h"

#include <vector>

namespace lpi
{

int screenWidth()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[2] contains the width in pixels of the viewport
  return array[2];
}

int screenHeight()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[3] contains the height in pixels of the viewport
  return array[3];
}

namespace
{
  //these values are in OpenGL viewport coordinates, that is NOT the same as pixel coordinates, use setScissor to properly set these
  std::vector<int> clipLeft;
  std::vector<int> clipTop;
  std::vector<int> clipRight;
  std::vector<int> clipBottom;
  
  int screenMode = -1;
  double lastNear, lastFar;
}

void set2DScreen(int w, int h)
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
void set3DScreen(double near, double far, int w, int h)
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
  
  glMatrixMode(GL_MODELVIEW); //make sure nothing else changed the projection matrix, which may be used only for the projection, not the camera.
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

void set2DScreen()
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  set2DScreen(array[2], array[3]);
}

void set3DScreen(double near, double far)
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  set3DScreen(near, far, array[2], array[3]);
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

bool onScreen(int x, int y)
{
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //get viewport size from OpenGL
  return (x >= 0 && y >= 0 && x < array[2] && y < array[3]);
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
  GLint array[4];
  glGetIntegerv(GL_VIEWPORT, array); //array[3] contains the height in pixels of the viewport
  glScissor(clipLeft[0], array[3] - clipBottom[0], clipRight[0] - clipLeft[0], clipBottom[0] - clipTop[0]);
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

} //namespace lpi
