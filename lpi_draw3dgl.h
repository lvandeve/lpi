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

//2D drawing with opengl

#ifndef LPI_DRAW3DGL_H_INCLUDED
#define LPI_DRAW3DGL_H_INCLUDED


#include <vector>
#include <cmath>

#include "lpi_color.h"
#include "lpi_math3d.h"

namespace lpi
{

void drawCircle(const Vector3& pos, const Vector3& n, double radius, const ColorRGB& color);

////////////////////////////////////////////////////////////////////////////////

void setGLMatrix(const Matrix3& m3, const Vector3& pos);
void transformGLMatrix(const Matrix3& m3, const Vector3& pos);
void setGLMatrixInvZ(const Matrix3& m3, const Vector3& pos);
void transformGLMatrixInvZ(const Matrix3& m3, const Vector3& pos);

} //end of namespace lpi

#endif
