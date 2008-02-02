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

#ifndef LPI_BASE64_H_INCLUDED
#define LPI_BASE64_H_INCLUDED

#include <string>
#include <vector>

namespace lpi
{
void encodeBase64(std::string& out, const std::vector<unsigned char>& in);
void decodeBase64(std::vector<unsigned char>& out, const std::string& in);
} //namespace lpi

#endif
