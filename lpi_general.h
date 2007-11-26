/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef general_h
#define general_h 

#include <SDL/SDL.h>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm> //std::min and std::max
#include <sstream>
#include <iomanip>

#include "lpi_color.h"

namespace lpi
{


/*bool doubleClick(float time, bool button = 0);*/
Uint32 getTicks(); //returns the ticks in milliseconds
inline double getSeconds() { return getTicks() / 1000.0; } //returns the ticks in seconds

double getRandom();
int getRandom(int first, int last); //get random number in the range of integers first-last (including last)
double getRandom(double first, double last); //get random number in the range of doubles first-last

////////////////////////////////////////////////////////////////////////////////
//HANDY AUXILIARY FUNCTIONS/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void combSort(std::vector<int> &order, std::vector<double> &dist);
int wrapmod(int i, int n); //wraps i between 0 and n, using the modulo operator
double clamp(double a, double low, double high);
int clamp(int a, int low, int high);

//there are no proper versions of these functions, or at least I've had troubles with them, so I just define the same functions but with a different name to avoid conflicts (abs, max, min)

//REMOVE THESE ASAP: std::abs, std::min and std::max work just as well, and there are also std::swap and so on by the way
template<typename T>
const T template_abs(const T &a)
{
  return (a < 0) ? -a : a;
}

bool isPowerOfTwo(int n);
int floatMod(double f, int m);
int gcd(int a, int b);
float floatpow(float a, float b);

inline int intdown(double f) //intdown(0.5) becomes 0, intdown(-0.5) becomes -1, intdown(0.0) becomes something unknown, it's a border
{
  if(f > 0) return int(f);
  else return int(f) - 1;
}

inline Sint32 intdivdown(Sint32 a, Sint32 b) //divide so that 2/2 = 1, 1/2 = 0, -1/2 = -1, -2/2 = -1, ...
{
  //return a < 0 ? (a + b) / b - 1 : a / b;

  if(a < 0)
  {
    return -((-a - 1) / b + 1);
  }
  else return a / b;
}


///////////////////////////////////////////////////////////////////////////////

class GameTime
{
  private:
  Sint32 oldTime; //this is in milliseconds
  Sint32 newTime;
  bool fps_prepared;
  bool oldtime_prepared;
  
  //for smooth fps value
  #define NUMLASTTIMES 64
  std::vector<Uint32> lastTimes;
  
  public:
  
  GameTime()
  {
    fps_prepared = false;
    oldtime_prepared = false;
  }
  
  void init_oldtime()
  {
    oldTime = getTicks();
    newTime = getTicks();
    
    oldtime_prepared = true;
  }
  
  void init_fps()
  {
    lastTimes.clear();
    lastTimes.push_back(newTime);
    
    fps_prepared = true;
  }
  
  Uint32 get_t() const { return newTime; } //get ticks
  double get_s() const { return newTime / 1000.0; } //get seconds
  Uint32 old_t() const { return oldTime; }
  double old_s() const { return oldTime / 1000.0; }
  
  Sint32 diff_t() const //return difference between two last frames
  {
    return newTime - oldTime;
  }
  
  double diff_s() const //return difference between two last frames, in seconds as floating point
  {
    return diff_t() / 1000.0;
  }
  
  Sint32 currentDiff() const //return difference between last frame and current time
  {
    return getTicks() - newTime;
  }
  
  double fps() const //returns fps from last NUMLASTTIMES frames instead of only the last one to have smoother values
  {
    if(!fps_prepared) return 0;
    double timeDiff = (lastTimes[lastTimes.size() - 1] - lastTimes[0]) / 1000.0; //time difference in seconds
    return lastTimes.size() / timeDiff;
  }
  
  void update() //don't call this too soon, but do call it before you start calling diff_s, fps(), etc. Don't call it before the game loads, or the fps values will be way off.
  {
    if(!oldtime_prepared) init_oldtime();
    if(!fps_prepared) init_fps();
    
    oldTime = newTime;
    newTime = getTicks();
    
    //for the fps stuff
    if(lastTimes.size() >= NUMLASTTIMES) lastTimes.erase(lastTimes.begin());
    lastTimes.push_back(newTime);
  }
  
};

void encodeBase64(std::string& out, const std::vector<unsigned char>& in);
void decodeBase64(std::vector<unsigned char>& out, const std::string& in);

} //namespace lpi

#endif //general_h
