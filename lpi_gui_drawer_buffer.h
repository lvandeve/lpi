/*
Copyright (c) 2005-2009 Lode Vandevenne
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

#include "lpi_gui_drawer.h"
#include "lpi_draw2d_buffer.h"
#include "lpi_color.h"
#include "lpi_texture.h"
#include "lpi_text_drawer_int.h"
#include "lpi_input_sdl.h"
#include "lpi_guipartdraw_int.h"

namespace lpi
{
namespace gui
{

  //TODO: implement these functions in the .cpp file instead of here
  class GUIDrawerBuffer : public AGUIDrawer
  {
    private:
      InputSDL input;
      Drawer2DBuffer drawer;
      InternalTextDrawer textdrawer;
      GUIPartDrawerInternal guidrawer;
    public:
      GUIDrawerBuffer();
      virtual IInput& getInput();
      virtual IDrawer2D& getDrawer();
      virtual const IDrawer2D& getDrawer() const;
      virtual ITextDrawer& getTextDrawer();
      virtual const ITextDrawer& getTextDrawer()const ;
      virtual IGUIPartDrawer& getGUIPartDrawer();
      virtual const IGUIPartDrawer& getGUIPartDrawer() const;
  };


} //namespace gui
} //namespace lpi

