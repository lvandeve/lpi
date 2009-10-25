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
   giveFilenameBackslashes(szDir);
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
  giveFilenameBackslashes(szDir);
  szDir += "\\*";

  hFind = FindFirstFile(szDir.c_str(), &ffd);// Find the first file in the directory.
  if (INVALID_HANDLE_VALUE == hFind) return;

  do
  {
    //if(std::string("..") == ffd.cFileName || std::string(".") == ffd.cFileName) continue; //I don't want these in the list.
    if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) dirs.push_back(ffd.cFileName);
  }
  while(FindNextFile(hFind, &ffd) != 0);

  FindClose(hFind);
}

std::string FileBrowseWin32::getParent(const std::string& path) const
{
  std::string result = IFileBrowse::getParent(path);
  if(path.size() > 1 && path[1] == ':')
  {
    char c = path[0];
    if(result.size() < 2) result = std::string() + c + ':' + result;
  }
  return result;
}

bool FileBrowseWin32::fileExists(const std::string& filename) const
{
  DWORD fileAttr;

  fileAttr = GetFileAttributes(filename.c_str());
  if (0xFFFFFFFF == fileAttr)
      return false;
  return true;
}

} //namespace lpi

#else

namespace lpi
{


bool FileBrowseWin32::isDirectory(const std::string& filename) const
{
  (void)filename;
  return false;
}

void FileBrowseWin32::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  (void)directory;
  files.push_back("Error: win32 filesystem not supported.");
}

void FileBrowseWin32::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  (void)dirs;
  (void)directory;
}

bool FileBrowseWin32::fileExists(const std::string& filename) const
{
  (void)filename;
  return false;
}

std::string FileBrowseWin32::getParent(const std::string& path) const
{
  std::string result = IFileBrowse::getParent(path);
  if(path.size() > 1 && path[1] == ':')
  {
    char c = path[0];
    if(result.size() < 2) result = std::string() + c + ':' + result;
  }
  return result;
}

} //namespace lpi

#endif

namespace lpi
{

bool FileBrowseWin32WithDrives::isDirectory(const std::string& filename) const
{
  return browser.isDirectory(filename);
}

void FileBrowseWin32WithDrives::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  if(directory.empty())
  {
    //no files
  }
  else browser.getFiles(files, directory);
}

void FileBrowseWin32WithDrives::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  if(directory.empty())
  {
    for(char c = 'A'; c <= 'Z'; c++)
    {
      dirs.push_back(std::string() + c + ':' + '\\');
    }
  }
  else browser.getDirectories(dirs, directory);
}

std::string FileBrowseWin32WithDrives::getParent(const std::string& path) const
{
  if((path.size() == 2 && path[1] == ':')
  || (path.size() == 3 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
  || path.size() < 2)
  return "";
  else return browser.getParent(path);
}

std::string FileBrowseWin32WithDrives::getChild(const std::string& path, const std::string& child) const
{
  std::string result = path;
  if(!path.empty()) ensureDirectoryEndOSSlash(result);
  result += child;
  return result;
}

bool FileBrowseWin32WithDrives::fileExists(const std::string& filename) const
{
  return browser.fileExists(filename);
}

}
