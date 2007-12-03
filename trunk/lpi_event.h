/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
