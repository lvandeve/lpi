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

#include "lpi_persist.h"
#include "lpi_xml.h"
#include "lpi_file.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <Shlobj.h>
#include "lpi_filebrowse_win32.h"
#endif

namespace lpi
{

void APersist::setSetting(const std::string& chapter, const std::string& name, const std::string& value)
{
  settings[chapter + "." + name] = value;
}

void APersist::removeSetting(const std::string& chapter, const std::string& name)
{
  settings.erase(chapter + "." + name);
}

void APersist::clear()
{
  settings.clear();
}

bool APersist::hasSetting(const std::string& chapter, const std::string& name) const
{
  return settings.find(chapter + "." + name) != settings.end();
}

std::string APersist::getSetting(const std::string& chapter, const std::string& name, const std::string& defaultvalue) const
{
  Settings::const_iterator it = settings.find(chapter + "." + name);
  if(it == settings.end()) return defaultvalue;
  else return it->second;
}

APersist::APersist(const std::string& appuid, bool global)
: appuid(appuid)
, global(global)
{
}

APersist::~APersist()
{
}

void APersist::writeToXML(std::string& xml) const
{
  using namespace xml;
  
  XMLTree tree;
  tree.setType(ET_NESTED);
  tree.content.name = "settings";
  
  Settings::const_iterator it = settings.begin();
  while(it != settings.end())
  {
    XMLTree* sub = tree.createNewChild();
    sub->content.name = "setting";
    sub->attributes.resize(2);
    sub->attributes[0].name = "key";
    sub->attributes[0].value = it->first;
    sub->attributes[1].name = "value";
    sub->attributes[1].value = it->second;
    it++;
  }
  
  tree.generate(xml);
}

void APersist::readFromXML(const std::string& xml)
{
  using namespace xml;
  settings.clear();
  XMLTree tree;
  tree.parse(xml);
  for(size_t i = 0; i < tree.children.size(); i++)
  {
    XMLTree* sub = tree.children[i];
    if(sub->attributes.size() < 2) continue;
    std::string key = sub->attributes[0].value;
    std::string value = sub->attributes[1].value;
    settings[key] = value;
  }
}

#if defined(_WIN32)

PersistWin32::PersistWin32(const std::string& appuid, bool global)
: APersist(appuid, global)
{
  load();
}

PersistWin32::~PersistWin32()
{
  save();
}

std::string PersistWin32::getPath() const
{
  TCHAR szPath[MAX_PATH];
  
  int csidl = global ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA;

  if(SUCCEEDED(SHGetFolderPath(NULL,
                               csidl,
                               NULL,
                               0,
                               szPath)))
  {
    std::string path = szPath;
    ensureDirectoryEndBackslash(path);
    path += "lpi\\" + appuid + "\\settings.txt";
    return path;
  }
  else return "";
}


void PersistWin32::save() const
{
  std::string path = getPath();
  if(!path.empty())
  {
    std::string xml;
    writeToXML(xml);
    FileBrowseWin32 browse;
    browse.createDirectory(path);
    saveFile(xml, path);
  }
}

void PersistWin32::load()
{
  std::string path = getPath();
  if(!path.empty())
  {
    std::string xml;
    loadFile(xml, path);
    readFromXML(xml);
  }
}

#elif defined(linux)

PersistLinux::PersistLinux(const std::string& appuid, bool global)
: APersist(appuid, global)
{
  load();
}

PersistLinux::~PersistLinux()
{
  save();
}

void PersistLinux::save() const
{
}

void PersistLinux::load()
{
}

#endif

} //namespace lpi
