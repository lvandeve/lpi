/*
Copyright (c) 2005-2010 Lode Vandevenne
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

#include <iostream>

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
  
  const double& operator[](int i) const { return *(&x + i); }
  double& operator[](int i) { return *(&x + i); }
  
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
Vector2 operator-(const Vector2& v, const Vector2& w); //a - b is b----->a
Vector2 operator-(const Vector2& v);
bool operator==(const Vector2& a, const Vector2& b);
bool operator!=(const Vector2& a, const Vector2& b);

double length(const Vector2& v);
double lengthsq(const Vector2& v);
double manhattan(const Vector2& v);
Vector2 normalize(const Vector2& v);
double distance(const Vector2& v, const Vector2& w);
double distancesq(const Vector2& v, const Vector2& w);
double manhattan(const Vector2& v, const Vector2& w);
double dot(const Vector2& v, const Vector2& w);

std::ostream& operator<<(std::ostream& ostr, const Vector2& v);

static const Vector2 Vector2_origin = Vector2(0.0, 0.0);
static const Vector2 Vector2_0 =      Vector2(0.0, 0.0);
static const Vector2 Vector2_x =      Vector2(1.0, 0.0);
static const Vector2 Vector2_y =      Vector2(0.0, 1.0);

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                  [ 0 2 ]                                                   //
// 2D Matrix Class  [ 1 3 ]                                                   //
////////////////////////////////////////////////////////////////////////////////
class Matrix2 //memory: exactly 4 doubles
{
  public:
    
  Vector2 a[2];
  
  const Vector2& operator[](int i) const { return a[i]; }
  Vector2& operator[](int i) { return a[i]; }
  
  Matrix2(double a0, double a1, double a2, double a3);
  Matrix2();
  
  void transpose();
  double determinant() const;
  void invert();
  
  Matrix2& operator+=(const Matrix2& rhs);
  Matrix2& operator-=(const Matrix2& rhs);
  Matrix2& operator*=(double f);
  Matrix2& operator/=(double f);
  Matrix2& operator*=(const Matrix2& rhs);
};

static const Matrix2 Matrix2_identity(1.0, 0.0, 0.0, 1.0);
static const Matrix2 Matrix2_unit(1.0, 0.0, 0.0, 1.0);

Matrix2 transpose(const Matrix2& A);
double determinant(const Matrix2& A);
Matrix2 inverse(const Matrix2& A);
Matrix2 operator+(const Matrix2& A, const Matrix2& B);
Matrix2 operator-(const Matrix2& A, const Matrix2& B);
Matrix2 operator*(const Matrix2& A, double a);
Matrix2 operator*(double a, const Matrix2& A);
Matrix2 operator/(const Matrix2& A, double a);
Vector2 operator*(const Matrix2& A, const Vector2& v);
Matrix2 operator*(const Matrix2& A, const Matrix2& B);

std::ostream& operator<<(std::ostream& ostr, const Matrix2& m);


////////////////////////////////////////////////////////////////////////////////



//precondition: dir must be normalized!!
Vector2 getComponentInDirection(const Vector2& v, const Vector2& dir);
double distancePointLine(const Vector2& p, const Vector2& a, const Vector2& b); //get distance from point p to the line given by a and b
double distancePointLineSegment(const Vector2& p, const Vector2& a, const Vector2& b);
double distancePointLineSq(const Vector2& p, const Vector2& a, const Vector2& b); //get distance from point p to the line given by a and b
double distancePointLineSegmentSq(const Vector2& p, const Vector2& a, const Vector2& b);
bool linesParallel(const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1);
void intersectLineLine(Vector2& result, const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1);
bool intersectLineSegmentLineSegment(Vector2& result, const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1);

bool sideOfLineGivenByTwoPoints(const Vector2& p, const Vector2& a, const Vector2& b);

//barycentric non-perspective correct coordinates for rasterizing 2D triangle; to make perspective correct, use barycentric_persp of lpi_math3d after this
void barycentric(double& alpha, double& beta, double& gamma, const lpi::Vector2& a, const lpi::Vector2& b, const lpi::Vector2& c, const lpi::Vector2& p);
bool pointInsideTriangle(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c);

//The 3 points are counterclockwise if result > 0, clockwise if result < 0, colinear if result = 0. Absolute value is triangle area.
double triangleArea(const Vector2& v0, const Vector2& v1, const Vector2& v2);
bool ccw(const Vector2& v0, const Vector2& v1, const Vector2& v2); //returns true if triangleArea(v0, v1, v2) < 0

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
