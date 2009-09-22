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

#pragma once

#include "lpi_color.h"
#include "lpi_texture.h"


namespace lpi
{

/*
Texture implemented with a simple buffer.
Maybe the name is confusing, but this is NOT a buffer of textures!
*/
class TextureBuffer : public ITexture
{
  private:
    std::vector<unsigned char> buffer;
    size_t u;
    size_t v;
    
  public:
  
  virtual void setSize(size_t u, size_t v);
  virtual size_t getU() const;
  virtual size_t getV() const;
  
  virtual size_t getU2() const;
  virtual size_t getV2() const;
  
  virtual unsigned char* getBuffer();
  virtual const unsigned char* getBuffer() const;
  
  virtual void update();
};



} //namespace lpi

