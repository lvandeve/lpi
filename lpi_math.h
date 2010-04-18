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

#ifndef LPI_MATH_H_INCLUDED
#define LPI_MATH_H_INCLUDED

#include <vector>
#include <cmath>
#include <algorithm> //std::min and std::max
#include <iostream>

namespace lpi
{

void sort(std::vector<int>& order, std::vector<double>& dist);

double getRandom(); //returns a random value in the range [0.0-1.0[, excluding the value 1.0 itself.
int getRandom(int first, int last); //get random number in the range of integers first-last (including last)
double getRandom(double first, double last); //get random number in the range of doubles first-last

/*
This wraps i between 0 and n, using the modulo operator. Result is in range [0,n[, n NOT included.
This is actually intended as a replacement
for the module operator that behaves different for negative numbers: the repeating pattern of the shape of the function curve
isn't disturbed at 0, the result is always positive
*/
int wrap(int i, int n); //


/*
WARNING! In the template functions below, wrap does NOT include the highest
value in the range of the result, while clamp DOES include the highest value in the range
of the result. The reason for this is that the functions can work on both floating point
numbers and integers. For floating point numbers, in the case of wrapping it's hard to define
how to include the highest value in a way consistent with integers (some difference has to be
chosen, 1 for integers, ???? for floating point). For clamping, it's the opposite, then it's
hard to choose how NOT to include the highest value (the next lower float, or -1 in case of integers).

So, read the comments about the result range in the wrap and clamp functions below very carefully.
*/

template<typename T, typename U, typename V>
T wrap(T a, U low, V high) //wraps in range [low,high[, high NOT included!
{
  if(low == (U)high) return low;
  
  T diff = high - low;
  
  if(a < (T)low) a += ((long)((low - a) / diff) + 1) * (diff);
  if(a >= (T)high) a -= ((long)((a - high) / diff) + 1) * (diff);

  return a;
}

template<typename T, typename U, typename V>
T clamp(T a, U low, V high) //clamps in range [low,high], high INCLUDED!
{
  if(a < (T)low) a = (T)low;
  if(a > (T)high) a = (T)high;
  return a;
}

bool isPowerOfTwo(int n);
int floatMod(double f, int m); //TODO: remove, this is just fmod from the C library
int gcd(int a, int b);

static const double pi = 3.14159265358979323846264338327950288419716939937510; //why isn't this in standard C++ anyway
static const double twopi = 6.283185307179586477;

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

/*
For the solve functions below:
*) return value is number of real solutions
*) if the number of solutions is smaller than the number of "x" parameters, then
   the valid values are filled in the first x'es.
*) the last x'es will then contain meaningless values that may be either
   the real part of a complex solutions, an uninitialized value, or the value
   they already had before being given as parameter to the function, but there's
   no way provided to find whether or not that's the case, so only use the
   first x'es that are said to be valid according to the return value
*) in some functions it's specified that the x'es will have the real part of the
   complex solution, e.g. for quadsol
*/
int linsol(double& x1, double a, double b); //linear equation solution (ax+b=0). if a == 0.0, then it'll return that there were 0 solutions
int quadsol(double& x1, double& x2, double a, double b, double c); //quadratic equation solution (axx+bx+c=0)
int cubicsol(double& x1, double& x2, double& x3, double a, double b, double c, double d); //cubic equation solution (axxx+bxx+cx+d=0). number of real solutions is always at least 1 if a isn't 0.0, else quadsol will be called.
int quartsol(double& x1, double& x2, double& x3, double& x4, double a, double b, double c, double d, double e); //quadratic equation solution (axxxx+bxxx+cxx+dx+e=0)



/*
If the compiler is confused about the parameter type of abs, max, min, ...,
or they are #defined wrongly or whatever else, the template functions below
might lessen the pain.
*/
template<typename T>
T template_abs(T a)
{
  return a > 0 ? a : -a;
}

template<typename T, typename U>
T template_max(T a, U b)
{
  return a > (T)b ? a : (T)b;
}

template<typename T, typename U>
T template_min(T a, U b)
{
  return a < (T)b ? a : (T)b;
}



}

#endif
