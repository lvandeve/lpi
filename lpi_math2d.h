/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef math2d_h
#define math2d_h

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
