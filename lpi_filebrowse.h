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
#include "lpi_os.h"

namespace lpi
{

class IFileBrowse
{
  public:
    virtual bool isDirectory(const std::string& filename) const = 0;
    
    //gets all the files in the given directory (only files, not directories inside the directory)
    //directory must have endslash
    //the returned files are not the full path, but directory + the file gives the full path.
    virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const = 0;
    //gets all the subdirectories in the given directory
    //directory must have endslash
    //the returned dirs are not the full path, but directory + the dir gives the full path.
    virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const = 0;
    
    virtual bool fileExists(const std::string& filename) const = 0;
    
    virtual std::string getParent(const std::string& path) const; //e.g. /mnt/D/folder/ gives /mnt/D/. Has default implementation for convenience.
    virtual std::string getChild(const std::string& path, const std::string& child) const; //child can be subdir or file
    
    virtual void createDirectory(const std::string& path) = 0; //the path is allowed to include a filename. It creates all directories and subdirectories needed to have the complete path (excluding the filename and extention part) created on disk. In fact, it does everything needed to make sure the file can be saved with a standard C++ ofstream afterwars.
    
    virtual void ensureDirectoryEndSlash(std::string& path) const = 0; //If you give a path without ending slash, it's added.
    virtual void fixSlashes(std::string& path) const = 0; //makes sure the slashes of the path are the correct type for this platform.
    
    enum DefaultDir
    {
      DD_HOME, //your home or "my documents" directory
      DD_USER_APP_SETTINGS, //folder where an application can store settings per user
      DD_GLOBAL_APP_SETTINGS //folder where an application can store settings globally for all users
    };
    
    virtual std::string getDefaultDir(DefaultDir dd) const = 0; //returns the path of one of the default dirs, or empty string if none available
};

#if defined(LPI_OS_WINDOWS)

class FileBrowseWin32 : public IFileBrowse
{
  public:
    virtual bool isDirectory(const std::string& filename) const;
    
    virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
    virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;

    virtual bool fileExists(const std::string& filename) const;
    
    virtual std::string getParent(const std::string& path) const;
    
    virtual void createDirectory(const std::string& path);
    
    virtual void ensureDirectoryEndSlash(std::string& path) const;
    virtual void fixSlashes(std::string& path) const;
    
    virtual std::string getDefaultDir(DefaultDir dd) const;
};

class FileBrowseWin32WithDrives : public FileBrowseWin32
{
  
  public:

  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;

  virtual std::string getParent(const std::string& path) const;
  virtual std::string getChild(const std::string& path, const std::string& child) const;
};

typedef FileBrowseWin32WithDrives FileBrowse;

#elif defined(LPI_OS_LINUX)

class FileBrowseLinux : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) const;
  
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;
  
  virtual bool fileExists(const std::string& filename) const;
  
  virtual std::string getParent(const std::string& path) const;
  
  virtual void createDirectory(const std::string& path);

  virtual void ensureDirectoryEndSlash(std::string& path) const;
  virtual void fixSlashes(std::string& path) const;
  
  virtual std::string getDefaultDir(DefaultDir dd) const;
};

typedef FileBrowseLinux FileBrowse;

#elif defined(LPI_OS_AMIGA) || defined(LPI_OS_AROS)

class FileBrowseAmiga : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) const;
  
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;
  
  virtual bool fileExists(const std::string& filename) const;
  
  virtual std::string getParent(const std::string& path) const;
  
  virtual void createDirectory(const std::string& path);

  virtual void ensureDirectoryEndSlash(std::string& path) const;
  virtual void fixSlashes(std::string& path) const;
  
  virtual std::string getDefaultDir(DefaultDir dd) const;
};

typedef FileBrowseAmiga FileBrowse;

#elif defined(HAVEGOTBOOSTFILESYSTEM)

class FileBrowseBoost : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) const;
  
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;
  
  virtual bool fileExists(const std::string& filename) const;
  
  virtual std::string getParent(const std::string& path) const;
  
  virtual void createDirectory(const std::string& path);

  virtual void ensureDirectoryEndSlash(std::string& path) const;
  virtual void fixSlashes(std::string& path) const;
};

typedef FileBrowseBoost FileBrowse;

#else

class FileBrowseNotSupported : public IFileBrowse
{
  public:
  virtual bool isDirectory(const std::string& filename) const;
  virtual void getFiles(std::vector<std::string>& files, const std::string& directory) const;
  virtual void getDirectories(std::vector<std::string>& dirs, const std::string& directory) const;
  virtual bool fileExists(const std::string& filename) const;
  virtual void createDirectory(const std::string& path);

  virtual void ensureDirectoryEndSlash(std::string& path) const;
  virtual void fixSlashes(std::string& path) const;
  
  virtual std::string getDefaultDir(DefaultDir dd) const;
};

typedef FileBrowseNotSupported FileBrowse;

#endif

} //namespace lpi


