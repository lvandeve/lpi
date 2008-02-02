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

#include "lpi_event.h"
#include "lpi_general.h"


namespace lpi
{

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


SDL_Event event = {0}; //the events such as input, windows events, ...

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
  if(key < 0 || key > 1024) return 0; //the state only remembers 1024 keys...
  
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
  if(key < 0 || key > 1024) return 0; //the state only remembers 1024 keys...
    
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
allowedChars disables or enables some characters such as numbers, special symbols, ...

If allowedChars is 0, all symbols are enabled
The following bits disable the following:
&1: disable punctuation symbols like ,(?.:
&2: disable numbers
&4: disable letters

Note that this function can also return things like when you pressed backspace, ...
*/

char previousChar[16];
float keyTime[16];
bool warmed_up[16];

Uint32 getTicks(); //defined in another .cpp file (link time dependency)

int unicodeKey(int allowedChars, int warmup, int rate, int index)
{
  if(index < 0) index = 0;
  if(index > 15) index %= 16;
  
  int asciiChar = 0;
  if ((event.key.keysym.unicode & 0xFF80) == 0)
  {
    if(event.type == SDL_KEYDOWN)
    {
      int inputChar = event.key.keysym.unicode & 0x7F;
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
      if(inputChar == previousChar[index])
      {
        asciiChar = 0;
        
        //if waited long enough, asciiChar can be set to inputChar anyway!
        if(getTicks() - keyTime[index] > warmup && !warmed_up[index])
        {
          keyTime[index] = getTicks();
          warmed_up[index] = 1;
        }
        else if(getTicks() - keyTime[index] > rate && warmed_up[index])
        {
          previousChar[index] = inputChar;
          asciiChar = inputChar;
          keyTime[index] = getTicks();
        }

      }
      else
      {
        previousChar[index] = inputChar;
        asciiChar = inputChar;
        keyTime[index] = getTicks();
        warmed_up[index] = 0;
      }
      
      
      
    }
    else
    previousChar[index] = 0; //so that you CAN press the same key twice in a row if you release it!
  }
  
  if(allowedChars & 1)
  {
    if(asciiChar < 0) asciiChar = 0; //signed extended ascii symbols
    if(asciiChar < 32 && asciiChar != 13 && asciiChar != 8) asciiChar = 0; //<32 ones, except enter and backspace
    if(asciiChar < 48 && asciiChar >= 32) asciiChar = 0; //before the numbers
    if(asciiChar > 57 && asciiChar < 65) asciiChar = 0; //between numbers and capitals
    if(asciiChar > 90 && asciiChar < 97) asciiChar = 0; //between capitals and small letters
    if(asciiChar > 122) asciiChar = 0; //after the small letters
  }
  if(allowedChars & 2)
  {
    if(asciiChar >= 47 && asciiChar <= 57) asciiChar = 0; //disable numbers too
  }
  if(allowedChars & 4)
  {
    if(asciiChar >= 65 && asciiChar <= 90) asciiChar = 0; //capitals
    if(asciiChar >= 97 && asciiChar <= 122) asciiChar = 0; //small letters
  }
  //disable unexpected special symbols except enter and backspace
  if(asciiChar < 32 && asciiChar != 13 && asciiChar != 8) asciiChar = 0; //<32 ones, except enter and backspace
  //some special ascii chars if you press left control with a keypad key
  readKeys();
  if(inkeys[SDLK_LCTRL] && !(allowedChars & 1))
  switch(asciiChar)
  {
    case '0': asciiChar = 128; break; //de franse c-dit
    case '1': asciiChar = 137; break; //e with trema: � zoals in Belgi�
    case '2': asciiChar = 129; break; //u met 2 puntjes 
    case '3': asciiChar = 164; break; //n wiht ~, like the spanish se�or 
    case '4': asciiChar = 168; break; //upside down question mark
    case '5': asciiChar = 173; break; //upside down exclamation mark
    case '6': asciiChar = 224; break; //alpha 
    case '7': asciiChar = 234; break; //large omega or ohm
    case '8': asciiChar = 227; break; //pi
    case '9': asciiChar = 230; break; //mu or micro
    case '/': asciiChar = 127; break; //pentagram
    case '*': asciiChar = 236; break; //infinity 
    case '-': asciiChar = 3; break; //heart
    case '+': asciiChar = 241; break; // +/-
    case '.': asciiChar = 1; break; //smiley
  }  
  return asciiChar;
}


//Waits until you press a key. First the key has to be loose, this means, if you put two sleep functions in a row, the second will only work after you first released the key.
void sleep()
{
  int done = 0;
  while(done == 0)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT) end();
      if(event.type == SDL_KEYDOWN) done = 1;
    }
    SDL_Delay(5); //so it consumes less processing power
  }
}


/*
Returns 1 if you close the window or press the escape key
Also gets events and key input, so if you have done() in a loop it'll take
care of events as well.
*/
bool frame(bool quit_if_esc, bool delay) //delay makes CPU have some free time, use once per frame to avoid 100% usage of a CPU core
{
  checkMouse(); //not needed for the frame() function, but this way your loop automaticly checks the mouse every frame!
  
  if(delay) SDL_Delay(5); //so it consumes less processing power

  int done = 0;
  if(!SDL_PollEvent(&event)) return 1;
  inkeys.readKeys();
  if(quit_if_esc && inkeys[SDLK_ESCAPE]) done = 1;
  if(event.type == SDL_QUIT) done = 1;
  
  globalMouseWheelUp = globalMouseWheelDown = false;
  if(event.type == SDL_MOUSEBUTTONDOWN)
  {
    if(event.button.button == SDL_BUTTON_WHEELUP) globalMouseWheelUp = true;
    if(event.button.button == SDL_BUTTON_WHEELDOWN) globalMouseWheelDown = true;
  }
  
  return !done;
}

//Ends the program
void end()
{
  SDL_Quit();
  std::exit(1);
}

} //namespace lpi

