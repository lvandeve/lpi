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

#include "lpi_texture_buffer.h"

namespace lpi
{

void TextureBuffer::setSize(size_t u, size_t v)
{
  buffer.resize(u * v * 4);
  this->u = u;
  this->v = v;
}

size_t TextureBuffer::getU() const
{
  return u;
}

size_t TextureBuffer::getV() const
{
  return v;
}

size_t TextureBuffer::getU2() const
{
  return u;
}

size_t TextureBuffer::getV2() const
{
  return v;
}

unsigned char* TextureBuffer::getBuffer()
{
  return buffer.empty() ? 0 : &buffer[0];
}

const unsigned char* TextureBuffer::getBuffer() const
{
  return buffer.empty() ? 0 : &buffer[0];
}

void TextureBuffer::update()
{
 //nothing to do :)
}

} //namespace lpi
