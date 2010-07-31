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

#if defined(LPI_OS_WINDOWS)
#include <windows.h>
#include <Shlobj.h>
#include "lpi_filebrowse.h"
#elif defined(LPI_OS_LINUX) || defined(LPI_OS_AMIGA) || defined(LPI_OS_AROS)
#include "lpi_filebrowse.h"
#include <cstdio>
#include <cstdlib>
#endif

namespace lpi
{

std::string IPersist::getSettingAndSet(const std::string& name, const std::string& defaultValue)
{
  std::string result = getSetting(name, defaultValue);
  setSetting(name, result);
  return result;
}


void APersist::setSetting(const std::string& name, const std::string& value)
{
  settings[name] = value;
}

void APersist::removeSetting(const std::string& name)
{
  settings.erase(name);
}

void APersist::clear()
{
  settings.clear();
}

bool APersist::hasSetting(const std::string& name) const
{
  return settings.find(name) != settings.end();
}

std::string APersist::getSetting(const std::string& name, const std::string& defaultValue) const
{
  Settings::const_iterator it = settings.find(name);
  if(it == settings.end()) return defaultValue;
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

#if defined(LPI_OS_WINDOWS)

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
    path += appuid + "\\settings.txt";
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
    FileBrowse browse;
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
    if(!xml.empty()) readFromXML(xml);
  }
}

#elif defined(LPI_OS_LINUX)

PersistLinux::PersistLinux(const std::string& appuid, bool global)
: APersist(appuid, global)
{
  load();
}

PersistLinux::~PersistLinux()
{
  save();
}

std::string PersistLinux::getPath() const
{
  if(global) return "/etc/" + appuid + "/settings.txt";
  else
  {
    std::string path;
    char* c = std::getenv("XDG_CONFIG_HOME");
    if(c != 0) path = c;
    if(!path.empty()) lpi::ensureDirectoryEndSlash(path);
    if(path.empty())
    {
      char* c = std::getenv("HOME");
      if(c != 0) path = c;
      if(!path.empty())
      {
        lpi::ensureDirectoryEndSlash(path);
        path += ".config/";
      }
    }
    return path + appuid + "/settings.txt";
  }
}

void PersistLinux::save() const
{
  std::string path = getPath();
  if(!path.empty())
  {
    std::string xml;
    writeToXML(xml);
    FileBrowse browse;
    browse.createDirectory(path);
    saveFile(xml, path);
  }
}

void PersistLinux::load()
{
  std::string path = getPath();
  if(!path.empty())
  {
    std::string xml;
    loadFile(xml, path);
    if(!xml.empty()) readFromXML(xml);
  }
}

#elif defined(LPI_OS_AMIGA) || defined(LPI_OS_AROS)

PersistAmiga::PersistAmiga(const std::string& appuid, bool global)
: APersist(appuid, global)
{
  load();
}

PersistAmiga::~PersistAmiga()
{
  save();
}

std::string PersistAmiga::getPath() const
{
 if(global) return "PROGDIR:" + appuid + "settings.txt";
 else
 {
   std::string path;
   char* c = "PROGDIR:"; // std::getenv("XDG_CONFIG_HOME");
   if(c != 0) path = c;
   if(!path.empty()) lpi::ensureDirectoryEndSlash(path);
   if(path.empty())
   {
     char* c = "PROGDIR:";
     if(c != 0) path = c;
     if(!path.empty())
     {
       lpi::ensureDirectoryEndSlash(path);
       path += ".config/";
     }
   }
   return path + appuid + "settings.txt";

 }
}

void PersistAmiga::save() const
{
 std::string path = "PROGDIR:settings.txt"; //getPath();
 if(!path.empty())
 {
   std::string xml;
   writeToXML(xml);
   FileBrowse browse;
   browse.createDirectory(path);
   saveFile(xml, path);
 }
}

void PersistAmiga::load()
{
 std::string path = "PROGDIR:settings.txt"; //getPath();
 if(!path.empty())
 {
   std::string xml;
   loadFile(xml, path);
   if(!xml.empty()) readFromXML(xml);
 }
}
#endif

} //namespace lpi
