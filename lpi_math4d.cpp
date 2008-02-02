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

Matrix4::Matrix4() {}

Matrix4::Matrix4(const Matrix4& m)
{
  for(int i = 0; i < 16; i++) a[i] = m.a[i];
}

void Matrix4::operator=(const Matrix4& m)
{
  for(int i = 0; i < 16; i++) a[i] = m.a[i];
}

void Matrix4::transpose()
{
  for(int i = 0; i < 16; i++)
  {
    int j = 4 * (i % 4) + (i / 4);
    if(i == j) continue;
    double temp = a[i];
    a[i] = a[j];
    a[j] = temp;
  }
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
    out.a[di][dj] = a[si * 4 + sj];
  }
}

double Matrix4::determinant()
{
  double det, result = 0, i = 1;
  Matrix3 msub3;
  int n;
  for ( n = 0; n < 4; n++, i *= -1 )
  {
    subMatrix(msub3, 0, n);
    det = msub3.determinant();
    result += a[n] * det * i;
  }
  return(result);
}

void Matrix4::invert()
{
  double mdet = determinant();
  Matrix3 mtemp;
  int sign;
  
  double result[16];

  for (int i = 0; i < 4; i++ )
  for (int j = 0; j < 4; j++ )
  {
    sign = 1 - ((i + j) % 2) * 2;
    subMatrix(mtemp, i, j);
    result[i+j*4] = (mtemp.determinant() * sign) / mdet;
  }
  
  for(int i = 0; i < 16; i++) a[i] = result[i];
}


Matrix4 operator*(const Matrix4& A, const Matrix4& B)
{
  Matrix4 C;
  C.a[ 0] = A.a[0]*B.a[0]  + A.a[4]*B.a[1]  + A.a[8] *B.a[2]  + A.a[12]*B.a[3];
  C.a[ 1] = A.a[1]*B.a[0]  + A.a[5]*B.a[1]  + A.a[9] *B.a[2]  + A.a[13]*B.a[3];
  C.a[ 2] = A.a[2]*B.a[0]  + A.a[6]*B.a[1]  + A.a[10]*B.a[2]  + A.a[14]*B.a[3];
  C.a[ 3] = A.a[3]*B.a[0]  + A.a[7]*B.a[1]  + A.a[11]*B.a[2]  + A.a[15]*B.a[3];
  C.a[ 4] = A.a[0]*B.a[4]  + A.a[4]*B.a[5]  + A.a[8] *B.a[6]  + A.a[12]*B.a[7];
  C.a[ 5] = A.a[1]*B.a[4]  + A.a[5]*B.a[5]  + A.a[9] *B.a[6]  + A.a[13]*B.a[7];
  C.a[ 6] = A.a[2]*B.a[4]  + A.a[6]*B.a[5]  + A.a[10]*B.a[6]  + A.a[14]*B.a[7];
  C.a[ 7] = A.a[3]*B.a[4]  + A.a[7]*B.a[5]  + A.a[11]*B.a[6]  + A.a[15]*B.a[7];
  C.a[ 8] = A.a[0]*B.a[8]  + A.a[4]*B.a[9]  + A.a[8] *B.a[10] + A.a[12]*B.a[11];
  C.a[ 9] = A.a[1]*B.a[8]  + A.a[5]*B.a[9]  + A.a[9] *B.a[10] + A.a[13]*B.a[11];
  C.a[10] = A.a[2]*B.a[8]  + A.a[6]*B.a[9]  + A.a[10]*B.a[10] + A.a[14]*B.a[11];
  C.a[11] = A.a[3]*B.a[8]  + A.a[7]*B.a[9]  + A.a[11]*B.a[10] + A.a[15]*B.a[11];
  C.a[12] = A.a[0]*B.a[12] + A.a[4]*B.a[13] + A.a[8] *B.a[14] + A.a[12]*B.a[15];
  C.a[13] = A.a[1]*B.a[12] + A.a[5]*B.a[13] + A.a[9] *B.a[14] + A.a[13]*B.a[15];
  C.a[14] = A.a[2]*B.a[12] + A.a[6]*B.a[13] + A.a[10]*B.a[14] + A.a[14]*B.a[15];
  C.a[15] = A.a[3]*B.a[12] + A.a[7]*B.a[13] + A.a[11]*B.a[14] + A.a[15]*B.a[15];
  return C;
}

Vector3 operator*(const Matrix4& A, const Vector3& v)
{
  Vector3 result;
  double w;
  result.x = A.a[0] * v.x + A.a[4] * v.y + A.a[8]  * v.z + A.a[12];
  result.y = A.a[1] * v.x + A.a[5] * v.y + A.a[9]  * v.z + A.a[13];
  result.z = A.a[2] * v.x + A.a[6] * v.y + A.a[10] * v.z + A.a[14];
         w = A.a[3] * v.x + A.a[7] * v.y + A.a[11] * v.z + A.a[15];
  result /= w;
  return result;
}

Vector4 operator*(const Matrix4& A, const Vector4& v)
{
  Vector4 result;
  result.x = A.a[0] * v.x + A.a[4] * v.y + A.a[8]  * v.z + A.a[12];
  result.y = A.a[1] * v.x + A.a[5] * v.y + A.a[9]  * v.z + A.a[13];
  result.z = A.a[2] * v.x + A.a[6] * v.y + A.a[10] * v.z + A.a[14];
  result.w = A.a[3] * v.x + A.a[7] * v.y + A.a[11] * v.z + A.a[15];
  return result;
}

Matrix4 operator*(const Matrix4& A, double d)
{
  Matrix4 result;
  for(int i = 0; i < 16; i++) result.a[i] = d * A.a[i];
  return result;
}

Matrix4 operator*(double d, const Matrix4& A)
{
  Matrix4 result;
  for(int i = 0; i < 16; i++) result.a[i] = d * A.a[i];
  return result;
}

void getTranslationMatrix4(Matrix4& m, const Vector3& d)
{
  m.a[0] = 1; m.a[4] = 0; m.a[8] = 0; m.a[12] = d.x;
  m.a[1] = 0; m.a[5] = 1; m.a[9] = 0; m.a[13] = d.y;
  m.a[2] = 0; m.a[6] = 0; m.a[10] = 1; m.a[14] = d.z;
  m.a[3] = 0; m.a[7] = 0; m.a[11] = 0; m.a[15] = 1;
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
  
  m.a[0] = t * axisn.x * axisn.x +           c; m.a[4] = t * axisn.x * axisn.y + s * axisn.z; m.a[8]  = t * axisn.x * axisn.z - s * axisn.y; m.a[12] = 0.0;
  m.a[1] = t * axisn.x * axisn.y - s * axisn.z; m.a[5] = t * axisn.y * axisn.y +           c; m.a[9]  = t * axisn.y * axisn.z + s * axisn.x; m.a[13] = 0.0;
  m.a[2] = t * axisn.x * axisn.z + s * axisn.y; m.a[6] = t * axisn.y * axisn.z - s * axisn.x; m.a[10] = t * axisn.z * axisn.z +           c; m.a[14] = 0.0;
  m.a[3] = 0.0;                                 m.a[7] = 0.0;                                 m.a[11] = 0.0;                                 m.a[15] = 1.0;
}

void makeIdentity(Matrix4& m)
{
  m.a[0] = m.a[5] = m.a[10] = m.a[15] = 1;
  m.a[1] = m.a[2] = m.a[3] = 0;
  m.a[4] = m.a[6] = m.a[7] = 0;
  m.a[8] = m.a[9] = m.a[11] = 0;
  m.a[12] = m.a[13] = m.a[14] = 0;
}

} //namespace lpi

