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

#include "lpi_draw3dgl.h"
#include <GL/gl.h>

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
void setGLMatrix(const Matrix3& m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  transformGLMatrix(m3, pos);
}

/*
Same as setGLMatrix, but won't load the identity matrix, so that you can transform the set matrix
*/
void transformGLMatrix(const Matrix3& m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity(); //NIET doen want dan kan je deze functie niet 2x achter elkaar oproepen
  
  //matrix multiplication
  double m[16];
  
  m[0] = m3.a[0][0];
  m[1] = m3.a[0][1];
  m[2] = m3.a[0][2];
  m[3] = 0.0;
  
  m[4] = m3.a[1][0];
  m[5] = m3.a[1][1];
  m[6] = m3.a[1][2];
  m[7] = 0.0;
  
  m[8] = m3.a[2][0];
  m[9] = m3.a[2][1];
  m[10] = m3.a[2][2];
  m[11] = 0.0;
  
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
  
  glMultMatrixd(m);
  
  //translation
  glTranslated(-pos.x, -pos.y, -pos.z);
}

void setGLMatrixInvZ(const Matrix3& m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  transformGLMatrixInvZ(m3, pos);
}

/*
same as above but with inverted Z axis because OpenGL has it inverted
*/
void transformGLMatrixInvZ(const Matrix3& m3, const Vector3& pos)
{
  glMatrixMode(GL_MODELVIEW);
  
  //matrix multiplication
  double m[16];
  
  m[0] = m3.a[0][0];
  m[1] = m3.a[0][1];
  m[2] = -m3.a[0][2];
  m[3] = 0.0f;
  
  m[4] = m3.a[1][0];
  m[5] = m3.a[1][1];
  m[6] = -m3.a[1][2];
  m[7] = 0.0f;
  
  m[8] = m3.a[2][0];
  m[9] = m3.a[2][1];
  m[10] = -m3.a[2][2];
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

