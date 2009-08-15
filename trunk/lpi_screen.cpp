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

#include "lpi_screen.h"

#include <iostream>
#include <cstdlib> //std::exit

namespace lpi
{

namespace
{
  SDL_Surface* scr; //the single SDL surface used
  int w; //width of the screen
  int h; //height of the screen
  bool fullscreenMode; //if true, it's fullscreen
}

/*
This function sets up an SDL window ready for OpenGL graphics.
You can choose the resolution, whether or not it's fullscreen, and a caption for the window.
This also inits everything else of the lpi application, so lpi::screen currently acts as init function.
*/
void screen(int width, int height, bool fullscreen, const char* text)
{
  int colorDepth = 32;
  w = width;
  h = height;

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    std::cout << std::string("Unable to init SDL: ") + SDL_GetError();
    SDL_Quit();
    std::exit(1);
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
    scr = SDL_SetVideoMode(width, height, colorDepth, SDL_FULLSCREEN | SDL_OPENGL);
    lock();
    fullscreenMode = 1;
  }
  else
  {
    scr = SDL_SetVideoMode(width, height, colorDepth, SDL_OPENGL);
    fullscreenMode = 0;
  }
  if(scr == 0)
  {
    std::cout << std::string("Unable to set video: ") + SDL_GetError();
    SDL_Quit();
    std::exit(1);
  }
  SDL_WM_SetCaption(text, NULL);
  
  initGL();
  
  SDL_EnableUNICODE(1); //for the text input things
  
  cls();
  
  //plane.create(RGB_Black, w, h);
}

/*int screenWidth()
{
  return w;
}

int screenHeight()
{
  return h;
}*/

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

//clear the screen to given color again
void cls(const ColorRGB& color)
{
  glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, 0);  //the clear color
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//make the content that you drew to the backbuffer visible by swapping the buffers
void redraw()
{
  SDL_GL_SwapBuffers();
}

} //end of namespace lpi
