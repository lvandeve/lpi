/*
Copyright (c) 2005-2007 Lode Vandevenne
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

#ifndef LPI_FILE_H_INCLUDED
#define LPI_FILE_H_INCLUDED

#include <vector>
#include <iostream>
#include <fstream>

namespace lpi
{


void saveFile(std::vector<unsigned char>& buffer, const std::string& filename);
void saveFile(std::string& buffer, const std::string& filename);
int loadFile(std::vector<unsigned char>& buffer, const std::string& filename);
int loadFile(std::string& buffer, const std::string& filename);
int getFilesize(const std::string& filename);

} //namespace lpi

#endif
