/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef math4d_included
#define math4d_included

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
  
  void convertTo(Vector3& v);
  void convertFrom(const Vector3& v);
};

class Matrix4
{
  public:
  
  double a[16];
  ////        ////
  // 0  4  8 12 //
  // 1  5  9 13 //
  // 2  6 10 14 //
  // 3  7 11 15 //
  ////        ////
  
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

