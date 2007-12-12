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
