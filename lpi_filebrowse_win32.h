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

/*
This file is for simple file support, it can't browse directories or see
which files exist, only read or write a file with a given path name.
*/

#include <vector>
#include <iostream>

#include "lpi_filebrowse.h"

namespace lpi
{

class FileBrowseWin32 : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) const;
  
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;

  virtual bool fileExists(const std::string& filename) const;
  
  virtual std::string getParent(const std::string& path) const;
};

class FileBrowseWin32WithDrives : public IFileBrowse
{
  FileBrowseWin32 browser;
  
  public:
  virtual bool isDirectory(const std::string& filename) const;

  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;

  virtual bool fileExists(const std::string& filename) const;

  virtual std::string getParent(const std::string& path) const;
  virtual std::string getChild(const std::string& path, const std::string& child) const;
};

} //namespace lpi


