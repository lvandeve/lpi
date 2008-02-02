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

/*
lpi_event: SDL events, mouse input and keyboard input
*/

#ifndef LPI_EVENT_H_INCLUDED
#define LPI_EVENT_H_INCLUDED

#include <SDL/SDL.h>

namespace lpi
{

extern SDL_Event event;

//like it or not, the mouse interface works with global variables. You can even control them yourself for gui unit tests. That is and stays the interface :)
extern int globalMouseX;
extern int globalMouseY;
extern bool globalLMB; //left mouse button
extern bool globalRMB; //right mouse button
extern bool globalMouseWheelUp; //mouse wheel up
extern bool globalMouseWheelDown; //mouse wheel down

//left or right mouse button
enum MouseButton
{
  LMB,
  RMB
};

inline bool getGlobalMouseButton(MouseButton button)
{
  if(button == LMB) return globalLMB;
  if(button == RMB) return globalRMB;
  return 0;
}

//these 3 functions can be used in the GUI unit test to mimic the behaviour of a user moving the mouse over controls
inline void debugSetMousePos(int x, int y) { globalMouseX = x; globalMouseY = y; }
inline void debugSetLMB(bool LMB) { globalLMB = LMB; }
inline void debugSetRMB(bool RMB) { globalLMB = RMB; }


void readKeys();

struct KeyState
{
  /*
  used for remembering if the keys were held down last frame, for e.g. keyPressed and keyPressedTime
  if you don't give a KeyState to keyPressed or keyPressedTime, it uses a default one
  you want to use a second one if you want to have independent checking of keyPressed
  */
  bool keyReleased[1024];
  bool keyWarmedUp[1024]; //for keyPressedTime
  double lastTime[1024]; //for keyPressedTime; last time it returned true
};

bool keyDown(int key);
bool keyPressed(int key, KeyState* state = 0); //only returns true the first time the key is down and you check
bool keyPressedTime(int key, double time, double warmupTime, double repTime, KeyState* state = 0); //reptime = time to repeat key press. All times in seconds.
bool pressedEnter(KeyState* state = 0);

void checkMouse(); //updates the values from the physical mouse
int mouseXDiff();
int mouseYDiff();
int mouseXDiffWarp();
int mouseYDiffWarp();
void resetMouseDiffFunctions();

/*
warmup = "warmup time": how long it takes before the key starts repeating
rate = repetition rate of the key after the warmupTime
having released the key resets the warmup
timingIndex = index of variable that remembers key timing, so you can have different independent inputs
*/
int unicodeKey(int allowedChars, int warmup = 0.5, int rate = 0.025, int index = 0);

//these were moved from general to input because they depend so much on input
void sleep();
bool frame(bool quit_if_esc, bool delay); //returns false when done, updates the event, should be called exactly once per frame
void end();

} //namespace lpi

#endif
