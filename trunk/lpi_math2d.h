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

#ifndef LPI_MATH2D_H_INCLUDED
#define LPI_MATH2D_H_INCLUDED

namespace lpi
{


////////////////////////////////////////////////////////////////////////////////
//                  [ x ]                                                     //
// 2D Vector Class  [ y ]                                                     //
////////////////////////////////////////////////////////////////////////////////
class Vector2
{
  public:

  double x;
  double y;
  
  Vector2(double x, double y);
  Vector2();
  
  Vector2& operator+=(const Vector2& v);
  Vector2& operator-=(const Vector2& v);
  Vector2& operator*=(double a);
  Vector2& operator/=(double a);
  
  double length() const;
  double lengthsq() const;
  double manhattan() const;
  void normalize(); //normalizes the vector, but lets it remain the 0 vector if it's length is 0
  double distance(const Vector2& v);
  double distancesq(const Vector2& v);
  double manhattan(const Vector2& v);
  double dot(const Vector2& v);
  void negate();
};

Vector2 operator*(const Vector2& v, double a);
Vector2 operator*(double a, const Vector2& v);
Vector2 operator/(const Vector2& v, double a);
Vector2 operator+(const Vector2& v, const Vector2& w);
Vector2 operator-(const Vector2& v, const Vector2& w);
Vector2 operator-(const Vector2& v);

double length(const Vector2& v);
double lengthsq(const Vector2& v);
double manhattan(const Vector2& v);
Vector2 normalize(const Vector2& v);
double distance(const Vector2& v, const Vector2& w);
double distancesq(const Vector2& v, const Vector2& w);
double manhattan(const Vector2& v, const Vector2& w);
double dot(const Vector2& v, const Vector2& w);

}

#endif
