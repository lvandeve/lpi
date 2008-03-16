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

#ifndef LPI_MATH4D_H_INCLUDED
#define LPI_MATH4D_H_INCLUDED

#include "lpi_math3d.h"


namespace lpi
{

////////////////////////////////////////////////////////////////////////////////

//4x4 math. It's a bit chaotically structured at the moment. It's unfinished.

class Vector4
{
  public:
  double x;
  double y;
  double z;
  double w;
  
  Vector4();
  Vector4(double x, double y, double z, double w);
  
  const double& operator[](int i) const { return *(&x + i); }
  double& operator[](int i) { return *(&x + i); }
  
  void convertTo(Vector3& v);
  void convertFrom(const Vector3& v);
  
  //NOTE: these operators affect w too! This is important to make the Matrix4 implementation that uses Vector4 work correctly
  //NOTE: this means these operators can't be used if you treat the Vector4 as homogeneous
  Vector4& operator+=(const Vector4& v);
  Vector4& operator-=(const Vector4& v);
  Vector4& operator*=(double a);
  Vector4& operator/=(double a);
};

Vector4 operator-(const Vector4& v, const Vector4& w);
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v, const Vector4& w);
Vector4 operator*(const Vector4& v, double a);
Vector4 operator*(double a, const Vector4& v);
Vector4 operator/(const Vector4& v, double a);

static const Vector4 Vector4_origin = Vector4(0.0, 0.0, 0.0, 1.0); //0001
static const Vector4 Vector4_0 =      Vector4(0.0, 0.0, 0.0, 0.0); //0000
static const Vector4 Vector4_x =      Vector4(1.0, 0.0, 0.0, 1.0); //1001
static const Vector4 Vector4_y =      Vector4(0.0, 1.0, 0.0, 1.0); //0101
static const Vector4 Vector4_z =      Vector4(0.0, 0.0, 1.0, 1.0); //0011

class Matrix4
{
  public:
  
  Vector4 a[4];
  
  Matrix4();
  Matrix4(const Matrix4& m);
  void operator=(const Matrix4& m);
  void transpose();
  void subMatrix(Matrix3& out, int i, int j );
  double determinant();
  void invert();
};

Matrix4 operator*(const Matrix4& A, const Matrix4& B);
Vector3 operator*(const Matrix4& A, const Vector3& v); //mathematically it may look incorrect, BUT, useful :)
Vector4 operator*(const Matrix4& A, const Vector4& v);
Matrix4 operator*(const Matrix4& A, double d);
Matrix4 operator*(double d, const Matrix4& A);
void getTranslationMatrix4(Matrix4& m, const Vector3& d);
//returns a rotation matrix that rotates stuff by multiplying with this returned matrix, where you put this returned matrix on the left side of the multiplication.
void getRotationMatrix4(Matrix4& m, const Vector3& axis, double angle);
void makeIdentity(Matrix4& m);


} //namespace lpi


#endif

