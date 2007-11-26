/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_math2d.h"

#include <cmath>

namespace lpi
{

Vector2::Vector2(double x, double y)
{
  this->x = x;
  this->y = y;
}

Vector2::Vector2()
{
  x = 0.0;
  y = 0.0;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
  x += v.x;
  y += v.y;
  return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
  x -= v.x;
  y -= v.y;
  return *this;
}

Vector2& Vector2::operator*=(double a)
{
  x *= a;
  y *= a;
  return *this;
}

Vector2& Vector2::operator/=(double a)
{
  x /= a;
  y /= a;
  return *this;
}

//Subtract two vectors: a----->b is b minus a
Vector2 operator-(const Vector2& v, const Vector2& w)
{
  Vector2 u = v;
  u -= w;
  return u;
}

void Vector2::negate()
{
  x = -x;
  y = -y;
}

//Return the negative of the vector
Vector2 operator-(const Vector2& v)
{
  Vector2 u = v;
  u.negate();
  return u;
}

//Add two vectors
Vector2 operator+(const Vector2& v, const Vector2& w)
{
  Vector2 u = v;
  u += w;
  return u;
}

//Multiplies a vector by a scalar
Vector2 operator*(const Vector2& v, double a)
{
  Vector2 w = v;
  w *= a;
  return w;
}

//Multiplies a vector by a scalar
Vector2 operator*(double a, const Vector2& v)
{
  Vector2 w = v;
  w *= a;
  return w;
}

//Divides a vector through a scalar
Vector2 operator/(const Vector2& v, double a)
{
  Vector2 w = v;
  w /= a;
  return w;
}

////////////////////////////////////////////////////////////////////////////////

double Vector2::length() const
{
  return std::sqrt(x * x + y * y);
}

double Vector2::lengthsq() const
{
  return x * x + y * y;
}

double Vector2::manhattan() const
{
  return std::abs(x) + std::abs(y);
}

double length(const Vector2& v)
{
  return std::sqrt(v.x * v.x + v.y * v.y);
}

double lengthsq(const Vector2& v)
{
  return v.x * v.x + v.y * v.y;
}

double manhattan(const Vector2& v)
{
  return std::abs(v.x) + std::abs(v.y);
}

void Vector2::normalize()
{
  double l = length();
  if(l != 0.0)
  {
    x /= l;
    y /= l;
  }
}

Vector2 normalize(const Vector2& v)
{
  Vector2 result = v;
  result.normalize();
  return result;
}

double Vector2::distance(const Vector2& v)
{
  return std::sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
}

double Vector2::distancesq(const Vector2& v)
{
  return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y);
}

double Vector2::manhattan(const Vector2& v)
{
  return std::abs(v.x - x) + std::abs(v.y - y);
}

double distance(const Vector2& v, const Vector2& w)
{
  return std::sqrt((v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y));
}

double distancesq(const Vector2& v, const Vector2& w) //square of the distance, no square root taken
{
  return (v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y);
}

double manhattan(const Vector2& v, const Vector2& w) //square of the distance, no square root taken
{
  return std::abs(v.x - w.x) + std::abs(v.y - w.y);
}

double Vector2::dot(const Vector2& v)
{
  return v.x * x + v.y * y;
}

double dot(const Vector2& v, const Vector2& w)
{
  return v.x * w.x + v.y * w.y;
}

}
