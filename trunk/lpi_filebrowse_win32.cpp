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

#include "lpi_filebrowse_win32.h"

#ifdef WIN32

#include "lpi_file.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>


#include <sstream>

namespace lpi
{


bool FileBrowseWin32::isDirectory(const std::string& filename) const
{
  return (GetFileAttributes(filename.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
}

void FileBrowseWin32::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;

   if(directory.size() > (MAX_PATH - 3)) return;
   
   std::string szDir = directory;
   giveFilenameBackwardSlashes(szDir);
   szDir += "\\*";
   
   hFind = FindFirstFile(szDir.c_str(), &ffd);// Find the first file in the directory.
   if (INVALID_HANDLE_VALUE == hFind) return;

   do
   {
    if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) files.push_back(ffd.cFileName);
   }
   while(FindNextFile(hFind, &ffd) != 0);

   FindClose(hFind);
}

void FileBrowseWin32::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;

   if(directory.size() > (MAX_PATH - 3)) return;

   std::string szDir = directory;
   giveFilenameBackwardSlashes(szDir);
   szDir += "\\*";

   hFind = FindFirstFile(szDir.c_str(), &ffd);// Find the first file in the directory.
   if (INVALID_HANDLE_VALUE == hFind) return;

   do
   {
    if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) dirs.push_back(ffd.cFileName);
   }
   while(FindNextFile(hFind, &ffd) != 0);

   FindClose(hFind);
}


} //namespace lpi

#else

namespace lpi
{


bool FileBrowseBoost::isDirectory(const std::string& filename)
{
  (void)filename;
  return false;
}

void FileBrowseBoost::getFiles(std::vector<std::string>& files, const std::string& directory)
{
  (void)directory;
  files.push_back("win32 filesystem");
  files.push_back("not supported.");
  files.push_back("This are not files.");
  files.push_back("Sorry...");
}

void FileBrowseBoost::getDirectories(std::vector<std::string>& dirs, const std::string& directory)
{
  (void)dirs;
  (void)directory;
}

} //namespace lpi

#endif
