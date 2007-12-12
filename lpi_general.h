/*
Copyright (c) 2005-2007 Lode Vandevenne
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

#ifndef LPI_GENERAL_H_INCLUDED
#define LPI_GENERAL_H_INCLUDED

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

Uint32 getTicks(); //returns the ticks in milliseconds
inline double getSeconds() { return getTicks() / 1000.0; } //returns the ticks in seconds

double getRandom();
int getRandom(int first, int last); //get random number in the range of integers first-last (including last)
double getRandom(double first, double last); //get random number in the range of doubles first-last

////////////////////////////////////////////////////////////////////////////////
//HANDY AUXILIARY FUNCTIONS/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void sort(std::vector<int> &order, std::vector<double> &dist);
int wrapmod(int i, int n); //wraps i between 0 and n, using the modulo operator
double clamp(double a, double low, double high);
int clamp(int a, int low, int high);

bool isPowerOfTwo(int n);
int floatMod(double f, int m);
int gcd(int a, int b);

static const double pi = 3.14159265358979323846264338327950288419716939937510; //why isn't this in standard C++ anyway

inline int intdown(double f) //intdown(0.5) becomes 0, intdown(-0.5) becomes -1, intdown(0.0) becomes something unknown, it's a border
{
  if(f > 0) return int(f);
  else return int(f) - 1;
}

inline int intdivdown(int a, int b) //divide so that 2/2 = 1, 1/2 = 0, -1/2 = -1, -2/2 = -1, ... (more logical on 2D tile maps than the standard integer dividion)
{
  if(a < 0) return -((-a - 1) / b + 1);
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
  
  GameTime() : oldTime(0), newTime(0), fps_prepared(false), oldtime_prepared(false)
  {
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
  
  Sint32 currentDiff() const //return difference between last frame and current time (in ms)
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

#endif
