 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_draw3dgl.h"

namespace { static const double pi = 3.141592653589793238; }

namespace lpi
{

void drawCircle(const Vector3& pos, const Vector3& n, double radius, const ColorRGB& color)
{
  
  glDisable(GL_TEXTURE_2D);
  const int p = 256; //the precision
  
  Vector3 xdir = normalize(getSomePerpendicularVector(n));
  Vector3 ydir = normalize(cross(xdir, n));
  
  glColor4ub(color.r, color.g, color.b, color.a);
  
  glBegin(GL_LINE_STRIP);
  
  for(int i = 0; i <= p; i++)
  {
    double angle = (2 * pi * i) / p;
    Vector3 v = pos + (cos(angle) * xdir + sin(angle) * ydir) * radius;
    
    glVertex3f(v.x, v.y, v.z);
  }
  glEnd();
}

////////////////////////////////////////////////////////////////////////////////

/*
this function sets the opengl modelview matrix to the transformation described by the Matrix3 and Vector3 used by my own 3D math functions
when using camera of type Transformation3: use cam.getInvMatrix() as m3, cam.getPos() as pos for camera
*/
void setGLMatrix(Matrix3 m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  transformGLMatrix(m3, pos);
}

/*
Same as setGLMatrix, but won't load the identity matrix, so that you can transform the set matrix
*/
void transformGLMatrix(Matrix3 m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity(); //NIET doen want dan kan je deze functie niet 2x achter elkaar oproepen
  
  //matrix multiplication
  double m[16];
  
  m[0] = m3.a[0];
  m[1] = m3.a[1];
  m[2] = m3.a[2];
  m[3] = 0.0;
  
  m[4] = m3.a[3];
  m[5] = m3.a[4];
  m[6] = m3.a[5];
  m[7] = 0.0;
  
  m[8] = m3.a[6];
  m[9] = m3.a[7];
  m[10] = m3.a[8];
  m[11] = 0.0;
  
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
  
  glMultMatrixd(m);
  
  //translation
  glTranslated(-pos.x, -pos.y, -pos.z);
}

void setGLMatrixInvZ(Matrix3 m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  transformGLMatrixInvZ(m3, pos);
}

/*
same as above but with inverted Z axis because OpenGL has it inverted
*/
void transformGLMatrixInvZ(Matrix3 m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  
  //matrix multiplication
  double m[16];
  
  m[0] = m3.a[0];
  m[1] = m3.a[1];
  m[2] = -m3.a[2];
  m[3] = 0.0f;
  
  m[4] = m3.a[3];
  m[5] = m3.a[4];
  m[6] = -m3.a[5];
  m[7] = 0.0f;
  
  m[8] = m3.a[6];
  m[9] = m3.a[7];
  m[10] = -m3.a[8];
  m[11] = 0.0f;
  
  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
  
  glMultMatrixd(m);
  
  //translation
  glTranslated(-pos.x, -pos.y, -pos.z);
}

} //end of namespace lpi

