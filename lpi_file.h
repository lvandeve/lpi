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

It also has some utilities for easier working with path and filenames.
*/

#include <vector>
#include <iostream>
#include <fstream>

namespace lpi
{
//file saving and loading

void saveFile(const std::vector<unsigned char>& buffer, const std::string& filename); //this does NOT create directories if they don't exist yet
void saveFile(const std::string& buffer, const std::string& filename);
int loadFile(std::vector<unsigned char>& buffer, const std::string& filename);
int loadFile(std::string& buffer, const std::string& filename);
int getFilesize(const std::string& filename);
bool fileExists(const std::string& filename);

//filename utilities

std::string getFileNamePathPart(const std::string& filename, bool include_end_slash = true); //e.g. "/mnt/D/main.cpp" gives "/mnt/D/" 
std::string getFileNameFilePart(const std::string& filename); //e.g. "/mnt/D/main.cpp" gives "main" 
std::string getFileNameExtPart(const std::string& filename, bool include_dot = true); //e.g. "/mnt/D/main.cpp" gives ".cpp" 
std::string getFileNameFileExtPart(const std::string& filename); //e.g. "/mnt/D/main.cpp" gives "main.cpp" 
void giveFilenameSlashes(std::string& filename); //gives forward slashes to the filename (for linux and mac)
void giveFilenameBackslashes(std::string& filename); //gives backward slashes to the filename (for windows)
void ensureDirectoryEndSlash(std::string& filename); //ensures that one ending slash is at the end (to indicate the path is a directory) NOTE: if the path is empty, no slash is added, or it'd mean "root directory"!
void ensureDirectoryEndBackslash(std::string& filename); //ensures that one ending slash is at the end (to indicate the path is a directory, for windows) NOTE: if the path is empty, no slash is added, or it'd mean "root directory"!
bool extEqualsIgnoreCase(const std::string& filename, const std::string& ext); //don't include the dot in ext


} //namespace lpi


