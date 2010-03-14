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

/*
lpi_event: SDL events, mouse input and keyboard input
*/

#pragma once

#include <SDL/SDL.h>

#include "lpi_input.h"

namespace lpi
{

//like it or not, the mouse interface works with global variables. You can even control them yourself for gui unit tests. That is and stays the interface :)
extern int globalMouseX;
extern int globalMouseY;
extern bool globalLMB; //left mouse button
extern bool globalRMB; //right mouse button
extern bool globalMMB; //middle mouse button
extern bool globalMouseWheelUp; //mouse wheel up
extern bool globalMouseWheelDown; //mouse wheel down

inline bool getGlobalMouseButton(MouseButton button)
{
  if(button == LMB) return globalLMB;
  if(button == RMB) return globalRMB;
  if(button == MMB) return globalMMB;
  return 0;
}

void readKeys();

struct KeyState
{
  KeyState();
  
  static const size_t NUM = 1024;
  
  /*
  used for remembering if the keys were held down last frame, for e.g. keyPressed and keyPressedTime
  if you don't give a KeyState to keyPressed or keyPressedTime, it uses a default one
  you want to use a second one if you want to have independent checking of keyPressed
  */
  bool keyReleased[NUM];
  bool keyWarmedUp[NUM]; //for keyPressedTime
  double lastTime[NUM]; //for keyPressedTime; last time it returned true
  
  //for "unicodeKey" (called "single" because it doesn't have to remember a separate one for each key)
  int singlePrevious;
  double singleKeyTime;
  bool singleWarmedUp;
};

bool keyDown(int key);
bool keyPressed(int key, KeyState* state = 0); //only returns true the first time the key is down and you check
bool keyPressedTime(int key, double time, double warmupTime = 0.5, double repTime = 0.025, KeyState* state = 0); //reptime = time to repeat key press. All times in seconds.
bool pressedEnter(KeyState* state = 0);

void checkMouse(); //updates the values from the physical mouse
int mouseXDiff();
int mouseYDiff();
int mouseXDiffWarp();
int mouseYDiffWarp();
void resetMouseDiffFunctions();

void setMousePos(int x, int y);
void changeMousePos(int x, int y);

/*
warmup = "warmup time": how long it takes before the key starts repeating
rate = repetition rate of the key after the warmupTime
having released the key resets the warmup
*/
int unicodeKey(double time, double warmupTime = 0.5, double repTime = 0.025, KeyState* state = 0);

void sleep(bool wake_up_on_mouse = false, bool quit_on_quit = false);
bool frame(bool quit_if_esc, bool delay); //returns false when done, updates the event, should be called exactly once per frame
void end();

//yeah not yet in a class right now. Forwards the events encountered in "frame" related to screen resizing.
bool resizeEventHappened();
int resizeEventNewSizeX();
int resizeEventNewSizeY();

/*
StandByUtil: this adds some SDL Delay if the mouse didn't move for a while.
This because normally every frame everything is redrawn and handled. If the user
is away, this is a lot of resources being used up for nothing. So go a little
slower then.
*/
class StandByUtil
{
  private:
    int lastMouseX;
    int lastMouseY;
    double lastChangeTime;
    double timeTillStandby;
  public:
    StandByUtil(double timeTillStandby = 60)
    : lastMouseX(0)
    , lastMouseY(0)
    , lastChangeTime(-1)
    , timeTillStandby(timeTillStandby)
    {
    }

    void handle(const lpi::IInput& input)
    {
      if(lastChangeTime < 0) lastChangeTime = input.getSeconds();

      int mouseX = input.mouseX();
      int mouseY = input.mouseY();

      if(mouseX != lastMouseX
      || mouseY != lastMouseY
      || input.mouseButtonDown(lpi::LMB)
      || input.mouseButtonDown(lpi::MMB)
      || input.mouseButtonDown(lpi::RMB))
      {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        lastChangeTime = input.getSeconds();
      }

      if(isStandBy(input))
      {
        SDL_Delay(200);
      }
    }

    bool isStandBy(const lpi::IInput& input) const
    {
      return input.getSeconds() - lastChangeTime > timeTillStandby;
    }
};

} //namespace lpi

