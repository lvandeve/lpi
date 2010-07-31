/*
Copyright (c) 2005-2009 Lode Vandevenne
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

#include "lpi_event.h"
#include <cstdlib>
#include <iostream>
#include <vector>


namespace lpi
{

KeyState::KeyState()
{
  for(size_t i = 0; i < NUM; i++)
  {
    keyReleased[i] = false;
    keyWarmedUp[i] = false;
    lastTime[i] = 0;
  }
  
  singlePrevious = 0;
  singleKeyTime = 0;
  singleWarmedUp = false;
}

class Inkeys //this class contains which keys are pressed (need to do readKeys to get updated state)
{
  private:
  Uint8* keybuffer;
  int size; //the amount of keys filled in inkeys
  
  public:
  void readKeys();
  
  Inkeys();
  
  bool operator[](int index);
};

Inkeys inkeys; //they keys that are pressed

void Inkeys::readKeys()
{
  keybuffer = SDL_GetKeyState(&size);
}

Inkeys::Inkeys()
{
  readKeys();
}

bool Inkeys::operator[](int index)
{
    return keybuffer[index];
}


std::vector<SDL_Event> events;

/*
Gives value of pressed key to inkeys.
the variable inkeys can then be used anywhere to check for input.
Normally you have to use readkeys every time you want to use inkeys, but the
done() function also uses inkeys so it's not needed to use readkeys if you have 
a gameloop that uses done().

This function must be done at least once BEFORE ever trying to use inkeys, or you
will get an invalid result
*/
void readKeys()
{
  inkeys.readKeys();
}

namespace
{
  KeyState static_state;
}

//this returns true for a key only if it's pressed for the first time, pressing means "for the first time" while "down" would mean that they key is down at that moment
//the state is there to be able to have different tests that don't screw up each other's values: give them a different state
bool keyPressed(int key, KeyState* state) //use the SDL key code here, like SDLK_KP7 for keypad 7
{
  if(key < 0 || key > (int)KeyState::NUM) return 0; //the state only remembers KeyState::NUM keys...
  
  if(!state) state = &static_state;
  
  if(inkeys[key] && state->keyReleased[key])
  {
    state->keyReleased[key] = 0;
    return 1;
  }
  if(!inkeys[key]) state->keyReleased[key] = 1;
  return 0;
}

bool keyPressedTime(int key, double time, double warmupTime, double repTime, KeyState* state) //use the SDL key code here, like SDLK_KP7 for keypad 7
{
  if(key < 0 || key > (int)KeyState::NUM) return false; //the state only remembers KeyState::NUM keys...
    
  if(!state) state = &static_state;
  
  if(inkeys[key])
  {
    if(state->keyReleased[key])
    {
      state->keyReleased[key] = false;
      state->keyWarmedUp[key] = false;
      state->lastTime[key] = time;
      return true;
    }
    else if(!state->keyWarmedUp[key])
    {
      if(time - state->lastTime[key] > warmupTime)
      {
        state->lastTime[key] = time;
        state->keyWarmedUp[key] = true;
        return true;
      }
      else return false;
    }
    else
    {
      if(time - state->lastTime[key] > repTime)
      {
        state->lastTime[key] = time;
        return true;
      }
      else return false;
    }
  }
  else state->keyReleased[key] = true;
  return false;
}

bool keyDown(int key)
{
  readKeys();
  if(inkeys[key]) return 1;
  else return 0;
}

bool pressedEnter(KeyState* state)
{
  if(keyPressed(SDLK_RETURN, state) || keyPressed(SDLK_KP_ENTER, state)) return 1;
  else return 0;
}


int globalMouseX;
int globalMouseY;
bool globalLMB;
bool globalRMB;
bool globalMMB;
//mouse wheel state is checked in the done function in api.cpp because it can only checked with an sdl event
bool globalMouseWheelUp; //mouse wheel up
bool globalMouseWheelDown; //mouse wheel down

//get position and pressed buttons of the mouse, LMB=left mouse button, RMB=right mouse button
void checkMouse()
{
  Uint8 mouseState = SDL_GetMouseState(&globalMouseX, &globalMouseY);
  
  if(mouseState & 1) globalLMB = true; 
  else globalLMB = false;
  if(mouseState & 4) globalRMB = true; 
  else globalRMB = false;
  if(mouseState & 2) globalMMB = true; 
  else globalMMB = false;
  //NOTE: scroll wheel is handled in "frame" function
}


int lastMouseX = -1, lastMouseY = -1;
/*
returns difference between current and last mouse x position, can be used only by one thing since calling it resets last position
this function doesn't call checkMouse so something else has already got to do it to make this work
*/
int mouseXDiff()
{
  int result = 0;
  if(lastMouseX >= 0) result = globalMouseX - lastMouseX;
  lastMouseX = globalMouseX;
  return result;
}

//idem for y direction
int mouseYDiff()
{
  int result = 0;
  if(lastMouseY >= 0) result = globalMouseY - lastMouseY;
  lastMouseY = globalMouseY;
  return result;
}

//same, but warps mouse back to old position every time
int mouseXDiffWarp()
{
  int result = 0;
  if(lastMouseX < 0) lastMouseX = globalMouseX;
  else
  {
    result = globalMouseX - lastMouseX;
    SDL_WarpMouse(lastMouseX, globalMouseY);
    globalMouseX = lastMouseX;
  }
  return result;
}

//idem for y direction
int mouseYDiffWarp()
{
  int result = 0;
  if(lastMouseY < 0) lastMouseY = globalMouseY;
  else
  {
    result = globalMouseY - lastMouseY;
    SDL_WarpMouse(globalMouseX, lastMouseY);
    globalMouseY = lastMouseY;
  }
  return result;
}

void resetMouseDiffFunctions()
{
  lastMouseX = lastMouseY = -1;
}

/*
This function returns which unicode key you're pressing, to type.
Note that this function can also return things like when you pressed backspace, ...
The intention of this function is to be used for controls in which you can type text.
*/

int unicodeKey(double time, double warmupTime, double repTime, KeyState* state)
{
  if(!state) state = &static_state;
  
  SDL_Event event = {0};
  bool event_found = false;
  for(size_t i = 0; i < events.size(); i++)
  {
    if((events[i].key.keysym.unicode & 0xFF80) == 0 && (events[i].type == SDL_KEYDOWN || events[i].type == SDL_KEYUP))
    {
      event_found = true;
      event = events[i];
      events.erase(events.begin() + i);
      break;
    }
  }
  
  int asciiChar = 0;

  static int sym = -1; //TODO: move this into KeyState?
  if(event_found && event.type == SDL_KEYDOWN)
  {
    sym = event.key.keysym.unicode;
  }
  if(event_found && event.type == SDL_KEYUP)
  {
    sym = -1;
  }
  
  if(sym > 0)
  {
    int inputChar = sym & 0x7F;
    //handle numpad too, because it's not included in SDL's unicode thingie
    if(asciiChar == 0)
    {
      int keyPadTest = event.key.keysym.sym;
      switch(keyPadTest)
      {
        case SDLK_KP0: inputChar = '0'; break;
        case SDLK_KP1: inputChar = '1'; break;
        case SDLK_KP2: inputChar = '2'; break;
        case SDLK_KP3: inputChar = '3'; break;
        case SDLK_KP4: inputChar = '4'; break;
        case SDLK_KP5: inputChar = '5'; break;
        case SDLK_KP6: inputChar = '6'; break;
        case SDLK_KP7: inputChar = '7'; break;
        case SDLK_KP8: inputChar = '8'; break;
        case SDLK_KP9: inputChar = '9'; break;
        case SDLK_KP_PLUS: inputChar = '+'; break;
        case SDLK_KP_MINUS: inputChar = '-'; break;
        case SDLK_KP_MULTIPLY: inputChar = '*'; break;
        case SDLK_KP_DIVIDE: inputChar = '/'; break;
        case SDLK_KP_PERIOD: inputChar = '.'; break;
      }
    }
    
    //below is the system that prevents typing 100s of times the same char when holding down the key. It uses warmup and rate
    if(inputChar == state->singlePrevious)
    {
      asciiChar = 0;
      
      //if waited long enough, asciiChar can be set to inputChar anyway!
      if(time - state->singleKeyTime > warmupTime && !state->singleWarmedUp)
      {
        state->singleKeyTime = time;
        state->singleWarmedUp = true;
      }
      else if(time - state->singleKeyTime > repTime && state->singleWarmedUp)
      {
        state->singlePrevious = inputChar;
        asciiChar = inputChar;
        state->singleKeyTime = time;
      }
    }
    else
    {
      state->singlePrevious = inputChar;
      asciiChar = inputChar;
      state->singleKeyTime = time;
      state->singleWarmedUp = 0;
    }
    
  }
  else state->singlePrevious = 0; //so that you CAN press the same key twice in a row if you release it!

  
  //disable unexpected special symbols except enter and backspace
  if(asciiChar < 32 && asciiChar != 13 && asciiChar != 8) asciiChar = 0; //<32 ones, except enter and backspace
 
  return asciiChar;
}



//Waits until you press a key. First the key has to be loose, this means, if you put two sleep functions in a row, the second will only work after you first released the key.
void sleep(bool wake_up_on_mouse, bool quit_on_quit)
{
  int done = 0;
  SDL_Event event = {0};
  while(done == 0)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) { if(quit_on_quit) end(); else done = 1; }
      if(event.type == SDL_KEYDOWN) done = 1;
      if(event.type == SDL_MOUSEBUTTONDOWN && wake_up_on_mouse) done = 1;
    }
    SDL_Delay(5); //so it consumes less processing power
  }
}

static bool resize_event_happaned = false;
static int resize_event_happaned_new_size_x = 0;
static int resize_event_happaned_new_size_y = 0;

bool resizeEventHappened()
{
  if(resize_event_happaned)
  {
    resize_event_happaned = false;
    return true;
  }
  else return false;
}

int resizeEventNewSizeX()
{
  return resize_event_happaned_new_size_x;
}

int resizeEventNewSizeY()
{
  return resize_event_happaned_new_size_y;
}

/*
Returns 1 if you close the window or press the escape key
Also gets events and key input, so if you have done() in a loop it'll take
care of events as well.
*/
bool frame(bool quit_if_esc, bool delay) //delay makes CPU have some free time, use once per frame to avoid 100% usage of a CPU core
{
  if(delay) SDL_Delay(5); //so it consumes less processing power
  
  //also do the checking of input (mouse & keyboard) for this frame
  checkMouse();
  inkeys.readKeys();
  globalMouseWheelUp = globalMouseWheelDown = false;
  
  if(quit_if_esc && inkeys[SDLK_ESCAPE]) return false;
  
  SDL_Event event = {0};
  
  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT) return false;
    else if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      if(event.button.button == SDL_BUTTON_WHEELUP) globalMouseWheelUp = true;
      if(event.button.button == SDL_BUTTON_WHEELDOWN) globalMouseWheelDown = true;
    }
    else if(event.type == SDL_VIDEORESIZE)
    {
      resize_event_happaned = true;
      resize_event_happaned_new_size_x = event.resize.w;
      resize_event_happaned_new_size_y = event.resize.h;
    }
    else events.push_back(event);
  }
  
  static const size_t MAX_EVENTS = 16;
  
  if(events.size() > MAX_EVENTS)
  {
    std::vector<SDL_Event> events2(MAX_EVENTS);
    std::copy(events.begin() + events.size() - MAX_EVENTS, events.end(), events2.begin());
    events.swap(events2);
  }
  
  return true; //the program may continue
}

//Ends the program
void end()
{
  SDL_Quit();
  std::exit(1);
}

void setMousePos(int x, int y)
{
  SDL_WarpMouse(x, y);
}

void changeMousePos(int x, int y)
{
  checkMouse();
  SDL_WarpMouse(globalMouseX + x, globalMouseY + y);
}

} //namespace lpi

