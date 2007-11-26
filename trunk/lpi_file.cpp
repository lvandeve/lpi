/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
