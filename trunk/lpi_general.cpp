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

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "lpi_general.h"


namespace lpi
{

//using namespace std;



//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////
//NON GRAPHICS//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//


//function removed here as it couldn't work when multiple things test for doubleclicking
/*bool doubleClick(float time, bool button)*/

//Returns the time in integer milliseconds since the program started
Uint32 getTicks()
{
  return SDL_GetTicks();
}

double getRandom() //returns random number between 0.0 and 1.0
{
  int r = rand();
  if(r < 0) r = -r;
  r %= 32768;
  return r / 32768.0;
}

int getRandom(int first, int last)
{
  int r = rand();
  if(r < 0) r = -r;
  r %= (last - first + 1);
  return first + r;
}

double getRandom(double first, double last)
{
  return first + getRandom() * (last - first);
}


//this sorts objects based on their distance, and will at the same time also sort an ordered index
void sort(std::vector<int> &order, std::vector<double> &dist)
{
  //implemented with combsort here
  
  if(order.size() != dist.size()) return;
  int amount = order.size();
  
  int gap = amount;
  bool swapped = false;
  while(gap > 1 || swapped)
  {
    //shrink factor 1.3
    gap = (gap * 10) / 13;
    if(gap == 9 || gap == 10) gap = 11;
    if (gap < 1) gap = 1;
    swapped = false;
    for (int i = 0; i < amount - gap; i++)
    {
      int j = i + gap;
      if (dist[i] < dist[j])// <: from large to small, >: from small to large
      {
        int tempi; double tempf;
        tempf = dist[i]; dist[i] = dist[j]; dist[j] = tempf;
        tempi = order[i]; order[i] = order[j]; order[j] = tempi;
        swapped = true;
      }
    }
  }
}

int wrapmod(int i, int n) //wraps i between 0 and n, using the modulo operator
{
  if(i >= n) i = i % n;
  if(i < 0) i = (n - ((-i) % n)); //modulo division on negative values
  if(i == n) i = 0; //happens if i = -n, -2n, ...
  
  return i;
}

double clamp(double a, double low, double high)
{
  if(a < low) a = low;
  if(a > high) a = high;
  return a;
}

int clamp(int a, int low, int high)
{
  if(a < low) a = low;
  if(a > high) a = high;
  return a;
}

bool isPowerOfTwo(int n) //does not work properly if n is <= 0, then an extra test n > 0 should be added
{
  return !(n & (n - 1)); //this checks if the integer n is a power of two or not
}

/*
this will apply modulo division to a floating point number. by converting to integer and then taking modulo division, sometimes you don't get nice results because the integer is too small to contain the value of the floating point number. This function here tries to overcome that problem.
the return value is an integer in the range [0, m)
negative f is made positive and then the result is calculated (since this fucntion is supposed to be used to find digits).

NOTE: std::fmod already does this in fact...
*/
int floatMod(double f, int m)
{
  if(f < 0.0) f = -f;
  
  if(f < 1.0) return 0;
  
  double s = m; //subtractor
  while(f > s) s *= m;
  s /= m; //the above subtractor was one too large!
  
  while(s > m * m / 2) //I use m * m / 2 instead of m to avoid unprecisions
  {
    while(f > 0) f -= s;
    f += s;
    s /= m;
  }
  
  return int(f) % m;
}

/*
return the greatest common divider of the two given numbers, using Euclid's algorithm
*/
int gcd(int a, int b)
{
   while(b != 0)
   {
     int t = b;
     b = a % b;
     a = t;
   }
   return a;
}

void encodeBase64(std::string& out, const std::vector<unsigned char>& in)
{
  const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned long bit24 = 0;
  unsigned long sextet[4] = {0, 0, 0, 0};
  unsigned long octet[3] = {0, 0, 0};
  
  out.clear();
  out.reserve((4 * in.size()) / 3);
  
  for(size_t pos = 0; pos < in.size(); pos += 3)
  {
    octet[0] = in[pos + 0];
    octet[1] = (pos + 1 < in.size()) ? in[pos + 1] : 0;
    octet[2] = (pos + 2 < in.size()) ? in[pos + 2] : 0;
    
    bit24  = 256 * 256 * octet[0];
    bit24 +=       256 * octet[1];
    bit24 +=             octet[2];
    
    sextet[0] = (bit24 / (64 * 64 * 64)) % 64;
    sextet[1] = (bit24 / (     64 * 64)) % 64;
    sextet[2] = (bit24 / (          64)) % 64;
    sextet[3] = (bit24 / (           1)) % 64;
    
    for(size_t i = 0; i < 4; i++)
    {
      if(pos + i <= in.size()) out.push_back(characters[sextet[i]]);
      else out.push_back('=');
    }
    
    if(pos % 57 == 54) out.push_back(10); //newline char every 76 chars (57 = 3/4th of 76)
  }
}
  
void decodeBase64(std::vector<unsigned char>& out, const std::string& in)
{
  unsigned long bit24 = 0;
  unsigned long sextet[4] = {0, 0, 0, 0};
  unsigned long octet[3] = {0, 0, 0};
  
  out.clear();
  out.reserve((3 * in.size()) / 4);
  
  for(size_t pos = 0; pos < in.size() - 3; pos += 4)
  {
    for(size_t i = 0; i < 4; i++)
    {
      unsigned long c = in[pos + i];
      if(c >= 65 && c <= 90) sextet[i] = c - 65;
      else if(c >= 97 && c <= 122) sextet[i] = c - 71;
      else if(c >= 48 && c <= 57) sextet[i] = c + 4;
      else if(c == '+') sextet[i] = 62;
      else if(c == '/') sextet[i] = 63;
      else if(c == '=') sextet[i] = 0; //value doesn't matter
      else //unknown char, can be whitespace, newline, ...
      {
        pos++;
        if(pos > in.size() - 3) return;
        i--;
      }
    }
    
    bit24  = 64 * 64 * 64 * sextet[0];
    bit24 +=      64 * 64 * sextet[1];
    bit24 +=           64 * sextet[2];
    bit24 +=                sextet[3];
    
    octet[0] = (bit24 / (256 * 256)) % 256;
    octet[1] = (bit24 / (      256)) % 256;
    octet[2] = (bit24 / (        1)) % 256;
    
    for(size_t i = 0; i < 3; i++)
    {
      if(in[pos + 1 + i] != '=') out.push_back(octet[i]);
    }
  }
}

} //namespace lpi
