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
#include "lpi_text.h"
#include "lpi_text_drawer.h"
#include "lpi_gui_input_sdl.h"
#include "lpi_guipartdraw_int.h"

namespace lpi
{
namespace gui
{

  class GUIDrawerBuffer : public lpi::gui::AGUIDrawer
  {
    public:
    lpi::gui::GUIInputSDL input;
    lpi::Drawer2DBuffer drawer;
    lpi::InternalTextDrawer textdrawer;
    lpi::gui::GUIPartDrawerInternal guidrawer;
    GUIDrawerBuffer() : textdrawer(lpi::TextureFactory<lpi::TextureBuffer>(), &drawer), guidrawer(lpi::TextureFactory<lpi::TextureBuffer>(), &drawer, &textdrawer){}
    virtual lpi::gui::IGUIInput& getInput() { return input; }
    virtual lpi::IDrawer2D& getDrawer() { return drawer; }
    virtual const lpi::IDrawer2D& getDrawer() const { return drawer; }
    virtual lpi::ITextDrawer& getTextDrawer() { return textdrawer; }
    virtual const lpi::ITextDrawer& getTextDrawer() const { return textdrawer; }
    virtual lpi::gui::IGUIPartDrawer& getGUIPartDrawer() { return guidrawer; }
    virtual const lpi::gui::IGUIPartDrawer& getGUIPartDrawer() const { return guidrawer; }
  };


} //namespace gui
} //namespace lpi

