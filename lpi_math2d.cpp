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

#include "lpi_math2d.h"

#include <cmath>
#include <iostream>

namespace lpi
{

Vector2::Vector2(double x, double y) : x(x), y(y)
{
}

Vector2::Vector2() /*: x(0.0), y(0.0)*/
{
}

Vector2::Vector2(const Vector2& other) : x(other.x), y(other.y)
{
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

void Vector2::clamp(double value)
{
  double v = lengthsq();
  if(v > value * value)
  {
    (*this) *= (value * value / v);
  }
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

bool operator==(const Vector2& a, const Vector2& b)
{
  return (a.x == b.x && a.y == b.y);
}

bool operator!=(const Vector2& a, const Vector2& b)
{
  return !(a == b);
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

std::ostream& operator<<(std::ostream& ostr, const Vector2& v)
{
  return ostr << "[" << v.x << " " << v.y << "]";
}

////////////////////////////////////////////////////////////////////////////////

Matrix2::Matrix2(double a0, double a1, double a2, double a3)
{
  a[0][0] = a0;
  a[0][1] = a1;
  a[1][0] = a2;
  a[1][1] = a3;
}

Matrix2::Matrix2()
{
}

////////////////////////////////////////////////////////////////////////////////
//Transpose:                                                                  //
//                                                                            //
// [ 0 2 ]T   [ 0 1 ]                                                         //
// [ 1 3 ]  = [ 2 3 ]                                                         //
////////////////////////////////////////////////////////////////////////////////
void Matrix2::transpose()
{
  std::swap(a[0][1], a[1][0]);
}

Matrix2 transpose(const Matrix2& A)
{
  Matrix2 result = A;
  result.transpose();
  return result;
}

double Matrix2::determinant() const
{
  double det = a[0][0] * a[1][1]
             - a[1][0] * a[0][1];
  return det;
}

double determinant(const Matrix2& A)
{
  return A.determinant();
}

//Inverse of a 2x2 matrix
void Matrix2::invert()
{
  /*
  the inverse is the adjoint divided through the determinant
  find the matrix of minors (minor = determinant of 2x2 matrix of the 2 rows/colums current element is NOT in)
  turn them in cofactors (= change some of the signs)
  find the adjoint by transposing the matrix of cofactors
  divide this through the determinant to get the inverse
  */
  /*
  abc 036
  def 147
  ghi 258
  */

  double det = determinant();
  Matrix2 B;

  //included in these calculations: minor, cofactor (changed signs), transpose (by the order of "="), division through determinant
  B.a[0][0] = ( a[1][1] ) / det;
  B.a[1][0] = (-a[1][0] ) / det;
  B.a[0][1] = (-a[0][1] ) / det;
  B.a[1][1] = ( a[0][0] ) / det;
  
  *this = B;
}

Matrix2 inverse(const Matrix2& A)
{
  Matrix2 result = A;
  result.invert();
  return result;
}

Matrix2& Matrix2::operator+=(const Matrix2& rhs)
{
  a[0][0] += rhs.a[0][0];
  a[1][0] += rhs.a[1][0];
  a[0][1] += rhs.a[0][1];
  a[1][1] += rhs.a[1][1];
  return *this;
}

Matrix2 operator+(const Matrix2& A, const Matrix2& B)
{
  Matrix2 result = A;
  result += B;
  return result;
}

Matrix2& Matrix2::operator-=(const Matrix2& rhs)
{
  a[0][0] -= rhs.a[0][0];
  a[1][0] -= rhs.a[1][0];
  a[0][1] -= rhs.a[0][1];
  a[1][1] -= rhs.a[1][1];
  return *this;
}

Matrix2 operator-(const Matrix2& A, const Matrix2& B)
{
  Matrix2 result = A;
  result -= B;
  return result;
}

Matrix2& Matrix2::operator*=(double f)
{
  a[0][0] *= f;
  a[1][0] *= f;
  a[0][1] *= f;
  a[1][1] *= f;
  return *this;
}

Matrix2 operator*(const Matrix2& A, double a)
{
  Matrix2 result = A;
  result *= a;
  return result;
}

Matrix2 operator*(double a, const Matrix2& A)
{
  Matrix2 result = A;
  result *= a;
  return result;
}

Matrix2& Matrix2::operator/=(double f)
{
  a[0][0] /= f;
  a[1][0] /= f;
  a[0][1] /= f;
  a[1][1] /= f;
  return *this;
}

Matrix2 operator/(const Matrix2& A, double a)
{
  Matrix2 result = A;
  result /= a;
  return result;
}

//Multiply a matrix with a column vector, resulting in a column vector
Vector2 operator*(const Matrix2& A, const Vector2& v)
{
  Vector2 w(A.a[0][0] * v.x + A.a[1][0] * v.y,
            A.a[0][1] * v.x + A.a[1][1] * v.y);
  return w;
}

Matrix2& Matrix2::operator*=(const Matrix2& rhs)
{
  Matrix2 temp;
  temp.a[0][0] = a[0][0]*rhs.a[0][0] + a[1][0]*rhs.a[0][1];
  temp.a[0][1] = a[0][1]*rhs.a[0][0] + a[1][1]*rhs.a[0][1];
  temp.a[1][0] = a[0][0]*rhs.a[1][0] + a[1][0]*rhs.a[1][1];
  temp.a[1][1] = a[0][1]*rhs.a[1][0] + a[1][1]*rhs.a[1][1];
  *this = temp;
  return *this;
}

Matrix2 operator*(const Matrix2& A, const Matrix2& B)
{
  //not implemented in terms of operator*= because there already is a copy in there...
  Matrix2 C;
  C.a[0][0] = A.a[0][0]*B.a[0][0] + A.a[1][0]*B.a[0][1];
  C.a[0][1] = A.a[0][1]*B.a[0][0] + A.a[1][1]*B.a[0][1];
  C.a[1][0] = A.a[0][0]*B.a[1][0] + A.a[1][0]*B.a[1][1];
  C.a[1][1] = A.a[0][1]*B.a[1][0] + A.a[1][1]*B.a[1][1];
  return C;
}

std::ostream& operator<<(std::ostream& ostr, const Matrix2& m)
{
  return ostr << "[" << m.a[0][0] << " " << m.a[1][0] << " ; " << m.a[0][1] << " " << m.a[1][1] << "]";
}

////////////////////////////////////////////////////////////////////////////////

//returns the component of v in the direction dir (dir must be normalized, otherwise the factor is wrong)
Vector2 getComponentInDirection(const Vector2& v, const Vector2& dir)
{
  /*Vector2 dirn = normalize(dir);
  double mag = dot(v, dirn);
  Vector2 proj = mag * dirn;
  return proj;*/
  
  return dir * dot(v, dir);
}

//get distance from point p to the line given by a and b
double distancePointLine(const Vector2& p, const Vector2& a, const Vector2& b)
{
  //Explanationn of the formula: see Vector3 version of this function.
  
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
  Vector2 q;
  q.x = a.x + k * (b.x - a.x);
  q.y = a.y + k * (b.y - a.y);
  
  return distance(p, q);
}

double distancePointLineSegment(const Vector2& p, const Vector2& a, const Vector2& b)
{
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
  if(k < 0) return distance(p, a);
  else if(k > 1) return distance(p, b);
  else
  {
    Vector2 q;
    q.x = a.x + k * (b.x - a.x);
    q.y = a.y + k * (b.y - a.y);
    return distance(p, q);
  }
}

//get squared distance from point p to the line given by a and b
double distancePointLineSq(const Vector2& p, const Vector2& a, const Vector2& b)
{
  //Explanationn of the formula: see Vector3 version of this function.
  
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
  Vector2 q;
  q.x = a.x + k * (b.x - a.x);
  q.y = a.y + k * (b.y - a.y);
  
  return distancesq(p, q);
}

double distancePointLineSegmentSq(const Vector2& p, const Vector2& a, const Vector2& b)
{
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
  if(k < 0) return distancesq(p, a);
  else if(k > 1) return distancesq(p, b);
  else
  {
    Vector2 q;
    q.x = a.x + k * (b.x - a.x);
    q.y = a.y + k * (b.y - a.y);
    return distancesq(p, q);
  }
}

////////////////////////////////////////////////////////////////////////////////

//not perspective correct
void barycentric(double& alpha, double& beta, double& gamma, const lpi::Vector2& a, const lpi::Vector2& b, const lpi::Vector2& c, const lpi::Vector2& p)
{
   /*
   The formula in non-optimized form:
   
   gamma = ((a.y-b.y)*p.x + (b.x-a.x)*p.y + a.x*b.y - b.x*a.y)
         / ((a.y-b.y)*c.x + (b.x-a.x)*c.y + a.x*b.y - b.x*a.y);
   beta  = ((a.y-c.y)*p.x + (c.x-a.x)*p.y + a.x*c.y - c.x*a.y)
         / ((a.y-c.y)*b.x + (c.x-a.x)*b.y + a.x*c.y - c.x*a.y);
   alpha = 1.0 - beta - gamma;
   */

  //It becomes much simpler by subtracting a from everything, making a.x and a.y 0, gamma and beta can even share the same denominator because only the sign differs
  Vector2 ba = b - a;
  Vector2 ca = c - a;
  Vector2 pa = p - a;
  
  double invdenom = 1.0 / (ba.x * ca.y - ba.y * ca.x);
  gamma = (ba.x * pa.y - ba.y * pa.x) * invdenom;
  beta  = (ca.y * pa.x - ca.x * pa.y) * invdenom;
  alpha = 1.0 - beta - gamma;
}

bool pointInsideTriangle(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c)
{
  double alpha, beta, gamma;
  barycentric(alpha, beta, gamma, a, b, c, p);
  return alpha >= 0.0 && alpha <= 1.0 && beta >= 0.0 && beta <= 1.0;
}

double triangleArea(const Vector2& v0, const Vector2& v1, const Vector2& v2)
{
  return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
}

bool ccw(const Vector2& v0, const Vector2& v1, const Vector2& v2)
{
  return triangleArea(v0, v1, v2) < 0;
}

//I want this to return true if you're on one side of the line, false if on other (use to check if two points are on same side)
bool sideOfLineGivenByTwoPoints(const Vector2& p, const Vector2& a, const Vector2& b)
{
  Vector2 p2 = p - a;
  Vector2 b2 = b - a;
  
  Vector2 n(b2.y, -b2.x);
  
  double proj = dot(p2, n);
  
  return (proj >= 0);
}

bool linesParallel(const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1)
{
  if(a1.x - a0.x == 0 && b1.x - b0.x == 0) return true;
  
  double ricoa = (a1.y-a0.y)/(a1.x-a0.x);
  double ricob = (b1.y-b0.y)/(b1.x-b0.x);
  
  return ricoa == ricob;
}

void intersectLineLine(Vector2& result, const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1)
{
  double noemer = ((b1.y-b0.y)*(a1.x-a0.x) - (b1.x-b0.x)*(a1.y-a0.y));
  double ua = ((b1.x-b0.x)*(a0.y-b0.y) - (b1.y-b0.y)*(a0.x-b0.x)) / noemer;
  result = a0 + ua * (a1 - a0);
}

bool intersectLineSegmentLineSegment(Vector2& result, const Vector2& a0, const Vector2& a1, const Vector2& b0, const Vector2& b1)
{
  double noemer = ((b1.y-b0.y)*(a1.x-a0.x) - (b1.x-b0.x)*(a1.y-a0.y));
  double ua = ((b1.x-b0.x)*(a0.y-b0.y) - (b1.y-b0.y)*(a0.x-b0.x)) / noemer;
  double ub = ((a1.x-a0.x)*(a0.y-b0.y) - (a1.y-a0.y)*(a0.x-b0.x)) / noemer;
  result = a0 + ua * (a1 - a0);
  return (ua >= 0.0 && ua <= 1.0 && ub >= 0.0 && ub <= 1.0);
}

////////////////////////////////////////////////////////////////////////////////

bool deflect(Vector2& dir, const Vector2& shooterpos, const Vector2& targetpos, const Vector2& vel, double speed)
{
  double a = speed * speed - lengthsq(vel);
  double b = -(2.0 * dot(vel, targetpos - shooterpos));
  double c = -lengthsq(targetpos - shooterpos);
  
  double D = b * b - 4.0 * a * c; //discriminant
  
  if(D <= 0.0 || a == 0.0)
  {
    dir = normalize(targetpos - shooterpos); //vel too fast for bullet's speed, so ignore it
    return false;
  }
  else
  {
    double sqD = std::sqrt(D);
    double t1 = (-b + sqD) / (2.0 * a); //time of hit solution 1
    double t2 = (-b - sqD) / (2.0 * a); //time of hit solution 2
    if(t1 < 0.0 && t2 < 0.0) //we can only hit the enemy in negative time, so no proper solution found
    {
      dir = normalize(targetpos - shooterpos); //vel too fast for bullet's speed, so ignore it
      return false;
    }
    else
    {
      double t; //pick the smallest nonnegative value
      
      if     (t1 < 0.0) t = t2;
      else if(t2 < 0.0) t = t1;
      else if(t1 < t2)  t = t1;
      else              t = t2;
      
      Vector2 intercept = targetpos + t * vel;
      dir = normalize(intercept - shooterpos);
      return true;
    }
  }
}

}
