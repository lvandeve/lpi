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

#pragma once

#include <SDL/SDL.h>
#include <vector>

namespace lpi
{

Uint32 getTicks(); //returns the ticks in milliseconds
inline double getSeconds() { return getTicks() / 1000.0; } //returns the ticks in seconds


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

} //namespace lpi

