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
  ensureDirectoryEndOSSlash(result);
  result += child;
  return result;
}

////////////////////////////////////////////////////////////////////////////////

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
  return false;
}

} //namespace lpi
