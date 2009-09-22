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

#include "lpi_gui_drawer.h"
#include "lpi_gui.h"
#include "lpi_tools.h"
#include "lodepng.h"
//#include "lpi_screen_gl.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_parse.h"
#include "lpi_event.h"


#include <SDL/SDL.h>

namespace lpi
{

void binaryFileToBase64File(const std::string& outfilename, const std::string& infilename, bool cppnewlines)
{
  std::string base64;
  std::vector<unsigned char> file;
  
  loadFile(file, infilename);
  encodeBase64(base64, file);
  if(cppnewlines) searchAndReplace(base64, "\n", "\\n\\\n");
  saveFile(base64, outfilename);
}

void base64FileToBinaryFile(const std::string& outfilename, const std::string& infilename)
{
  std::string base64;
  std::vector<unsigned char> file;
  
  loadFile(base64, infilename);
  decodeBase64(file, base64);
  saveFile(file, outfilename);
}

void base64StringToBinaryFile(const std::string& outfilename, const std::string& in)
{
  std::vector<unsigned char> file;
  decodeBase64(file, in);
  saveFile(file, outfilename);
}

void graphicalKeyBoardNumberTest(gui::IGUIDrawer& drawer)
{
  for(size_t i = 0; i < 1024; i++) if(lpi::keyDown(i))
  {
    int x = (i % 32) * 32;
    int y = (i / 32) * 8;
    drawer.print(i, x, y);
  }
}

} //namespace lpi

