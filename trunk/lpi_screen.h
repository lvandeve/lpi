/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef screen_h
#define screen_h

#include <SDL/SDL.h>
#include <vector>


#include <GL/gl.h>

#include "lpi_color.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
//EXTERNAL VARIABLES////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern SDL_Surface *scr; //currently the single SDL surface used
extern int w;
extern int h;

//clip the drawing area of the screen (scissor area) (a stack of them so you can set many clipping areas with the setScissor and resetScissor functions)
//values left, top, ... of parameters below are given in pixel coordinates with origin at top left (but the setOpenGLScissor function will convert them to OpenGL style and with width and height)
extern std::vector<int> clipLeft;
extern std::vector<int> clipRight;
extern std::vector<int> clipTop;
extern std::vector<int> clipBottom;
void setScissor(int left, int top, int right, int bottom);
void setSmallestScissor(int left, int top, int right, int bottom); //same as setScissor, but will new scissor area will be inside the old scissor area, all parts outside are removed
void setOpenGLScissor();
void resetScissor();
bool onScreen(int x, int y);


////////////////////////////////////////////////////////////////////////////////
//BASIC SCREEN FUNCTIONS////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern int screenMode; //0 = 2D screen, 1 = 3D screen, this is used by the set2DScreen and set3DScreen functions to only change when not changed to this mode already

void screen(int width, int height, bool fullscreen, const char* text);
void initGL();
void set2DScreen();
void set3DScreen(double near, double far);
void lock();
void unlock();
void redraw();
void cls(const ColorRGB& color = RGB_Black);

void enableOneSided();
void enableTwoSided();
void enableZBuffer();
void disableZBuffer();
extern bool smoothing_gl; //OpenGL can't remember this
void enableSmoothing();
void disableSmoothing();




} //namespace lpi

#endif //screen_h

