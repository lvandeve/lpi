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

#include "lpi_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace lpi
{

//this sorts objects based on their distance, and will at the same time also sort an ordered index
void sort(std::vector<int>& order, std::vector<double>& dist)
{
  ////implemented with combsort here
  
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

int linsol(double& x1, double a, double b) //ax + b = 0
{
  //if a is 0.0, then the equation is 0.0 * x + b = 0, and then there's no solution (well, -inf)
  x1 = -b / a;
  if(a == 0.0) return 0;
  else return 1;
}

int quadsol(double& x1, double& x2, double a, double b, double c) // axx + bx + c = 0
{
  if(a == 0.0) //it's a linear equation, so the method below will NOT work. Call linsol instead.
  {
    return linsol(x1, b, c);
  }
  
  double D = b * b - 4.0 * a * c; //discriminant
  
  if(D < 0.0)
  {
    x1 = x2 = 0.0;
    return 0;
  }
  else if(D == 0.0)
  {
    x1 = x2 = (-b) / (2.0 * a);
    return 1;
  }
  else
  {
    double sqD = std::sqrt(D);
    x1 = (-b + sqD) / (2.0 * a);
    x2 = (-b - sqD) / (2.0 * a);
    return 2;
  }
}

static double cbrt(double d) //cubic root
{
  return d < 0.0 ? -std::pow(-d, 1.0/3.0) : std::pow(d, 1.0/3.0);
}

int cubicsol(double& x1, double& x2, double& x3, double a, double b, double c, double d) //axxx + bxx + cx + d = 0
{
  if(a != 0.0)
  {
    //normalize the equation
    d /= a;
    c /= a;
    b /= a;
    a = 1.0;
  }
  else //it's a third degree equation, so the method below will NOT work. Call quadsol instead.
  {
    return quadsol(x1, x2, b, c, d);
  }

  double Q = (3.0*c - b*b) / 9.0;
  double R = (9.0*b*c - 27.0*d - 2.0*b*b*b) / 54.0;
  
  double D = Q*Q*Q + R*R;
  
  if(D >= 0.0)
  {
    double S = cbrt(R + std::sqrt(D));
    double T = cbrt(R - std::sqrt(D));

    x1 = -b / 3.0 + (S + T);
    //commented out is the imaginary part (j represents sqrt(-1))
    x2 = -b / 3.0 - (S + T) / 2.0 /*+ j * std::sqrt(3.0) * (S - T) / 2.0*/;
    x3 = -b / 3.0 - (S + T) / 2.0 /*- j * std::sqrt(3.0) * (S - T) / 2.0*/;
    
    if(D == 0.0) return 2; //then S == T, hence no imaginary part for x2 and x3, but they're both the same
    else return 1;
  }
  else //if D < 0, there are 3 real solutions, found with goniometrical functions
  {
    double sqQ = std::sqrt(-Q); //Q is negative, otherwise D could never be negative
    double theta = std::acos(R / (sqQ * sqQ * sqQ));
    
    x1 = 2.0 * sqQ * std::cos(theta / 3.0) - b / 3.0;
    x2 = 2.0 * sqQ * std::cos((theta + 2.0 * pi) / 3.0) - b / 3.0;
    x3 = 2.0 * sqQ * std::cos((theta + 4.0 * pi) / 3.0) - b / 3.0;
    
    return 3;
  }
}

int quartsol(double& x1, double& x2, double& x3, double& x4, double a, double b, double c, double d, double e) //axxxx + bxxx + cxx + dx + e = 0
{
  if(a != 0.0)
  {
    //normalize the equation
    e /= a;
    d /= a;
    c /= a;
    b /= a;
    a = 1.0;
  }
  else //it's a third degree equation, so the method below will NOT work. Call quadsol instead.
  {
    return cubicsol(x1, x2, x3, b, c, d, e);
  }
  
  //all a's have been commented out because they're 1.0
  
  double alpha = -(3.0*b*b)/(8.0/**a*a*/) + c/*/a*/;
  double beta = (b*b*b)/(8.0/**a*a*a*/) - (b*c)/(2.0/**a*a*/) + d/*/a*/;
  double gamma = -(3.0*b*b*b*b)/(256.0/**a*a*a*a*/) + (c*b*b)/(16.0/**a*a*a*/) - (b*d)/(4/**a*a*/) + e/*/a*/;
  
  if(beta == 0.0)
  {
    double temp1 = alpha*alpha - 4.0*gamma;
    
    if(temp1 < 0.0) return 0; //no real solutions
    
    double temp2a = (-alpha + std::sqrt(temp1)) / 2.0;
    double temp2b = (-alpha - std::sqrt(temp1)) / 2.0;
    
    double temp3 = -b/(4.0/**a*/);
    
    if(temp2a < 0.0 && temp2b < 0.0) return 0; //no real solutions
    else if(temp2a >= 0.0 && temp2b < 0.0)
    {
      x1 = temp3 + std::sqrt(temp2a);
      x2 = temp3 - std::sqrt(temp2a);
      return 2;
    }
    else if(temp2a < 0.0 && temp2b >= 0.0)
    {
      x1 = temp3 + std::sqrt(temp2b);
      x2 = temp3 - std::sqrt(temp2b);
      return 2;
    }
    else
    {
      x1 = temp3 + std::sqrt(temp2a);
      x2 = temp3 - std::sqrt(temp2a);
      x3 = temp3 + std::sqrt(temp2b);
      x4 = temp3 - std::sqrt(temp2b);
      return 4;
    }
  }
  else
  {
    double P, Q, R, U, y, W;
    P = -(alpha*alpha)/12.0 - gamma;
    Q = -(alpha*alpha*alpha)/108.0 + (alpha*gamma)/3.0 - (beta*beta)/8.0;
    double temp1 = Q*Q/4.0 + P*P*P/27.0;
    if(temp1 < 0.0) return 0; //no real solutions
    R = Q/2.0 +/*-*/std::sqrt(temp1);
    U = cbrt(R);
    y = -(5.0/6.0) * alpha - U + (U == 0.0 ? 0.0 : P/(3.0*U));
    double temp = alpha + 2.0 * y;
    W = temp > 0.0 ? std::sqrt(temp) : 0.0; //mathematically proven to be always real here, but for numerical stability extra test with temp is added
  
    double temp2a = -(3.0*alpha + 2.0*y + (2.0*beta)/W);
    double temp2b = -(3.0*alpha + 2.0*y - (2.0*beta)/W);
    
    double temp3 = -b/(4.0/**a*/);
    
    if(temp2a < 0.0 && temp2b < 0.0) return 0; //no real solutions
    else if(temp2a >= 0.0 && temp2b <= 0.0)
    {
      x1 = temp3 + (+W + std::sqrt(temp2a)) / 2.0;
      x2 = temp3 + (+W - std::sqrt(temp2a)) / 2.0;
      return 2;
    }
    else if(temp2a <= 0.0 && temp2b >= 0.0)
    {
      x1 = temp3 + (-W + std::sqrt(temp2b)) / 2.0;
      x2 = temp3 + (-W - std::sqrt(temp2b)) / 2.0;
      return 2;
    }
    else
    {
      x1 = temp3 + (+W + std::sqrt(temp2a)) / 2.0;
      x2 = temp3 + (+W - std::sqrt(temp2a)) / 2.0;
      x3 = temp3 + (-W + std::sqrt(temp2b)) / 2.0;
      x4 = temp3 + (-W - std::sqrt(temp2b)) / 2.0;
      return 4;
    }
  }
}



} //namespace lpi


// void TEST_cubicsol(double a, double b, double c, double d)
// {
//   double x1, x2, x3;
//   int num = lpi::cubicsol(x1, x2, x3, a, b, c, d);
//   std::cout << "test3: " << a << "xxx + " << b << "xx + " << c << "x + " << d << " = 0" << std::endl;
//   std::cout << "num: " << num << " " << "sol: " << x1 << " " << x2 << " " << x3 << std::endl;
//   std::cout << "check: " << a*x1*x1*x1+b*x1*x1+c*x1+d
//                   << " " << a*x2*x2*x2+b*x2*x2+c*x2+d
//                   << " " << a*x3*x3*x3+b*x3*x3+c*x3+d
//                          << std::endl;
//   
//   std::cout << std::endl;
// }
// 
// void TEST_quartsol(double a, double b, double c, double d, double e)
// {
//   double x1, x2, x3, x4;
//   int num = lpi::quartsol(x1, x2, x3, x4, a, b, c, d, e);
//   std::cout << "test4: " << a << "xxxx + " << b << "xxx + " << c << "xx + " << d << " x + " << e << " = 0" << std::endl;
//   std::cout << "num: " << num << " " << "sol: " << x1 << " " << x2 << " " << x3 << " " << x4 << std::endl;
//   std::cout << "check: " << a*x1*x1*x1*x1+b*x1*x1*x1+c*x1*x1+d*x1+e
//                   << " " << a*x2*x2*x2*x2+b*x2*x2*x2+c*x2*x2+d*x2+e
//                   << " " << a*x3*x3*x3*x3+b*x3*x3*x3+c*x3*x3+d*x3+e
//                   << " " << a*x4*x4*x4*x4+b*x4*x4*x4+c*x4*x4+d*x4+e
//                          << std::endl;
//   
//   std::cout << std::endl;
// }
// 
// int main()
// {
//   /*TEST_cubicsol(1,2,3,4);
//   TEST_cubicsol(1,-2,3,-4);
//   TEST_cubicsol(-1,2,-3,4);
//   
//   TEST_cubicsol(1,1,1,1);
//   TEST_cubicsol(0,1,1,1);
//   TEST_cubicsol(0,1,1,-1);
//   TEST_cubicsol(1,0,0,0);
//   TEST_cubicsol(0,1,0,0);
//   TEST_cubicsol(0,0,1,0);
//   TEST_cubicsol(0,0,0,1);*/
//   
//   TEST_cubicsol(45,24,-7,-2);
//   
//   TEST_quartsol(1,0,6,-60,36);
//   TEST_quartsol(1,-1,1,-1,1);
//   TEST_quartsol(1,2,3,4,5);
//   TEST_quartsol(1,0,0,0,0);
//   TEST_quartsol(1,2,1,2,1);
//   TEST_quartsol(1,2,-1,-2,1); //2x the same 2 real solutions
//   TEST_quartsol(1,2,-2,2,10);
//   TEST_quartsol(3,6,-123,-126,1080); //4 real roots
//   
// }

//int main()
//{
  //std::cout
  //<<lpi::wrap(-6, 0, 3)<<" "<<lpi::wrap(-5, 0, 3)<<" "<<lpi::wrap(-4, 0, 3)<<" "
  //<<lpi::wrap(-3, 0, 3)<<" "<<lpi::wrap(-2, 0, 3)<<" "<<lpi::wrap(-1, 0, 3)<<" "
  //<<lpi::wrap(0, 0, 3)<<" "<<lpi::wrap(1, 0, 3)<<" "<<lpi::wrap(2, 0, 3)<<" "
  //<<lpi::wrap(3, 0, 3)<<" "<<lpi::wrap(4, 0, 3)<<" "<<lpi::wrap(5, 0, 3)<<" "
  //<<lpi::wrap(6, 0, 3)<<" "<<lpi::wrap(7, 0, 3)<<" "<<lpi::wrap(8, 0, 3)<<" "
  //<<lpi::wrap(9, 0, 3)<<" "<<lpi::wrap(10, 0, 3)<<" "<<lpi::wrap(11, 0, 3)<<" "
  //<<std::endl;
  //std::cout
  //<<lpi::wrap(-6.5, 0.5, 3.5)<<" "<<lpi::wrap(-5.5, 0.5, 3.5)<<" "<<lpi::wrap(-4.5, 0.5, 3.5)<<" "
  //<<lpi::wrap(-3.5, 0.5, 3.5)<<" "<<lpi::wrap(-2.5, 0.5, 3.5)<<" "<<lpi::wrap(-1.5, 0.5, 3.5)<<" "
  //<<lpi::wrap(0.5, 0.5, 3.5)<<" "<<lpi::wrap(1.5, 0.5, 3.5)<<" "<<lpi::wrap(2.5, 0.5, 3.5)<<" "
  //<<lpi::wrap(3.5, 0.5, 3.5)<<" "<<lpi::wrap(4.5, 0.5, 3.5)<<" "<<lpi::wrap(5.5, 0.5, 3.5)<<" "
  //<<lpi::wrap(6.5, 0.5, 3.5)<<" "<<lpi::wrap(7.5, 0.5, 3.5)<<" "<<lpi::wrap(8.5, 0.5, 3.5)<<" "
  //<<lpi::wrap(9.5, 0.5, 3.5)<<" "<<lpi::wrap(10.5, 0.5, 3.5)<<" "<<lpi::wrap(11.5, 0.5, 3.5)<<" "
  //<<std::endl;
//}
