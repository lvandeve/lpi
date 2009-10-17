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

#if 0 //set to 1 if you have boost::filesystem, 0 otherwise (but then file listing with this isn't supported)

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <sstream>

namespace fs = boost::filesystem;

namespace lpi
{


bool FileBrowseBoost::isDirectory(const std::string& filename) const
{
  fs::path p(fs::initial_path());
  p = fs::system_complete(fs::path(filename.c_str(), fs::native));
  return fs::is_directory(p);
}

void FileBrowseBoost::getFiles(std::vector<std::string>& files, const std::string& directory) const
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

void FileBrowseBoost::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
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


} //namespace lpi

#else

namespace lpi
{


bool FileBrowseBoost::isDirectory(const std::string& filename) const
{
  (void)filename;
  return false;
}

void FileBrowseBoost::getFiles(std::vector<std::string>& files, const std::string& directory) const
{
  (void)directory;
  files.push_back("boost::filesystem");
  files.push_back("not supported.");
  files.push_back("This are not files.");
  files.push_back("Sorry...");
}

void FileBrowseBoost::getDirectories(std::vector<std::string>& dirs, const std::string& directory) const
{
  (void)dirs;
  (void)directory;
}

} //namespace lpi

#endif
