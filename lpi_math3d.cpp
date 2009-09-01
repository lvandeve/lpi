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

#include "lpi_math3d.h"
#include <iostream>

namespace { const double pi = 3.14159265358979323846264338327950288419716939937510; }

namespace lpi
{

///////////////////////////////////////////////////////////////////////////////

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z)
{
}

Vector3::Vector3() /*: x(0.0), y(0.0), z(0.0)*/
{
}

Vector3::Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z)
{
}

Vector3& Vector3::operator+=(const Vector3& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

Vector3& Vector3::operator*=(double a)
{
  x *= a;
  y *= a;
  z *= a;
  return *this;
}

Vector3& Vector3::operator/=(double a)
{
  x /= a;
  y /= a;
  z /= a;
  return *this;
}

double Vector3::length() const
{
  return std::sqrt(x * x + y * y + z * z);
}

double Vector3::lengthsq() const
{
  return x * x + y * y + z * z;
}

double Vector3::manhattan() const
{
  return std::abs(x) + std::abs(y) + std::abs(z);
}

double length(const Vector3& v)
{
  return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

double lengthsq(const Vector3& v)
{
  return v.x * v.x + v.y * v.y + v.z * v.z;
}

double manhattan(const Vector3& v)
{
  return std::abs(v.x) + std::abs(v.y) + std::abs(v.z);
}

void Vector3::normalize()
{
  double l = length();
  if(l != 0.0)
  {
    x /= l;
    y /= l;
    z /= l;
  }
}

Vector3 normalize(const Vector3& v)
{
  Vector3 result = v;
  result.normalize();
  return result;
}

double Vector3::distance(const Vector3& v)
{
  return std::sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z));
}

double Vector3::distancesq(const Vector3& v)
{
  return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z);
}

double Vector3::manhattan(const Vector3& v)
{
  return std::abs(v.x - x) + std::abs(v.y - y) + std::abs(v.z - z);
}

double distance(const Vector3& v, const Vector3& w)
{
  return std::sqrt((v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y) + (v.z - w.z) * (v.z - w.z));
}

double distancesq(const Vector3& v, const Vector3& w) //square of the distance, no square root taken
{
  return (v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y) + (v.z - w.z) * (v.z - w.z);
}

double manhattan(const Vector3& v, const Vector3& w)
{
  return std::abs(v.x - w.x) + std::abs(v.y - w.y) + std::abs(v.z - w.z);
}

double Vector3::dot(const Vector3& v)
{
  return v.x * x + v.y * y + v.z * z;
}

double dot(const Vector3& v, const Vector3& w)
{
  return v.x * w.x + v.y * w.y + v.z * w.z;
}

Vector3 Vector3::cross(const Vector3& v)
{
  Vector3 u;

  u.x = ((v.y * z) - (v.z * y));
  u.y = ((v.z * x) - (v.x * z));
  u.z = ((v.x * y) - (v.y * x));
  return u;
}

Vector3 cross(const Vector3& v, const Vector3& w)
{
  Vector3 u;

  u.x = ((v.y * w.z) - (v.z * w.y));
  u.y = ((v.z * w.x) - (v.x * w.z));
  u.z = ((v.x * w.y) - (v.y * w.x));
  return u;
}

//Subtract two vectors
Vector3 operator-(const Vector3& v, const Vector3& w)
{
  Vector3 u = v;
  u -= w;
  return u;
}

void Vector3::negate()
{
  x = -x;
  y = -y;
  z = -z;
}

void Vector3::clamp(double value)
{
  double v = lengthsq();
  if(v > value * value)
  {
    *this *= (value * value / v);
  }
}

//Return the negative of the vector
Vector3 operator-(const Vector3& v)
{
  Vector3 u = v;
  u.negate();
  return u;
}

//Add two vectors
Vector3 operator+(const Vector3& v, const Vector3& w)
{
  Vector3 u = v;
  u += w;
  return u;
}

//Multiplies a vector by a scalar
Vector3 operator*(const Vector3& v, double a)
{
  Vector3 w = v;
  w *= a;
  return w;
}

//Multiplies a vector by a scalar
Vector3 operator*(double a, const Vector3& v)
{
  Vector3 w = v;
  w *= a;
  return w;
}

//Divides a vector through a scalar
Vector3 operator/(const Vector3& v, double a)
{
  Vector3 w = v;
  w /= a;
  return w;
}

bool operator==(const Vector3& a, const Vector3& b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

bool operator!=(const Vector3& a, const Vector3& b)
{
  return !(a == b);
}

//Gives the angle between two 3D vectors (in radians), should be a value from 0 to Pi
//if v == w, the dot product is 1, so the result will be 0
//if v is 90 degrees on w, the dot product is 0 and thus the acos of that is Pi/2 (90 degrees)
//if v == -w, the dot product is -1, so the result will be acos(-1) = Pi (180 degrees)
//any other case is a value between 0 and Pi
double vectorAngle(const Vector3& v, const Vector3& w)
{
  //dot product(v,w) = length(v)*length(w)*cos(angle) ==> angle = acos(dot/(length(v)*length(w))) = acos(dot(norm(v)*norm(w)));
  double cosineOfAngle = dot(normalize(v), normalize(w));
  cosineOfAngle = std::min(std::max(cosineOfAngle, -1.0), 1.0); //avoid acos giving nan due to numerical imprecisions
  return std::acos(cosineOfAngle);
}

/*
get the angle of vector v and w, after v and w are projected on the plane with normal n
will give positive value if the angle v - w is in clockwise direction when looking down on n, otherwise negative
*/
double vectorAngleInDirection(const Vector3& v, const Vector3& w, const Vector3& n)
{
  Vector3 v2 = projectOnPlane(v, n);
  Vector3 w2 = projectOnPlane(w, n);
  int sign = 1;
  if(hasOppositeDirection(cross(v2, w2), normalize(n))) sign = -1;
  return sign * vectorAngle(v2, w2);
}

/*
Rotate vector v around arbitrary axis axis for angle radians
It can only rotate around an axis through the origin, to rotate around another axis:
first translate the object to the axis, then use this function, then translate back in the new direction.
*/
Vector3 rotateAroundArbitrary(const Vector3& v, const Vector3& axis, double angle)
{
  if((v.x == 0) && (v.y == 0) && (v.z == 0)) return Vector3_origin;

  Vector3 w;
  double c, s, t;
  
  Vector3 axis_n = axis;

  axis_n.normalize();

  //calculate parameters of the rotation matrix
  c = cos(angle);
  s = sin(angle);
  t = 1 - c;

  //multiply v with rotation matrix
  w.x = (t * axis_n.x * axis_n.x +      c) * v.x 
      + (t * axis_n.x * axis_n.y + s * axis_n.z) * v.y 
      + (t * axis_n.x * axis_n.z - s * axis_n.y) * v.z;
    
  w.y = (t * axis_n.x * axis_n.y - s * axis_n.z) * v.x 
      + (t * axis_n.y * axis_n.y +      c) * v.y 
      + (t * axis_n.y * axis_n.z + s * axis_n.x) * v.z;
    
  w.z = (t * axis_n.x * axis_n.z + s * axis_n.y) * v.x 
      + (t * axis_n.y * axis_n.z - s * axis_n.x) * v.y 
      + (t * axis_n.z * axis_n.z +      c) * v.z;

  w.normalize();
  w = w * v.length();
  
  return w;
}

//returns a rotation matrix that rotates stuff by multiplying with this returned matrix, where you put this returned matrix on the left side of the multiplication.
Matrix3 getRotationMatrix3(const Vector3& axis, double angle)
{
  if((axis.x == 0) && (axis.y == 0) && (axis.z == 0)) return Matrix3_unit;

  double c, s, t;

  Vector3 axisn = normalize(axis);

  //calculate parameters of the rotation matrix
  c = cos(angle);
  s = sin(angle);
  t = 1 - c;
  
  Matrix3 r = Matrix3(t * axisn.x * axisn.x +           c, t * axisn.x * axisn.y + s * axisn.z, t * axisn.x * axisn.z - s * axisn.y,
                      t * axisn.x * axisn.y - s * axisn.z, t * axisn.y * axisn.y +           c, t * axisn.y * axisn.z + s * axisn.x,
                      t * axisn.x * axisn.z + s * axisn.y, t * axisn.y * axisn.z - s * axisn.x, t * axisn.z * axisn.z +           c);

  return r;
}

Matrix3 rotateAroundArbitrary(const Matrix3& m, const Vector3& axis, double angle)
{
  Matrix3 rot = getRotationMatrix3(axis, angle);
  Matrix3 result = rot * m;
  return result;
}

Matrix3::Matrix3(double a0, double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8)
{
  a[0][0] = a0;
  a[0][1] = a1;
  a[0][2] = a2;
  a[1][0] = a3;
  a[1][1] = a4;
  a[1][2] = a5;
  a[2][0] = a6;
  a[2][1] = a7;
  a[2][2] = a8;
}

Matrix3::Matrix3()
{
}

////////////////////////////////////////////////////////////////////////////////
//Transpose:                                                                  //
//                                                                            //
// [ 0 3 6 ]T   [ 0 1 2 ]                                                     //
// [ 1 4 7 ]  = [ 3 4 5 ]                                                     //
// [ 2 5 8 ]    [ 6 7 8 ]                                                     //
////////////////////////////////////////////////////////////////////////////////
void Matrix3::transpose()
{
  std::swap(a[0][1], a[1][0]);
  std::swap(a[0][2], a[2][0]);
  std::swap(a[1][2], a[2][1]);
}

Matrix3 transpose(const Matrix3& A)
{
  Matrix3 result = A;
  result.transpose();
  return result;
}

double Matrix3::determinant() const
{
  double det = a[0][0] * a[1][1] * a[2][2]
             + a[1][0] * a[2][1] * a[0][2]
             + a[0][1] * a[1][2] * a[2][0]
             - a[0][2] * a[1][1] * a[2][0]
             - a[0][1] * a[1][0] * a[2][2]
             - a[1][2] * a[2][1] * a[0][0];
  return det;
}

double determinant(const Matrix3& A)
{
  return A.determinant();
}

//Inverse of a 3x3 matrix
void Matrix3::invert()
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
  Matrix3 B;

  //included in these calculations: minor, cofactor (changed signs), transpose (by the order of "="), division through determinant
  B.a[0][0] = ( a[1][1] * a[2][2] - a[1][2] * a[2][1]) / det;
  B.a[1][0] = (-a[1][0] * a[2][2] + a[1][2] * a[2][0]) / det;
  B.a[2][0] = ( a[1][0] * a[2][1] - a[1][1] * a[2][0]) / det;
  B.a[0][1] = (-a[0][1] * a[2][2] + a[0][2] * a[2][1]) / det;
  B.a[1][1] = ( a[0][0] * a[2][2] - a[0][2] * a[2][0]) / det;
  B.a[2][1] = (-a[0][0] * a[2][1] + a[0][1] * a[2][0]) / det;
  B.a[0][2] = ( a[0][1] * a[1][2] - a[0][2] * a[1][1]) / det;
  B.a[1][2] = (-a[0][0] * a[1][2] + a[0][2] * a[1][0]) / det;
  B.a[2][2] = ( a[0][0] * a[1][1] - a[0][1] * a[1][0]) / det;
  
  *this = B;
}

Matrix3 inverse(const Matrix3& A)
{
  Matrix3 result = A;
  result.invert();
  return result;
}

Matrix3& Matrix3::operator+=(const Matrix3& rhs)
{
  a[0][0] += rhs.a[0][0];
  a[1][0] += rhs.a[1][0];
  a[2][0] += rhs.a[2][0];
  a[0][1] += rhs.a[0][1];
  a[1][1] += rhs.a[1][1];
  a[2][1] += rhs.a[2][1];
  a[0][2] += rhs.a[0][2];
  a[1][2] += rhs.a[1][2];
  a[2][2] += rhs.a[2][2];
  return *this;
}

Matrix3 operator+(const Matrix3& A, const Matrix3& B)
{
  Matrix3 result = A;
  result += B;
  return result;
}

Matrix3& Matrix3::operator-=(const Matrix3& rhs)
{
  a[0][0] -= rhs.a[0][0];
  a[1][0] -= rhs.a[1][0];
  a[2][0] -= rhs.a[2][0];
  a[0][1] -= rhs.a[0][1];
  a[1][1] -= rhs.a[1][1];
  a[2][1] -= rhs.a[2][1];
  a[0][2] -= rhs.a[0][2];
  a[1][2] -= rhs.a[1][2];
  a[2][2] -= rhs.a[2][2];
  return *this;
}

Matrix3 operator-(const Matrix3& A, const Matrix3& B)
{
  Matrix3 result = A;
  result -= B;
  return result;
}

Matrix3& Matrix3::operator*=(double f)
{
  a[0][0] *= f;
  a[1][0] *= f;
  a[2][0] *= f;
  a[0][1] *= f;
  a[1][1] *= f;
  a[2][1] *= f;
  a[0][2] *= f;
  a[1][2] *= f;
  a[2][2] *= f;
  return *this;
}

Matrix3 operator*(const Matrix3& A, double a)
{
  Matrix3 result = A;
  result *= a;
  return result;
}

Matrix3 operator*(double a, const Matrix3& A)
{
  Matrix3 result = A;
  result *= a;
  return result;
}

Matrix3& Matrix3::operator/=(double f)
{
  a[0][0] /= f;
  a[1][0] /= f;
  a[2][0] /= f;
  a[0][1] /= f;
  a[1][1] /= f;
  a[2][1] /= f;
  a[0][2] /= f;
  a[1][2] /= f;
  a[2][2] /= f;
  return *this;
}

Matrix3 operator/(const Matrix3& A, double a)
{
  Matrix3 result = A;
  result /= a;
  return result;
}

//Multiply a matrix with a column vector, resulting in a column vector
Vector3 operator*(const Matrix3& A, const Vector3& v)
{
  Vector3 w(A.a[0][0] * v.x + A.a[1][0] * v.y + A.a[2][0] * v.z,
            A.a[0][1] * v.x + A.a[1][1] * v.y + A.a[2][1] * v.z,
            A.a[0][2] * v.x + A.a[1][2] * v.y + A.a[2][2] * v.z);
  return w;
}

//function DISABLED, the vector3 class is one of column vectors
/*//Multiply a vector with a row matrix, resulting in a row vector
Vector3 operator*(const Vector3& v, Matrix3 A)
{
  Vector3 w(A.a[0][0] * v.x + A.a[0][1] * v.y + A.a[0][2] * v.z,
            A.a[1][0] * v.x + A.a[1][1] * v.y + A.a[1][2] * v.z,
            A.a[2][0] * v.x + A.a[2][1] * v.y + A.a[2][2] * v.z);
  return w;
}*/

Matrix3& Matrix3::operator*=(const Matrix3& rhs)
{
  Matrix3 temp;
  temp.a[0][0] = a[0][0]*rhs.a[0][0] + a[1][0]*rhs.a[0][1] + a[2][0]*rhs.a[0][2];
  temp.a[0][1] = a[0][1]*rhs.a[0][0] + a[1][1]*rhs.a[0][1] + a[2][1]*rhs.a[0][2];
  temp.a[0][2] = a[0][2]*rhs.a[0][0] + a[1][2]*rhs.a[0][1] + a[2][2]*rhs.a[0][2];
  temp.a[1][0] = a[0][0]*rhs.a[1][0] + a[1][0]*rhs.a[1][1] + a[2][0]*rhs.a[1][2];
  temp.a[1][1] = a[0][1]*rhs.a[1][0] + a[1][1]*rhs.a[1][1] + a[2][1]*rhs.a[1][2];
  temp.a[1][2] = a[0][2]*rhs.a[1][0] + a[1][2]*rhs.a[1][1] + a[2][2]*rhs.a[1][2];
  temp.a[2][0] = a[0][0]*rhs.a[2][0] + a[1][0]*rhs.a[2][1] + a[2][0]*rhs.a[2][2];
  temp.a[2][1] = a[0][1]*rhs.a[2][0] + a[1][1]*rhs.a[2][1] + a[2][1]*rhs.a[2][2];
  temp.a[2][2] = a[0][2]*rhs.a[2][0] + a[1][2]*rhs.a[2][1] + a[2][2]*rhs.a[2][2];
  *this = temp;
  return *this;
}

Matrix3 operator*(const Matrix3& A, const Matrix3& B)
{
  //not implemented in terms of operator*= because there already is a copy in there...
  Matrix3 C;
  C.a[0][0] = A.a[0][0]*B.a[0][0] + A.a[1][0]*B.a[0][1] + A.a[2][0]*B.a[0][2];
  C.a[0][1] = A.a[0][1]*B.a[0][0] + A.a[1][1]*B.a[0][1] + A.a[2][1]*B.a[0][2];
  C.a[0][2] = A.a[0][2]*B.a[0][0] + A.a[1][2]*B.a[0][1] + A.a[2][2]*B.a[0][2];
  C.a[1][0] = A.a[0][0]*B.a[1][0] + A.a[1][0]*B.a[1][1] + A.a[2][0]*B.a[1][2];
  C.a[1][1] = A.a[0][1]*B.a[1][0] + A.a[1][1]*B.a[1][1] + A.a[2][1]*B.a[1][2];
  C.a[1][2] = A.a[0][2]*B.a[1][0] + A.a[1][2]*B.a[1][1] + A.a[2][2]*B.a[1][2];
  C.a[2][0] = A.a[0][0]*B.a[2][0] + A.a[1][0]*B.a[2][1] + A.a[2][0]*B.a[2][2];
  C.a[2][1] = A.a[0][1]*B.a[2][0] + A.a[1][1]*B.a[2][1] + A.a[2][1]*B.a[2][2];
  C.a[2][2] = A.a[0][2]*B.a[2][0] + A.a[1][2]*B.a[2][1] + A.a[2][2]*B.a[2][2];
  return C;
}

//Transformation3 functions

/*
Note:
Every function that changes u, v or dir should use "generateMatrix();" at the end,
unless that function already calls another one that generates it, at the end.
*/
Transformation3::Transformation3(double posx, double posy, double posz, double ux, double uy, double uz, double vx, double vy, double vz, double dirx, double diry, double dirz, double nearClip, double farClip)
{
  pos.x = posx;
  pos.y = posy;
  pos.z = posz;
  trans[0][0] = ux;
  trans[0][1] = uy;
  trans[0][2] = uz;
  trans[1][0] = vx;
  trans[1][1] = vy;
  trans[1][2] = vz;
  trans[2][0] = dirx;
  trans[2][1] = diry;
  trans[2][2] = dirz;
  this->nearClip = nearClip;
  this->farClip = farClip;
  invMatrixUpToDate = false;
  generateMatrix();
  planetGroundPlaneType = XZ;
}

//Construct a camera with default parameters
Transformation3::Transformation3()
{
  pos.x = 0.0;
  pos.y = 0.0;
  pos.z = 0.0;
  trans[0][0] = 1.0;
  trans[0][1] = 0.0;
  trans[0][2] = 0.0;
  trans[1][0] = 0.0;
  trans[1][1] = 1.0;
  trans[1][2] = 0.0;
  trans[2][0] = 0.0;
  trans[2][1] = 0.0;
  trans[2][2] = 1.0;
  nearClip = 0.1;
  farClip = 1000000.0;
  invMatrixUpToDate = false;
  generateMatrix();
  planetGroundPlaneType = XZ;
}

Vector3& Transformation3::getU()
{
  return trans[0];
}

Vector3& Transformation3::getV()
{
  return trans[1];
}

Vector3& Transformation3::getDir()
{
  return trans[2];
}

Vector3& Transformation3::getPos()
{
  return pos;
}

const Vector3& Transformation3::getU() const
{
  return trans[0];
}

const Vector3& Transformation3::getV() const
{
  return trans[1];
}

const Vector3& Transformation3::getDir() const
{
  return trans[2];
}

const Vector3& Transformation3::getPos() const
{
  return pos;
}

void Transformation3::setU(const Vector3& newU)
{
  trans[0] = newU;
  generateMatrix();
}

void Transformation3::setV(const Vector3& newV)
{
  trans[1] = newV;
  generateMatrix();
}

void Transformation3::setDir(const Vector3& newDir)
{
  trans[2] = newDir;
  generateMatrix();
}

void Transformation3::setPos(const Vector3& newPos)
{
  pos = newPos;
}

void Transformation3::move(const Vector3& offset)
{
  pos = pos + offset;
}

/*
rotate around the center of this transformation, around certain axis through this center
*/
void Transformation3::rotate(const Vector3& axis, double angle)
{
  trans[0] = rotateAroundArbitrary(trans[0], axis, angle);
  trans[1] = rotateAroundArbitrary(trans[1], axis, angle);
  trans[2] = rotateAroundArbitrary(trans[2], axis, angle);
  generateMatrix();
}

/*
rotate around a line given by 2 points

It can only rotate around an axis through our object, to rotate around another axis:
first translate the object to the axis, then use this function, then translate back in the new direction.
*/
void Transformation3::rotate(const Vector3& a, const Vector3& b, double angle)
{
  //make a the origin, so that b - a becomes the rotation axis for rotateAroundArbitrary function that only works for axis through origin
  pos = pos - a;
  Vector3 axis = b - a;
  
  trans[0] = rotateAroundArbitrary(trans[0], axis, angle);
  trans[1] = rotateAroundArbitrary(trans[1], axis, angle);
  trans[2] = rotateAroundArbitrary(trans[2], axis, angle);
  pos = rotateAroundArbitrary(pos, axis, angle);
  pos = pos + rotateAroundArbitrary(a, axis, angle); //now a itself is also rotated, the translation over which the vectors were moved is rotated
  
  generateMatrix();
}

//returns a vector that is perpendicular on v, it may be any random vector all that you know is that it's perpendicular on v
Vector3 getSomePerpendicularVector(const Vector3& v)
{
  Vector3 w = cross(v, Vector3(0, 0, 1));
  if(length(w) == 0) //this only happens if v == (0, 0, 1) or v == -(0, 0, 1)
  {
    w = cross(v, Vector3(0, 1, 0));//now we try with a different vector so now w will certainly not have length 0
  }
  return w;
}

//Look in a certain direction.
void Transformation3::setLookDir(const Vector3& newDir)
{
  //calculate the rotation axis
  Vector3 axis = cross(trans[2], newDir);
  if(axis.length() == 0) axis = getSomePerpendicularVector(trans[2]); //this happens if dir is equal to newDir or the opposite, and then you rotate either 0 or 180 degrees, so the direction of the axis doesn't matter, it just must be perpendicular

  double angle = -vectorAngle(newDir, trans[2]);
  if(angle != 0) rotate(axis, angle);
  
  generateMatrix();
}

//Look at a certain point (the point will come in the center of the screen, if nothing's in front of it that is)
void Transformation3::lookAt(const Vector3& lookAtMe)
{
  setLookDir(lookAtMe - pos);
}

//this function sets the v vector to the direction of the given normal of the plane, so that this plane becomes the ground
void Transformation3::setGroundPlane(const Vector3& n)
{
  //calculate the rotation axis
  Vector3 axis = cross(trans[1], -n);
  if(axis.length() == 0) axis = getSomePerpendicularVector(trans[1]); //see in setLookDir function for explanation of this

  double angle = -vectorAngle(-n, trans[1]);
  if(angle != 0) rotate(axis, angle);
  
  generateMatrix();
}

double Transformation3::getDist(const Vector3& point) const
{
  return distance(pos, point);
}



//Set the distance to a point
void Transformation3::setDist(const Vector3& point, double dist)
{
  //double currentDist = distance(pos, point);
  setPos(point + dist * normalize(pos - point));
}

void Transformation3::zoom(double a)
{
  //increasing length of dir or decreasing length of the plane (u and v) = zoom IN
  trans[0] = trans[0] / a;
  trans[1] = trans[1] / a;
  generateMatrix();
}

void Transformation3::zoomU(double a)
{
  trans[0] = trans[0] / a;
  generateMatrix();
}

void Transformation3::zoomV(double a)
{
  trans[1] = trans[1] / a;
  generateMatrix();
}

double Transformation3::getZoomU() const
{
  return(trans[2].length() / trans[0].length());
}

double Transformation3::getZoomV() const
{
  return(trans[2].length() / trans[1].length());
}

void Transformation3::setZoomU(double a)
{
  trans[0] = trans[0] / (a / getZoomU());
  generateMatrix();
}

void Transformation3::setZoomV(double a)
{
  trans[1] = trans[1] / (a / getZoomV());
  generateMatrix();
}

double Transformation3::getFOVU(double w, double h) const
{
  //NOTE: it's multiplied by w and divided through h, because only then it's correct for the opengl projection system used
  return(w * 2.0 * atan2(trans[0].length(), trans[2].length()) / h);
}

double Transformation3::getFOVV(double /*w*/, double /*h*/) const
{
  return(2.0 * atan2(trans[1].length(), trans[2].length()));
}

void Transformation3::setFOVU(double angle)
{
  setZoomU(1.0 / tan(angle / 2.0));
}

void Transformation3::setFOVV(double angle)
{
  setZoomV(1.0 / tan(angle / 2.0));
}

////////////////////////////////////////////////////////////////////////////////
//Functions to get Yaw, Pitch and Roll. Normally to do things with the camera, 
//you never need these angles, everything works with vectors!
//However, these can be useful if you want a compass in your game, or a weapon 
//that allows you to accurately change it's pitch
////////////////////////////////////////////////////////////////////////////////

/*
Yaw is the wind direction the camera is looking at if you'd be standing on planet earth, and the ground has vectors X and Z, while Y points to the sky
       N
       ^ 
       |z 
       |   x      Looking in the direction Z = North (0 deg., 0 rad)
W -----+-----> E  Looking in the direction X = East (+90 deg., +1.57 rad)
       |          Looking in negative Z = South (180 deg., 3.1415 rad, sign jump)
       |          Looking in negative X = West (-90 deg., -1.57 rad)
       |
       S
*/

//NOTE!!!! It may seem confusing that X and Z are wind directions, while Y is the axis of the world that points to the sky, but this is the convention used in most 3D engines as well as OpenGL, and it makes a camera with unit matrix have it's up vector really up to the sky while the looking direction is a wind direction!

double Transformation3::getYaw() const
{
  //the atan2 function returns the angle of a 2D point (like from polar coordinates), so here it gives angle of dir projected on XZ plane, which is what we want for the yaw
  if(planetGroundPlaneType == XZ) return(atan2(trans[2].x, trans[2].z));
  else /*if(planetGroundPlaneType == XY)*/ return(atan2(trans[2].x, trans[2].y));
}

//setYaw can be used to make you look at a certain wind direction (where the ground is the XZ plane)
void Transformation3::setYaw(double angle)
{  
  double currentAngle = getYaw();
  //to change yaw, you have to rotate around the "up" axis of the WORLD = the y axis
  
  if(planetGroundPlaneType == XZ) rotate(Vector3(0,1,0), - angle + currentAngle);
  else if(planetGroundPlaneType == XY) rotate(Vector3(0,0,1), - angle + currentAngle);
  
  generateMatrix();
}

//rotates camera around world Y-axis with given angle (in space this would make no sense since there isn't really an "up" or "down" of the world, instead you rotate around the "up" and "down" of your own spaceship there
//When rolled, yawPlanet will give very annoying rotation, so for a spacegame or flight simulator you'll want to use yawSpace instead.
void Transformation3::yawPlanet(double angle)
{
  if(planetGroundPlaneType == XZ) rotate(Vector3(0,1,0), angle);
  else if(planetGroundPlaneType == XY) rotate(Vector3(0,0,1), angle);
  
  generateMatrix();
}

//rotates camera around camera v axis with given angle (this one makes sense in space, but not on a planet, on a planet your camera would start getting a "roll")
void Transformation3::yawSpace(double angle)
{
  rotate(trans[1], angle);
}

/*
Pitch is only useful to define if you're standing on a plane or a planet, and there's a sky
The plane you're standing on is the XZ plane, and Y points to the sky
Pitch is 0 deg. if you look forward (direction vector parallel to the XZ plane)
Pitch is +90 deg. if you look to the sky (maximum "up")
Pitch is -90 deg. if you look at the ground (maximum "down")
Pitches of more than 90 deg. or less than -90 deg. aren't defined, since these can also be made by having  yaw 180 deg. rotated and having a pitch between -90 deg. and +90 deg.
*/
double Transformation3::getPitch() const
{
  //Project dir on the XZ plane
  //Then find angle between dir and projected dir   
  //With atan2: angle of the point (lengthof2Dvector(dir.x, dir.z), dir.y)
  
  if(planetGroundPlaneType == XZ) return atan2(trans[2].y, std::sqrt(trans[2].x * trans[2].x + trans[2].z * trans[2].z));
  else /*if(planetGroundPlaneType == XY)*/ return atan2(trans[2].z, std::sqrt(trans[2].x * trans[2].x + trans[2].y * trans[2].y));
}

/*
setPitch can be used to make you look to the sky or floor with certain angle without changing wind direction
NOT useful for spacecrafts etc... because there's no up or down in space and X, Y and Z axes have no physical meaning there
only useful for FPS games or other games where you walk on a planet surface
*/
void Transformation3::setPitch(double angle)
{
  double currentAngle = getPitch();
  //to change pitch, you have to rotate around the horizontal vector of the camera
  rotate(trans[0], angle - currentAngle);
}

//pitches the camera over a certain amount
void Transformation3::pitch(double angle)
{
  rotate(trans[0], angle);
}

/*
Roll is only necessary for spacegames, flightsimulators or rare effects of FPS games
Roll 0 deg. = cam.u parallel with XZ plane, cam.v points upwards
Roll +90 deg. = cam.v parallel with XZ plane, cam.u points downwards
Roll +-180 deg. = cam.u parallel with XZ plane, cam.v points downwards (= you're upside down)
Roll -90 deg. = cam.v parallel with XZ plane, cam.u points upwards
In space games the angles have no physical meaning, but in the coordinate system this angle is an "Euler" angle.
*/
double Transformation3::getRoll() const
{
  //roll is the angle between the plane (world_up, camera_dir) and the plane (camera_up, camera_dir)
  //the angle between two planes is the angle between their normals
  //the normals are gotten with cross products
  //the vectorAngle function uses acos and dot product
  
  if(planetGroundPlaneType == XZ)
  {
    double roll = vectorAngle(cross(Vector3(0, 1, 0), trans[2]), cross(trans[1], trans[2]));
    if(trans[0].y < 0) roll = -roll;
    return roll;
  }
  else /*if(planetGroundPlaneType == XY)*/
  {
    double roll = vectorAngle(cross(Vector3(0, 0, 1), trans[2]), cross(trans[1], trans[2]));
    if(trans[0].z < 0) roll = -roll;
    return roll;
  }
}

void Transformation3::setRoll(double angle)
{
  double currentAngle = getRoll();
  //to change roll, you have to rotate around the direction vector of the camera
  rotate(trans[2], angle - currentAngle);
}

//rolls the camera by rotating it around the direction vector (only makes sense in space or for "shaking" effects)
void Transformation3::roll(double angle)
{
  rotate(trans[2], angle);
}

//makes u, v and dir perpendicular by using cross product, maintains exact direction and roll if only v was skewed
void Transformation3::resetSkewU()
{
  double oldZoomU = getZoomU(); 
  double oldZoomV = getZoomV();
  trans[0] = cross(trans[2], trans[1]);
  trans[1] = cross(trans[2], -trans[0]);
  setZoomU(oldZoomU);
  setZoomV(oldZoomV);
  generateMatrix();
}

//makes u, v and dir perpendicular by using cross product, maintains exact direction and roll if only u was skewed
void Transformation3::resetSkewV()
{
  double oldZoomU = getZoomU(); 
  double oldZoomV = getZoomV();
  trans[1] = cross(trans[2], trans[0]);
  trans[0] = cross(trans[2], -trans[1]);
  setZoomU(oldZoomU);
  setZoomV(oldZoomV);
  generateMatrix();
}

//get and set screen ratios of the camera (ratio of length of u and v, e.g. 4:3, 16:9, 640:480, ...)
double Transformation3::getRatioUV() const
{
  return trans[0].length() / trans[1].length();
}

double Transformation3::getRatioVU() const
{
  return trans[1].length() / trans[0].length();
}

//changes V  
void Transformation3::setRatioUV(double ratio)
{
  trans[1].normalize();
  trans[1] = trans[1] * trans[0].length() / ratio;
  generateMatrix();
}

//changes U
void Transformation3::setRatioVU(double ratio)
{
  trans[0].normalize();
  trans[0] = trans[0] * (trans[1].length() / ratio);
  generateMatrix();
}

//scale U, V and Dir without changing what you see
double Transformation3::getScale() const
{
  return trans[2].length();
}

void Transformation3::setScale(double dirLength)
{
  scale(trans[2].length() / dirLength);
}

void Transformation3::scale(double factor) 
{
  trans *= factor;
  generateMatrix();
}

void Transformation3::generateMatrix()
{
  //instead of generating it here, it's just set that it's not up to date anymore and it'll be generated only as soon as it's actually needed, in the get functions. Always use the get functions, even inside this class, to make sure up to date matrices are always used.
  invMatrixUpToDate = false;
}

const Matrix3& Transformation3::getMatrix() const
{
  return trans;
}

Matrix3 Transformation3::getInvMatrix() const
{
  if(!invMatrixUpToDate)
  {
    //this is the inverse of the trans, to use for transformations
    invCamMatrix = inverse(getMatrix());
    invMatrixUpToDate = true;
  }
  return invCamMatrix;
}

void Transformation3::setMatrix(const Matrix3& matrix)
{
  //to make sure the vectors and the matrix are according to each other, set the vectors to the given matrix 
  //and then use generateMatrix with these new vectors
  trans = matrix;
  generateMatrix();
}

Vector3 Transformation3::transformWithoutPos(const Vector3& v) const
{
  /*
  Transformation from coordinate system 1 (base 1) to coordinate system 2 (base 2):
  
  vector v = coordinates of point p in base 1 (column vector)
  matrix A = the 3 vectors representing coordinates of unit vectors of base 2 in base 1 (3 column vectors in matrix)
  vector w = coordinates of point p in base 2(column vector)
  
  Then: v = A * w
        w = A^(-1) * v
  
  Now: base 1 = worldspace, base 2 = cameraspace
  v = coordinates of point in worldspace (the given v )
  A = the matrix from the camera: the coordinates of the camera vectors in worldspace
  w = coordinates of point in cameraspace
  
  So our transformation is w = A^(-1) * v, where v is the parameter given to the function, and A = perspective matrix of camera
  
  All this text to describe only one line of code...
  */
  
  return getInvMatrix() * v;
}

Vector3 Transformation3::transform(const Vector3& v) const
{
  Vector3 a = v - pos;
  return transformWithoutPos(a);
}

//Auxiliary Functions

//Swap between radians and degrees
double radToDeg(double rad)
{
  return 360.0 * rad / (pi * 2.0);
}

double degToRad(double deg)
{
  return (pi * 2.0) * deg / 360.0;
}

Vector3 normalOfTriangle(const Vector3& a, const Vector3& b, const Vector3& c)
{
  return cross(b - a, c - a);
}

/*
Project vector v on the plane described by n (n is the normal on the plane, all vectors and the plane are through the origin)
*/
Vector3 projectOnPlane(const Vector3& v, const Vector3& n)
{
  Vector3 nn = normalize(n); //normalized normal
  //first project v on n by using the dot project
  double d = dot(v, nn);
  Vector3 nproj = d * nn;
  
  //then subract this from v to project it on the plane
  Vector3 result = v - nproj;
  
  return result;
}

/*
Returns either true or false, depending on which side of plane XY vector a is
*/
bool sideOfPlaneGivenByTwoVectors(const Vector3& p, const Vector3& X, const Vector3& Y)
{
  //get the Z axis
  Vector3 Z = cross(X, Y);
  
  //project a on the Z axis
  double pproj = dot(p, Z);
  
  //if it's negative, it's on the one site, if it's positive, it's on the other side
  return (pproj >= 0);
}

//I want this to return true if you're on the side where you see the points in clockwise order, false on the other side, and true if it's on the plane
bool sideOfPlaneGivenByThreePoints(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
  Vector3 p2 = p - a;
  Vector3 b2 = b - a;
  Vector3 c2 = c - a;
  Vector3 a2 = a - a;
  
  Vector3 n = normalOfTriangle(a2, b2, c2);
  
  double proj = dot(p2, n);
  
  return (proj >= 0);
}

bool sideOfPlaneGivenByNormal(const Vector3& p, const Vector3& n)
{
  double pproj = dot(p, n);
  
  //if it's negative, it's on the one site, if it's positive, it's on the other side
  return (pproj >= 0);
}

double angleWithPlaneGivenByTwoVectors(const Vector3& v, const Vector3& X, const Vector3& Y)
{
  return vectorAngle(v, cross(X, Y)) - pi / 2.0;
}

/*
Returns the angle or rotation vector a has compared to vector b, around the axis, it's a value between 0 and 2pi so the full 360 degrees are supported
*/
double rotationAroundAxis(const Vector3& a, const Vector3& b, const Vector3& axis)
{
  Vector3 ap = projectOnPlane(a, axis);
  Vector3 bp = projectOnPlane(b, axis);
  double angle = vectorAngle(ap, bp); //now the angle is between 0 and pi, so / gives the same as \. I want it to go from 0 to 360 degrees. So now it's time to convert it to that... ER KLOPT IETS NI WANT VECTORANGLE IS VAN -Pi TOT 0 EN NI VAN 0 TOT whatever ik hier verwacht, wtf is hier toch allemaal aan de hand
  
  if(sideOfPlaneGivenByTwoVectors(ap, bp, axis)) angle = 6.283185307179586477025617918812372408865 - angle;//da mag nooit < 0 of > 6.28... worden en TOCH doet het dat!!!!!!
  return angle;
}

//returns the component of v in the direction dir (dir must be normalized, otherwise the factor is wrong)
Vector3 getComponentInDirection(const Vector3& v, const Vector3& dir)
{
  /*Vector3 dirn = normalize(dir);
  double mag = dot(v, dirn);
  Vector3 proj = mag * dirn;
  return proj;*/
  
  return dir * dot(v, dir);
}

//returns if the angle between the two vectors is larger than 90 degrees or not, if it's larger they have opposite direction
bool hasOppositeDirection(const Vector3& v, const Vector3& w)
{
  return dot(v, w) < 0.0;
}

//get distance from point p to the line given by a and b
double distance(const Vector3& p, const Vector3& a, const Vector3& b)
{
  /*methode: bepaal vlak loodrecht op de rechte en door p; bepaal q = doorsnede van vlak en rechte; result = distance(p, q);
  bepalen van dat vlak: de formule van dat vlak is: (bx-ax)*(x-px) + (by-ay)*(y-py) + (bz-az)*(z-pz) = 0
  de doorsnede van dat vlak en de rechte ab is dan de x, y, z die op de rechte ligt en aan de bovenstaande vergelijking voldoet;
  welke q =(qx, qy, qz) ligt op de rechte? die van de vorm q = a + k * (b - a) met k een scalar
  
  dus het stelsel om q te zoeken is:
  qx = ax + k * (bx - ax)
  qy = ay + k * (by - ay)
  qz = az + k * (bz - az)
  (bx-ax)*(qx-px) + (by-ay)*(qy-py) + (bz-az)*(qz-pz) = 0 //de vergelijking van het vlak
  
  //substitueer de qx, qy, en qz in de laatste vergelijking van het vlak
  (bx-ax)*(ax + k * (bx - ax)-px) + (by-ay)*(ay + k * (by - ay)-py) + (bz-az)*(az + k * (bz - az)-pz) = 0
  
  //nu dit oplossen naar k
  (bx-ax)*(bx-ax)*k + (bx-ax)*(ax-px) + (by-ay)*(by-ay)*k + (by-ay)*(ay-py) + (bz-az)*(bz-az)*k + (bz-az)*(az-pz) = 0
  ==>
  k * [(bx-ax)^2 + (by-ay)^2 + (bz-az)^2] + [(bx-ax)*(ax-px)+(by-ay)*(ay-py)+(bz-az)*(az-pz)] = 0
  ==>
  k = -[(bx-ax)*(ax-px)+(by-ay)*(ay-py)+(bz-az)*(az-pz)]/[(bx-ax)^2 + (by-ay)^2 + (bz-az)^2]
  
  en dan nu nog vinden welke qx, qy en qz daarmee overeenkomen: gewoon k invullen in die 3 vgls van de rechte
  */
  
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y)+(b.z-a.z)*(a.z-p.z))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z));
  Vector3 q;
  q.x = a.x + k * (b.x - a.x);
  q.y = a.y + k * (b.y - a.y);
  q.z = a.z + k * (b.z - a.z);
  
  return distance(p, q);
}



//gives the distance between point p and the plane given by 3 points a, b, c
double distance(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
  //equation of plane: ux+vy+wz+t = 0
  double u, v, w, t;
  planeEquation(a, b, c, u, v, w, t);
  
  double distance = std::abs(u * p.x + v * p.y + w * p.z + t) / std::sqrt(u * u + v * v + w * w); //"template_abs", because I'm so sick of this confusing abs, fabs, ... that take specific types
  
  return distance;
}

/*test if part of line between a and b goes through sphere given by pos p and it's radius*/
bool linesegmentThroughSphere(const Vector3& a, const Vector3& b, const Vector3& p, double radius)
{
  //if one of the endpoints is inside the sphere, it certainly is through the sphere
  if(distance(p, a) < radius) return true;
  if(distance(p, b) < radius) return true;
  
  double k = -((b.x-a.x)*(a.x-p.x)+(b.y-a.y)*(a.y-p.y)+(b.z-a.z)*(a.z-p.z))/((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z));
  Vector3 q;
  q.x = a.x + k * (b.x - a.x);
  q.y = a.y + k * (b.y - a.y);
  q.z = a.z + k * (b.z - a.z);
  
  if(k > 0.0 && k < 1.0 && distance(p, q) < radius) return true; 
  else return false;
  //de test is fout!!!! het is niet omdat k niet tussen 0 en 1 ligt, dat er ni nog een stuk van het lijnstuk door de bol kan gaan!
  /*
  laten we het eens zo proberen:
  als k > 1.0, test of het punt bij k = 1.0 in de bol ligt
  als k < 0.0, test of het punt bij k = 0.0 in de bol ligt
  in dat gevan snijdt het lijnstuk ook de bol
  */
}

/*
calculates u, v, w and t of plane equation ux+vy+wz+t=0 of plane given by 3 points a, b and c
*/
void planeEquation(const Vector3& a, const Vector3& b, const Vector3& c, double& u, double& v, double& w, double& t)
{
  u = (a.y - b.y) * (a.z - c.z) - (a.z - b.z) * (a.y - c.y);
  v = (a.z - b.z) * (a.x - c.x) - (a.x - b.x) * (a.z - c.z);
  w = (a.x - b.x) * (a.y - c.y) - (a.y - b.y) * (a.x - c.x);
  t = -(a.x * u + a.y * v + a.z * w);
}

Vector3 linePlaneIntersection(const Vector3& a, const Vector3& b, const Vector3& p, const Vector3& q, const Vector3& r)
{
  double u, v, w, t;
  planeEquation(p, q, r, u, v, w, t);
  
  //vergelijking vlak is nu: ux + vy + wz + t = 0
  /*vergelijking rechte:
  x = a.x + k * (b.x - a.x)
  y = a.y + k * (b.y - a.y)
  z = a.z + k * (b.z - a.z)
  */
  /*
  vul vergelijking rechte in in die van vlak ==>
  
  u * (a.x + k * (b.x - a.x)) + v * (a.y + k * (b.y - a.y)) + w * (a.z + k * (b.z - a.z)) + t = 0
  ==> k * [u * (b.x - a.x) + v * (b.y - a.y) + w * (b.z - a.z)] + [u * a.x + v * a.y + w * a.z + t] = 0
  
  ==> k = -[u * a.x + v * a.y + w * a.z + t] / [u * (b.x - a.x) + v * (b.y - a.y) + w * (b.z - a.z)]
  */
  double k = -(u * a.x + v * a.y + w * a.z + t) / (u * (b.x - a.x) + v * (b.y - a.y) + w * (b.z - a.z));
  
  Vector3 result;
  result.x = a.x + k * (b.x - a.x);
  result.y = a.y + k * (b.y - a.y);
  result.z = a.z + k * (b.z - a.z);
  
  return result;
}

//given a point p on the triangle a, b, c. Find the weights wa, wb, wc of the point p in the triangle's coordinate system. E.G. if p is lying on a, the weights will be (1, 0, 0). If p is lying exactly in the center of the triangle, the weights will be (0.33, 0.33, 0.33). The 3 values of the weight are stored in the returned vector's x, y and z component respectively. The sum of the 3 weights is always 1. The point p must lie on the same plane as the triangle (a, b, c and d coplanar). wa, wb and wc are actually normalized baricentric coordinates.
//precondition: a, b, c, p must be coplanar. a, b, c may not be colinear.
Vector3 barycentric(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p)
{
  Vector3 na = cross(c - b, p - b);
  Vector3 nb = cross(a - c, p - c);
  Vector3 nc = cross(b - a, p - a);
  Vector3 n = cross(b - a, c - a);
  
  double nn = dot(n, n);
  
  Vector3 result;
  result.x = dot(n, na) / nn; //alpha
  result.y = dot(n, nb) / nn; //beta
  result.z = dot(n, nc) / nn; //gamma
  
  return result;
}

//non-perspective correct to perspective correct; beta2 and gamma2 are non-perspective correct barycentric values
//the non-perspective correct beta2 and gamma2 are gotten from lpi_math2d's 2D barycentric formula for rasterizing triangles
void barycentric_persp(double& alpha, double& beta, double& gamma, /*double alpha2,*/ double beta2, double gamma2, double za, double zb, double zc)
{
   double denom = zb * zc + zc * beta2 * (za - zb) + zb * gamma2 * (za - zc);
   double invdenom = 1.0 / denom;
   beta = za * zc * beta2 * invdenom;
   gamma = za * zb * gamma2 * invdenom;
   alpha = 1.0 - beta - gamma;
}

/*
What this function does:
Suppose there's a plane with equation Z = 0, so it's an XY plane. Point p has a position in the same space as this plane. Then the plane rotates around the point (0, 0, 0) so that it has a new normal, given by n. p follows the same rotation as the plane. The new position p has is returned.

*/
Vector3 rotateWithPlane(const Vector3& p, const Vector3& n)
{
  Vector3 oldnormal = Vector3(0, 0, 1); //n is the new normal
  Vector3 axis = cross(oldnormal, n); //the axis around which the rotation will be done
  if(axis.length() == 0) axis = getSomePerpendicularVector(n);
  
  double angle = -vectorAngle(n, oldnormal);
  Vector3 result = rotateAroundArbitrary(p, axis, angle);
  
  return result;
}

/*
in 2D there is the problem to find a line through a point p that is tangent to a circle. 
This function does exactly the same but with a plane through a point, and a sphere.
The plane also has an extra condition, it must be parallel with given vector v (the up vector of the camera).
Basically, this can be used to find the position on the right of the sphere projected on screen that is the perfect position to draw some text indicating the name of a planet or so, so that the label is next to the sphere and not on top of it.
The function returns the position of the intersection between the plane and the sphere.

p = position plane has to go through
o = center of sphere
v = vector plane has to be parallel with (up vector of camera in useful cases)
radius = radius of the sphere
*/
Vector3 planeSphereTangent(const Vector3& p, const Vector3& o, double radius, const Vector3& v)
{
  //o, i, p is a right triangle (the corner at i is 90 degrees). That is used to find angles and lengths.
  double OP = distance(o, p); //known
  double OI = radius; //known
  double PI = std::sqrt(OP * OP - OI * OI); //calculated from the two knowns with pythagoras
  double ucor = PI / OP; //de cosinus: aanliggende gedeeld door schuine
  double dircor = OI / OP; //de sinus: overstaande gedeeld door schuine
  Vector3 lookdir = normalize(o - p);
  Vector3 udir = normalize(cross(v, lookdir));
  Vector3 I = PI * (dircor * udir + ucor * lookdir); //the final point of intersection
  
  return I;
}

std::ostream& operator<<(std::ostream& ostr, const Vector3& v)
{
  return ostr << "[" << v.x << " " << v.y << " " << v.z << "]";
}

std::ostream& operator<<(std::ostream& ostr, const Matrix3& m)
{
  return ostr << "[" << m.a[0][0] << " " << m.a[1][0] << " " << m.a[2][0] << " ; " << m.a[0][1] << " " << m.a[1][1] << " " << m.a[2][1] << " ; " << m.a[0][2] << " " << m.a[1][2] << " " << m.a[2][2] << "]";
}

////////////////////////////////////////////////////////////////////////////////

bool deflect(Vector3& dir, const Vector3& shooterpos, const Vector3& targetpos, const Vector3& vel, double speed)
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
      
      Vector3 intercept = targetpos + t * vel;
      dir = normalize(intercept - shooterpos);
      return true;
    }
  }
}

} //end of namespace lpi
