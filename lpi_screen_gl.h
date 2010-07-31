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

#pragma once

#include <SDL/SDL.h>
#include <vector>

#include "lpi_color.h"
#include "lpi_gl_context.h"

namespace lpi
{

class ScreenGL
{
  private:
    SDL_Surface* scr; //the single SDL surface used
    int w; //width of the screen
    int h; //height of the screen
    
    std::vector<int> clipLeft;
    std::vector<int> clipTop;
    std::vector<int> clipRight;
    std::vector<int> clipBottom;

    int screenMode; //-1: uninited, 0: 2d thin geometry, 1: 2d filled geometry & textures, 2: 3d
    double lastNear, lastFar;
    
    GLContext context;

    bool print_debug_messages;

  private:
    void lock();
    void unlock();

    void initGL();
    void set2DScreen(int w, int h, bool filledGeometry);
    void set3DScreen(double near, double far, int w, int h);

    bool glsmoothing;
    
    bool setVideoMode(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages);

    
  public:
    ScreenGL(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages=false);
    ~ScreenGL();
    
    void cls(const ColorRGB& color = RGB_Black);
    void redraw();
  
    void set2DScreen(bool filledGeometry);
    void set3DScreen(double near, double far);
  
    int screenWidth();
    int screenHeight();
    bool onScreen(int x, int y);

    void setScissor(int left, int top, int right, int bottom);
    void setSmallestScissor(int left, int top, int right, int bottom); //same as setScissor, but will new scissor area will be inside the old scissor area, all parts outside are removed
    void setOpenGLScissor();
    void resetScissor();

    void enableOneSided();
    void enableTwoSided();
    void enableZBuffer();
    void disableZBuffer();
    //smoothing: to draw the pixels of the texture filtered or not
    void enableSmoothing();
    void disableSmoothing();
    bool isSmoothingEnabled();
    
    void changeResolution(int width, int height, bool fullscreen, bool enable_fsaa, bool resizable, const char* text, bool print_debug_messages=false);
    
    GLContext* getGLContext();
};


} //namespace lpi

