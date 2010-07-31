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

#include "lpi_filebrowse.h"

namespace lpi
{

std::string IFileBrowse::getParent(const std::string& path) const
{
  if(path.empty()) return path;
  
  int lastslash = 0; //last slash or backslash in the string, except if it's completely at the end
  
  for(int i = 0; i < ((int)path.size()) - 1; i++)
  {
    if(path[i] == '/' || path[i] == '\\') lastslash = i;
  }
  
  return path.substr(0, lastslash + 1);
}

std::string IFileBrowse::getChild(const std::string& path, const std::string& child) const
{
  std::string result = path;
  ensureDirectoryEndSlash(result);
  result += child;
  return result;
}

} // namespace lpi

////////////////////////////////////////////////////////////////////////////////

#if defined(LPI_OS_WINDOWS)

#include "lpi_file.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Shlobj.h>
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

void FileBrowseWin32::createDirectory(const std::string& path)
{
  std::string dir = getFileNamePathPart(path);

  if(fileExists(dir)) return;

  giveFilenameBackslashes(dir);
  if(!dir.empty())
  {
    if(dir[dir.size() - 1] == '\\')
    {
      std::string parent = getParent(dir);
      createDirectory(parent); //recursive
    }
    
    if(dir[dir.size() - 1] == '\\' && !(dir.size() > 2 && dir[dir.size() - 2] == ':'))
    {
      CreateDirectory(dir.c_str(), 0); //win32
    }
  }
}

void FileBrowseWin32::fixSlashes(std::string& path) const
{
  giveFilenameBackslashes(path);
}

void FileBrowseWin32::ensureDirectoryEndSlash(std::string& path) const
{
  lpi::ensureDirectoryEndBackslash(path);
}

std::string FileBrowseWin32::getDefaultDir(DefaultDir dd) const
{
  TCHAR szPath[MAX_PATH];

  int csidl = -1;
  
  switch(dd)
  {
    case DD_HOME: csidl = CSIDL_PERSONAL; break;
    case DD_USER_APP_SETTINGS: csidl = CSIDL_APPDATA; break;
    case DD_GLOBAL_APP_SETTINGS: csidl = CSIDL_COMMON_APPDATA; break;
    default: return "";
  }

  if(SUCCEEDED(SHGetFolderPath(NULL,
                               csidl,
                               NULL,
                               0,
                               szPath)))
  {
    std::string path = szPath;
    ensureDirectoryEndBackslash(path);
    return path;
  }
  else return "";
}

void FileBrowseWin32WithDrives::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  if(directory.empty())
  {
    //no files
  }
  else FileBrowseWin32::getFiles(files, directory);
}

void FileBrowseWin32WithDrives::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  if(directory.empty() || directory == "\\")
  {
    for(char c = 'A'; c <= 'Z'; c++)
    {
      dirs.push_back(std::string() + c + ':' + '\\');
    }
  }
  else FileBrowseWin32::getDirectories(dirs, directory);
}

std::string FileBrowseWin32WithDrives::getParent(const std::string& path) const
{
  if((path.size() == 2 && path[1] == ':')
  || (path.size() == 3 && path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
  || path.size() < 2)
  return "";
  else return FileBrowseWin32::getParent(path);
}

std::string FileBrowseWin32WithDrives::getChild(const std::string& path, const std::string& child) const
{
  std::string result = path;
  if(!path.empty()) ensureDirectoryEndSlash(result);
  result += child;
  return result;
}

} // namespace lpi

////////////////////////////////////////////////////////////////////////////////

#elif defined(LPI_OS_LINUX)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace lpi
{

bool FileBrowseLinux::isDirectory(const std::string& filename) const
{
  struct stat _stat;
  if(lstat(filename.c_str(), &_stat) == 0 )
  {
    return S_ISDIR(_stat.st_mode);
  }
  return false;
}

static bool isRegularFile(const std::string& filename)
{
  struct stat _stat;
  if(lstat(filename.c_str(), &_stat) == 0 )
  {
    return S_ISREG(_stat.st_mode);
  }
  return false;
}

void FileBrowseLinux::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  DIR* d = opendir( directory.c_str() );
  static struct dirent* dirp;
  if (!d)
  {
    files.push_back("Error. No permission?");
    return;
  }
  
  while ( (dirp = readdir(d)) != NULL )
  {
    if( isRegularFile(directory + dirp->d_name))
    {
      files.push_back(dirp->d_name);
    }
  }
  closedir(d);
}

void FileBrowseLinux::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  DIR* d = opendir( directory.c_str() );
  static struct dirent* dirp;
  if (!d)
  {
    dirs.push_back("..");
    return;
  }
  
  while ( (dirp = readdir(d)) != NULL )
  {
    if( isDirectory(directory + dirp->d_name))
    {
      dirs.push_back(dirp->d_name);
    }
  }
  closedir(d);
}

bool FileBrowseLinux::fileExists(const std::string& filename) const
{
  struct stat _stat;
  return(stat(filename.c_str(),&_stat) == 0);
}

std::string FileBrowseLinux::getParent(const std::string& path) const
{
  return IFileBrowse::getParent(path);
}

void FileBrowseLinux::createDirectory(const std::string& path)
{
  std::string dir = getFileNamePathPart(path);

  if(fileExists(dir)) return;

  giveFilenameSlashes(dir);
  if(!dir.empty())
  {
    if(dir[dir.size() - 1] == '/')
    {
      std::string parent = getParent(dir);
      createDirectory(parent); //recursive
    }

    if(dir[dir.size() - 1] == '/' && !(dir.size() > 2 && dir[dir.size() - 2] == ':'))
    {
      mkdir(dir.c_str(), 0777);
    }
  }
}

void FileBrowseLinux::fixSlashes(std::string& path) const
{
  giveFilenameSlashes(path);
}

void FileBrowseLinux::ensureDirectoryEndSlash(std::string& path) const
{
  lpi::ensureDirectoryEndSlash(path);
}

std::string FileBrowseLinux::getDefaultDir(DefaultDir dd) const
{
  switch(dd)
  {
    case DD_HOME:
    {
      char* c = std::getenv("HOME");
      if(c == 0) return "";
      std::string path = c;
      ensureDirectoryEndSlash(path);
      return path;
    }
    case DD_USER_APP_SETTINGS:
    {
      std::string path;
      char* c = std::getenv("XDG_CONFIG_HOME");
      if(c != 0) path = c;
      if(!path.empty()) ensureDirectoryEndSlash(path);
      if(path.empty())
      {
        char* c = std::getenv("HOME");
        if(c != 0) path = c;
        if(!path.empty())
        {
          ensureDirectoryEndSlash(path);
          path += ".config/";
        }
      }
      return path;
    }
    case DD_GLOBAL_APP_SETTINGS:
    {
      return "/etc/";
    }
    default: return "";
  }
}

} // namespace lpi

////////////////////////////////////////////////////////////////////////////////
#elif defined(LPI_OS_AMIGA) || defined(LPI_OS_AROS)

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace lpi
{

bool FileBrowseAmiga::isDirectory(const std::string& filename) const
{
  struct stat _stat;
  if(lstat(filename.c_str(), &_stat) == 0 )
  {
    return S_ISDIR(_stat.st_mode);
  }
  return false;
}

static bool isRegularFile(const std::string& filename)
{
  struct stat _stat;
  if(lstat(filename.c_str(), &_stat) == 0 )
  {
    return S_ISREG(_stat.st_mode);
  }
  return false;
}

void FileBrowseAmiga::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  DIR* d = opendir( directory.c_str() );
  static struct dirent* dirp;
  if (!d)
  {
    files.push_back("Error. No permission?");
    return;
  }
  
  while ( (dirp = readdir(d)) != NULL )
  {
    if( isRegularFile(directory + dirp->d_name))
    {
      files.push_back(dirp->d_name);
    }
  }
  closedir(d);
}

void FileBrowseAmiga::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  DIR* d = opendir( directory.c_str() );
  static struct dirent* dirp;
  if (!d)
  {
    dirs.push_back("/");
    return;
  }
  
  while ( (dirp = readdir(d)) != NULL )
  {
    if( isDirectory(directory + dirp->d_name))
    {
      dirs.push_back(dirp->d_name);
    }
  }
  closedir(d);
}

bool FileBrowseAmiga::fileExists(const std::string& filename) const
{
  struct stat _stat;
  return(stat(filename.c_str(),&_stat) == 0);
}

std::string FileBrowseAmiga::getParent(const std::string& path) const
{
  return IFileBrowse::getParent(path);
}

void FileBrowseAmiga::createDirectory(const std::string& path)
{
  std::string dir = getFileNamePathPart(path);

  if(fileExists(dir)) return;

  giveFilenameSlashes(dir);
  if(!dir.empty())
  {
    if(dir[dir.size() - 1] == '/')
    {
      std::string parent = getParent(dir);
      createDirectory(parent); //recursive
    }

    if(dir[dir.size() - 1] == '/' && !(dir.size() > 2 && dir[dir.size() - 2] == ':'))
    {
      mkdir(dir.c_str(), 0777);
    }
  }
}

void FileBrowseAmiga::fixSlashes(std::string& path) const
{
  giveFilenameSlashes(path);
}

void FileBrowseAmiga::ensureDirectoryEndSlash(std::string& path) const
{
  lpi::ensureDirectoryEndSlash(path);
}

std::string FileBrowseAmiga::getDefaultDir(DefaultDir dd) const
{
  switch(dd)
  {
    case DD_HOME:
    {
      char* c = "PROGDIR:";
      if(c == 0) return "";
      std::string path = c;
      ensureDirectoryEndSlash(path);
      return path;
    }
    case DD_USER_APP_SETTINGS:
    {
      std::string path;
      char* c = std::getenv("XDG_CONFIG_HOME");
      if(c != 0) path = c;
      if(!path.empty()) ensureDirectoryEndSlash(path);
      if(path.empty())
      {
        char* c = "PROGDIR:";
        if(c != 0) path = c;
        if(!path.empty())
        {
          ensureDirectoryEndSlash(path);
          path += ".config/";
        }
      }
      return path;
    }
    case DD_GLOBAL_APP_SETTINGS:
    {
      return "PROGDIR:";
    }
    default: return "";
  }
}

} // namespace lpi

////////////////////////////////////////////////////////////////////////////////

#elif defined(HAVEGOTBOOSTFILESYSTEM)

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <sstream>

namespace fs = boost::filesystem;

namespace lpi
{

bool FileBrowseBoost::isDirectory(const std::string& filename) const
{
  try
  {
    fs::path p(fs::initial_path());
    p = fs::system_complete(fs::path(filename.c_str(), fs::native));
    return fs::is_directory(p);
  }
  catch(...)
  {
    return false;
  }
}

void FileBrowseBoost::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  try
  {
    fs::path full_path(fs::initial_path());

    full_path = fs::system_complete(fs::path(directory.c_str(), fs::native));

    if(!fs::exists(full_path)) return;
    if(!fs::is_directory(full_path)) return;
    
    fs::directory_iterator end_iter;
    for(fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
    {
      if(!fs::is_directory(*dir_itr))
      {
        std::ostringstream ss;
        ss << dir_itr->leaf();
        files.push_back(ss.str());
      }
    }
  }
  catch(...)
  {
    files.push_back("Error. No permission?");
  }
}

void FileBrowseBoost::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  try
  {
    fs::path full_path(fs::initial_path());

    full_path = fs::system_complete(fs::path(directory.c_str(), fs::native));

    if(!fs::exists(full_path)) return;
    if(!fs::is_directory(full_path)) return;
    
    fs::directory_iterator end_iter;
    for(fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
    {
      if(fs::is_directory(*dir_itr))
      {
        std::ostringstream ss;
        ss << dir_itr->leaf();
        dirs.push_back(ss.str());
      }
    }
  }
  catch(...)
  {
    dirs.push_back("..");
  }
}

bool FileBrowseBoost::fileExists(const std::string& filename) const
{
  try
  {
    fs::path full_path(fs::initial_path());
    full_path = fs::system_complete(fs::path(filename.c_str(), fs::native));

    return fs::exists(full_path);
  }
  catch(...)
  {
    return true; //I return true instead of false because if file exists you get warning "do you want to overwrite?" which is safer than returning false.
  }
}

std::string FileBrowseBoost::getParent(const std::string& path) const
{
  return IFileBrowse::getParent(path);
}

void FileBrowseBoost::createDirectory(const std::string& path)
{
  std::string dir = getFileNamePathPart(path);

  if(fileExists(dir)) return;

  giveFilenameSlashes(dir);
  if(!dir.empty())
  {
    if(dir[dir.size() - 1] == '/' || (dir[dir.size() - 1] == '\\'))
    {
      std::string parent = getParent(dir);
      createDirectory(parent); //recursive
    }

    if((dir[dir.size() - 1] == '/' || dir[dir.size() - 1] == '\\') && !(dir.size() > 2 && dir[dir.size() - 2] == ':'))
    {
      fs::create_directory(dir.c_str());
    }
  }
}

void FileBrowseBoost::fixSlashes(std::string& path) const
{
  giveFilenameSlashes(path);
}

void FileBrowseBoost::ensureDirectoryEndSlash(std::string& path) const
{
  lpi::ensureDirectoryEndSlash(path);
}

std::string FileBrowseBoost::getDefaultDir(DefaultDir dd) const
{
  (void)dd;
  
  return ""; //not supported yet for the FileBrowseBoost implementation! TODO
}

} // namespace lpi

////////////////////////////////////////////////////////////////////////////////

#else

bool FileBrowseNotSupported::isDirectory(const std::string& filename) const
{
  (void)filename;
  return false;
}

void FileBrowseNotSupported::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  (void)directory;
  files.push_back("Error: file browsing not supported");
}

void FileBrowseNotSupported::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  (void)directory;
  (void)dirs;
}

bool FileBrowseNotSupported::fileExists(const std::string& filename) const
{
  (void)filename;
  return false;
}

void FileBrowseNotSupported::createDirectory(const std::string& path)
{
  (void)path;
}

void FileBrowseNotSupported::fixSlashes(std::string& path) const
{
  (void)path;
}

void FileBrowseNotSupported::ensureDirectoryEndSlash(std::string& path) const
{
  (void)path;
}

std::string FileBrowseBoost::getDefaultDir(DefaultDir dd) const
{
  (void)dd;
  return "";
}


} //namespace lpi

#endif

////////////////////////////////////////////////////////////////////////////////


