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

#include <iostream>

#include "lpi_gui_drawer_buffer.h"
#include "lpi_texture.h"

namespace lpi
{
namespace gui
{

GUIDrawerBuffer::GUIDrawerBuffer() : textdrawer(TextureFactory<TextureBuffer>(), &drawer), guidrawer(TextureFactory<TextureBuffer>(), &drawer, &textdrawer){}
IInput& GUIDrawerBuffer::getInput() { return input; }
IDrawer2D& GUIDrawerBuffer::getDrawer() { return drawer; }
const IDrawer2D& GUIDrawerBuffer::getDrawer() const { return drawer; }
ITextDrawer& GUIDrawerBuffer::getTextDrawer() { return textdrawer; }
const ITextDrawer& GUIDrawerBuffer::getTextDrawer() const { return textdrawer; }
IGUIPartDrawer& GUIDrawerBuffer::getGUIPartDrawer() { return guidrawer; }
const IGUIPartDrawer& GUIDrawerBuffer::getGUIPartDrawer() const { return guidrawer; }

} //namespace gui
} //namespace lpi


