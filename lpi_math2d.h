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
  Vector2(const Vector2& other);
  
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
  void clamp(double value); //set maximum length to value
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

//precondition: dir must be normalized!!
Vector2 getComponentInDirection(const Vector2& v, const Vector2& dir);

//barycentric non-perspective correct coordinates for rasterizing 2D triangle; to make perspective correct, use barycentric_persp of lpi_math3d after this
void barycentric(double& alpha, double& beta, double& gamma, const lpi::Vector2& a, const lpi::Vector2& b, const lpi::Vector2& c, const lpi::Vector2& p);

/*
deflect, aka Target Leading or Target Prediction System.
Calculates what direction to shoot a slow moving bullet at to make it hit a moving target.
The result is stored in dir.
Return value is true if a solution was found, false if there wasn't a solution, e.g. if the velocity is too high for a bullet of speed to handle.
vel is targetvelocity - shootervelocity.
speed is speed of the slow moving bullet.
*/
bool deflect(Vector2& dir, const Vector2& shooterpos, const Vector2& targetpos, const Vector2& vel, double speed);

}

#endif
