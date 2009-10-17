/*
Copyright (c) 2005-2009 Lode Vandevenne
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

#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "lpi_file.h"

namespace lpi
{

class IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) = 0;
  
  //gets all the files in the given directory (only files, not directories inside the directory)
  //directory must have endslash
  //the returned files are not the full path, but directory + the file gives the full path.
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) = 0;
  //gets all the subdirectories in the given directory
  //directory must have endslash
  //the returned dirs are not the full path, but directory + the dir gives the full path.
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) = 0;
};

class FileBrowseNotSupported : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename);
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory);
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory);
};

} //namespace lpi

