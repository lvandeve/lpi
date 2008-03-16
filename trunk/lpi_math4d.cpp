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

#include "lpi_math4d.h"

#include <iostream>

namespace lpi
{

Vector4::Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w)
{
}

Vector4::Vector4() /*: x(0.0), y(0.0), z(0.0), w(0.0)*/
{
}

void Vector4::convertTo(Vector3& v)
{
  v.x = x / w;
  v.y = y / w;
  v.z = z / w;
}

void Vector4::convertFrom(const Vector3& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = 1.0;
}

Vector4& Vector4::operator+=(const Vector4& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

Vector4& Vector4::operator*=(double a)
{
  x *= a;
  y *= a;
  z *= a;
  w *= a;
  return *this;
}

Vector4& Vector4::operator/=(double a)
{
  x /= a;
  y /= a;
  z /= a;
  w /= a;
  return *this;
}

//Add two vectors
Vector4 operator+(const Vector4& v, const Vector4& w)
{
  Vector4 u = v;
  u += w;
  return u;
}

//Multiplies a vector by a scalar
Vector4 operator*(const Vector4& v, double a)
{
  Vector4 w = v;
  w *= a;
  return w;
}

//Multiplies a vector by a scalar
Vector4 operator*(double a, const Vector4& v)
{
  Vector4 w = v;
  w *= a;
  return w;
}

//Divides a vector through a scalar
Vector4 operator/(const Vector4& v, double a)
{
  Vector4 w = v;
  w /= a;
  return w;
}

////////////////////////////////////////////////////////////////////////////////

Matrix4::Matrix4() {}

Matrix4::Matrix4(const Matrix4& m)
{
  for(int i = 0; i < 4; i++) a[i] = m.a[i];
}

void Matrix4::operator=(const Matrix4& m)
{
  for(int i = 0; i < 4; i++) a[i] = m.a[i];
}

void Matrix4::transpose()
{
  std::swap(a[0][1], a[1][0]);
  std::swap(a[0][2], a[2][0]);
  std::swap(a[0][3], a[3][0]);
  std::swap(a[1][2], a[2][1]);
  std::swap(a[1][3], a[3][1]);
  std::swap(a[2][3], a[3][2]);
}

void Matrix4::subMatrix(Matrix3& out, int i, int j )
{
  int di, dj, si, sj;
  // loop through 3x3 submatrix
  for( di = 0; di < 3; di ++ )
  for( dj = 0; dj < 3; dj ++ )
  {
    // map 3x3 element (destination) to 4x4 element (source)
    si = di + ( ( di >= i ) ? 1 : 0 );
    sj = dj + ( ( dj >= j ) ? 1 : 0 );
    // copy element
    out.a[di][dj] = a[si][sj];
  }
}

double Matrix4::determinant()
{
  double det, result = 0, i = 1;
  Matrix3 msub3;
  for(int n = 0; n < 4; n++, i *= -1)
  {
    subMatrix(msub3, 0, n);
    det = msub3.determinant();
    result += a[0][n] * det * i;
  }
  return(result);
}

void Matrix4::invert()
{
  double mdet = determinant();
  Matrix3 mtemp;
  int sign;
  
  Matrix4 result;

  for (int i = 0; i < 4; i++ )
  for (int j = 0; j < 4; j++ )
  {
    sign = 1 - ((i + j) % 2) * 2;
    subMatrix(mtemp, i, j);
    result.a[i][j] = (mtemp.determinant() * sign) / mdet;
  }
  
  *this = result;
}


Matrix4 operator*(const Matrix4& A, const Matrix4& B)
{
  Matrix4 C;
  C.a[0][0] = A.a[0][0]*B.a[0][0] + A.a[1][0]*B.a[0][1] + A.a[2][0]*B.a[0][2] + A.a[3][0]*B.a[0][3];
  C.a[0][1] = A.a[0][1]*B.a[0][0] + A.a[1][1]*B.a[0][1] + A.a[2][1]*B.a[0][2] + A.a[3][1]*B.a[0][3];
  C.a[0][2] = A.a[0][2]*B.a[0][0] + A.a[1][2]*B.a[0][1] + A.a[2][2]*B.a[0][2] + A.a[3][2]*B.a[0][3];
  C.a[0][3] = A.a[0][3]*B.a[0][0] + A.a[1][3]*B.a[0][1] + A.a[2][3]*B.a[0][2] + A.a[3][3]*B.a[0][3];
  C.a[1][0] = A.a[0][0]*B.a[1][0] + A.a[1][0]*B.a[1][1] + A.a[2][0]*B.a[1][2] + A.a[3][0]*B.a[1][3];
  C.a[1][1] = A.a[0][1]*B.a[1][0] + A.a[1][1]*B.a[1][1] + A.a[2][1]*B.a[1][2] + A.a[3][1]*B.a[1][3];
  C.a[1][2] = A.a[0][2]*B.a[1][0] + A.a[1][2]*B.a[1][1] + A.a[2][2]*B.a[1][2] + A.a[3][2]*B.a[1][3];
  C.a[1][3] = A.a[0][3]*B.a[1][0] + A.a[1][3]*B.a[1][1] + A.a[2][3]*B.a[1][2] + A.a[3][3]*B.a[1][3];
  C.a[2][0] = A.a[0][0]*B.a[2][0] + A.a[1][0]*B.a[2][1] + A.a[2][0]*B.a[2][2] + A.a[3][0]*B.a[2][3];
  C.a[2][1] = A.a[0][1]*B.a[2][0] + A.a[1][1]*B.a[2][1] + A.a[2][1]*B.a[2][2] + A.a[3][1]*B.a[2][3];
  C.a[2][2] = A.a[0][2]*B.a[2][0] + A.a[1][2]*B.a[2][1] + A.a[2][2]*B.a[2][2] + A.a[3][2]*B.a[2][3];
  C.a[2][3] = A.a[0][3]*B.a[2][0] + A.a[1][3]*B.a[2][1] + A.a[2][3]*B.a[2][2] + A.a[3][3]*B.a[2][3];
  C.a[3][0] = A.a[0][0]*B.a[3][0] + A.a[1][0]*B.a[3][1] + A.a[2][0]*B.a[3][2] + A.a[3][0]*B.a[3][3];
  C.a[3][1] = A.a[0][1]*B.a[3][0] + A.a[1][1]*B.a[3][1] + A.a[2][1]*B.a[3][2] + A.a[3][1]*B.a[3][3];
  C.a[3][2] = A.a[0][2]*B.a[3][0] + A.a[1][2]*B.a[3][1] + A.a[2][2]*B.a[3][2] + A.a[3][2]*B.a[3][3];
  C.a[3][3] = A.a[0][3]*B.a[3][0] + A.a[1][3]*B.a[3][1] + A.a[2][3]*B.a[3][2] + A.a[3][3]*B.a[3][3];
  return C;
}

Vector3 operator*(const Matrix4& A, const Vector3& v)
{
  Vector3 result;
  double w;
  result.x = A.a[0][0] * v.x + A.a[1][0] * v.y + A.a[2][0] * v.z + A.a[3][0];
  result.y = A.a[0][1] * v.x + A.a[1][1] * v.y + A.a[2][1] * v.z + A.a[3][1];
  result.z = A.a[0][2] * v.x + A.a[1][2] * v.y + A.a[2][2] * v.z + A.a[3][2];
         w = A.a[0][3] * v.x + A.a[1][3] * v.y + A.a[2][3] * v.z + A.a[3][3];
  result /= w;
  return result;
}

Vector4 operator*(const Matrix4& A, const Vector4& v)
{
  Vector4 result;
  result.x = A.a[0][0] * v.x + A.a[1][0] * v.y + A.a[2][0] * v.z + A.a[3][0];
  result.y = A.a[0][1] * v.x + A.a[1][1] * v.y + A.a[2][1] * v.z + A.a[3][1];
  result.z = A.a[0][2] * v.x + A.a[1][2] * v.y + A.a[2][2] * v.z + A.a[3][2];
  result.w = A.a[0][3] * v.x + A.a[1][3] * v.y + A.a[2][3] * v.z + A.a[3][3];
  return result;
}

Matrix4 operator*(const Matrix4& A, double d)
{
  Matrix4 result;
  for(int i = 0; i < 4; i++) result.a[i] = d * A.a[i];
  return result;
}

Matrix4 operator*(double d, const Matrix4& A)
{
  Matrix4 result;
  for(int i = 0; i < 4; i++) result.a[i] = d * A.a[i];
  return result;
}

void getTranslationMatrix4(Matrix4& m, const Vector3& d)
{
  m.a[0] = m.a[1] = m.a[2] = Vector4_0;
  m.a[3].x = d.x;
  m.a[3].y = d.y;
  m.a[3].z = d.z;
  m.a[3].w = 1;
}

//returns a rotation matrix that rotates stuff by multiplying with this returned matrix, where you put this returned matrix on the left side of the multiplication.
void getRotationMatrix4(Matrix4& m, const Vector3& axis, double angle)
{
  double c, s, t;

  Vector3 axisn = normalize(axis);

  //calculate parameters of the rotation matrix
  c = std::cos(angle);
  s = std::sin(angle);
  t = 1 - c;
  
  m.a[0][0] = t * axisn.x * axisn.x +           c; m.a[0][1] = t * axisn.x * axisn.y + s * axisn.z; m.a[0][2] = t * axisn.x * axisn.z - s * axisn.y; m.a[0][3] = 0.0;
  m.a[1][0] = t * axisn.x * axisn.y - s * axisn.z; m.a[1][1] = t * axisn.y * axisn.y +           c; m.a[1][2] = t * axisn.y * axisn.z + s * axisn.x; m.a[1][3] = 0.0;
  m.a[2][0] = t * axisn.x * axisn.z + s * axisn.y; m.a[2][1] = t * axisn.y * axisn.z - s * axisn.x; m.a[2][2] = t * axisn.z * axisn.z +           c; m.a[2][3] = 0.0;
  m.a[3][0] = 0.0;                                 m.a[3][1] = 0.0;                                 m.a[3][2] = 0.0;                                 m.a[3][3] = 1.0;
}

void makeIdentity(Matrix4& m)
{
  m.a[0][0] = m.a[1][1] = m.a[2][2] = m.a[3][3] = 1.0;
  m.a[1][0] = m.a[2][0] = m.a[3][0] = m.a[0][1] = 0.0;
  m.a[2][1] = m.a[3][1] = m.a[0][2] = m.a[1][2] = 0.0;
  m.a[3][2] = m.a[0][3] = m.a[1][3] = m.a[2][3] = 0.0;
}

} //namespace lpi

