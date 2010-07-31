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

#include "lpi_gl_context.h"

#include <iostream>

namespace lpi
{

GLContext::GLContext()
: active(false)
, index(-1)
{
}

GLContext::~GLContext()
{
}

bool GLContext::isActive() const
{
  return active;
}

int GLContext::getID() const
{
  return index;
}

void GLContext::onNewGLContext()
{
  active = true;
  index++;
}

void GLContext::onGLContextDestroyed()
{
  active = false;
}

} //end of namespace lpi
