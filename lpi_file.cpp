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
#include "lpi_os.h"

#include <algorithm>

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
void saveFile(const std::vector<unsigned char>& buffer, const std::string& filename)
{
  ofstream file(filename.c_str(), ios::out|ios::binary);
  file.write((char*)&buffer[0], buffer.size());
  file.close();
}

//write given buffer to the file, overwriting the file, it doesn't append to it.
void saveFile(const std::string& buffer, const std::string& filename)
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

bool fileExists(const std::string& filename)
{
  return getFilesize(filename) > 0; //TODO: use a better implementation, now it returns false for a file that exists but has size 0...
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//filename utilities

std::string getFileNamePathPart(const std::string& filename, bool include_end_slash)
{
  if(filename.empty()) return "";
  
  //find last backwards or forwards slash
  int last = -1;
  for(size_t i = 0; i < filename.size(); i++)
  {
#if !defined(LPI_OS_AMIGA) && !defined(LPI_OS_AROS) //todo: remove all platform dependent code from lpi_file.cpp and move it to lpi_filebrowse.cpp
    if(filename[i] == '/' || filename[i] == '\\') last = i;
#else
	if(filename[i] == '/' || filename[i] == ':') last = i;
#endif
  }
  
  return last < 0 ? std::string("") : filename.substr(0, include_end_slash ? last + 1 : last);
}

std::string getFileNameFilePart(const std::string& filename)
{
  //find last backwards or forwards slash
  int lastslash = -1;
  for(size_t i = 0; i < filename.size(); i++)
  {
#if !defined(LPI_OS_AMIGA) && !defined(LPI_OS_AROS) //todo: remove all platform dependent code from lpi_file.cpp and move it to lpi_filebrowse.cpp
    if(filename[i] == '/' || filename[i] == '\\') lastslash = i;
#else
    if(filename[i] == '/' || filename[i] == ':') lastslash = i;
#endif
  }
  
  //find the last dot
  int lastdot = -1;
  for(size_t i = 0; i < filename.size(); i++)
  {
    if(filename[i] == '.') lastdot = i;
  }
  
  /*
  ALL of these examples should return "main"
  *) main.cpp
  *) /mnt/D/main.cpp
  *) /mnt/D/main
  *) /mnt/dir.d/main
  *) D:\main.cpp
  
  The following examples should NOT return "main", but an empty string
  *) .main
  *) /mnt/D/main/
  *) /mnt/D/.main
  
  */
  
  std::string result = filename;
  if(lastdot >= 0 && !(lastslash >= 0 && lastdot < lastslash)) result = result.substr(0, lastdot);
  if(lastslash == (int)result.size() - 1) result = "";
  else if(lastslash >= 0) result = result.substr(lastslash + 1, result.size() - lastslash);
  
  return result;
}

std::string getFileNameExtPart(const std::string& filename, bool include_dot)
{
  std::string result = getFileNameFileExtPart(filename);
  
  //find the last dot
  int last = -1;
  for(size_t i = 0; i < filename.size(); i++)
  {
    if(filename[i] == '.') last = i;
  }
  
  if(last < 0) return "";
  else if(include_dot) return filename.substr(last, filename.size() - last);
  else return filename.substr(last + 1, filename.size() - last - 1);
}

std::string getFileNameFileExtPart(const std::string& filename)
{
  //return getFileNameFilePart(filename) + getFileNameExtPart(filename, true);
  
  //find last backwards or forwards slash
  int last = -1;
  for(size_t i = 0; i < filename.size(); i++)
  {
#if !defined(LPI_OS_AMIGA) && !defined(LPI_OS_AROS) //todo: remove all platform dependent code from lpi_file.cpp and move it to lpi_filebrowse.cpp
    if(filename[i] == '/' || filename[i] == '\\') last = i;
#else
    if(filename[i] == '/' || filename[i] == ':') last = i;
#endif
  }
  
  return last < 0 ? filename : filename.substr(last + 1, filename.size() - last - 1);
}

void giveFilenameSlashes(std::string& filename)
{
  for(size_t i = 0; i < filename.size(); i++)
  {
    if(filename[i] == '\\') filename[i] = '/';
  }
}

void giveFilenameBackslashes(std::string& filename)
{
  for(size_t i = 0; i < filename.size(); i++)
  {
    if(filename[i] == '/') filename[i] = '\\';
  }
}

void ensureDirectoryEndSlash(std::string& filename)
{
#if !defined(LPI_OS_AMIGA) && !defined(LPI_OS_AROS) //todo: remove all platform dependent code from lpi_file.cpp and move it to lpi_filebrowse.cpp
  if(!filename.empty() && filename[filename.size() - 1] != '/') filename += '/';
#else
  if(!filename.empty() && filename[filename.size() - 1] != '/' && filename[filename.size() - 1] != ':') filename += '/';
#endif
}

void ensureDirectoryEndBackslash(std::string& filename)
{
  if(!filename.empty() && filename[filename.size() - 1] != '\\') filename += '\\';
}

bool extEqualsIgnoreCase(const std::string& filename, const std::string& ext)
{
  std::string fext = getFileNameExtPart(filename, false);
  std::transform(fext.begin(), fext.end(), fext.begin(), ::tolower);
  std::string lext = ext;
  std::transform(lext.begin(), lext.end(), lext.begin(), ::tolower);
  return lext == fext;
}


} //namespace lpi




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




#if 0

#include "lpi_unittest.h"

#define LUT_MY_RESET \
{\
}

int main()
{

  LUT_START_UNIT_TEST
  
  LUT_CASE("filename parts linux")
    std::string filename = "/mnt/D/main.cpp";
    
    std::cout<<lpi::getFileNamePathPart(filename)<<" "<<lpi::getFileNamePathPart(filename, false) <<std::endl;
    
    LUT_SUB_ASSERT_TRUE((lpi::getFileNamePathPart(filename) == "/mnt/D/"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNamePathPart(filename, false) == "/mnt/D"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameFilePart(filename) == "main"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameExtPart(filename) == ".cpp"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameExtPart(filename, false) == "cpp"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameFileExtPart(filename) == "main.cpp"))

  LUT_CASE_END
  
  LUT_CASE("filename parts windows")
    std::string filename = "D:\\folder\\main.cpp";
    
    std::cout<<lpi::getFileNamePathPart(filename)<<" "<<lpi::getFileNamePathPart(filename, false) <<std::endl;
    
    LUT_SUB_ASSERT_TRUE((lpi::getFileNamePathPart(filename) == "D:\\folder\\"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNamePathPart(filename, false) == "D:\\folder"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameFilePart(filename) == "main"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameExtPart(filename) == ".cpp"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameExtPart(filename, false) == "cpp"))
    LUT_SUB_ASSERT_TRUE((lpi::getFileNameFileExtPart(filename) == "main.cpp"))

  LUT_CASE_END
  
  
  LUT_END_UNIT_TEST

}


#endif
