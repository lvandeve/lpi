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

#include "lpi_file.h"

namespace lpi
{

using namespace std;

//returns the size of the file
int getFilesize(const std::string& filename)
{
  ifstream file(filename.c_str(), ios::in|ios::binary|ios::ate);
  
  //get filesize
  file.seekg(0, ios::end);
  int size = file.tellg();
  file.seekg(0, ios::beg);
  size -= file.tellg();
  
  file.close();
  
  return size;  
}

//write given buffer to the file, overwriting the file, it doesn't append to it.
void saveFile(std::vector<unsigned char>& buffer, const std::string& filename)
{
  ofstream file(filename.c_str(), ios::out|ios::binary);
  file.write((char*)&buffer[0], buffer.size());
  file.close();
}

//write given buffer to the file, overwriting the file, it doesn't append to it.
void saveFile(std::string& buffer, const std::string& filename)
{
  ofstream file(filename.c_str(), ios::out|ios::binary);
  file.write((char*)&buffer[0], buffer.size());
  file.close();
}

/*
Read given file and store into given buffer. Returns filesize.
*/
int loadFile(std::vector<unsigned char>& buffer, const std::string& filename)
{
  buffer.clear();
  int size = getFilesize(filename.c_str());
  buffer.resize(size);
  
  ifstream file(filename.c_str(), ios::in|ios::binary|ios::ate);
  file.seekg(0, ios::beg);
  file.read((char*)&buffer[0], size);
  file.close();
  return size;
}

/*
read file into string
*/
int loadFile(std::string& buffer, const std::string& filename)
{
  buffer.clear();
  int size = getFilesize(filename.c_str());
  buffer.resize(size);
  
  ifstream file(filename.c_str(), ios::in|ios::binary|ios::ate);
  file.seekg(0, ios::beg);
  file.read((char*)&buffer[0], size);
  file.close();
  return size;
}

} //namespace lpi
