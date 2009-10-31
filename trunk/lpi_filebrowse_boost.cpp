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

#include "lpi_filebrowse_boost.h"

#ifndef _WIN32 //easy if you have boost installed on linux but not on windows

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
    if(dir[dir.size() - 1] == '/')
    {
      std::string parent = getParent(dir);
      createDirectory(parent); //recursive
    }

    if(dir[dir.size() - 1] == '/' && !(dir.size() > 2 && dir[dir.size() - 2] == ':'))
    {
      fs::create_directory(dir.c_str());
    }
  }
}

} //namespace lpi

#endif
