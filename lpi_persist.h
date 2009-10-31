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

#include <string>
#include <map>
#include <vector>

/*
lpi_persist is for remembering settings of your program between sessions. They
are saved there where the operation system has reserved the best space to save
per-user or global settings. How they're actually saved is completely up to the
implementation here, you don't need to know whether it's in a file, registry, xml,
binary or plain text. All you need to know is that each setting is a key value pair
where both the key and the value are a string and that you can choose the setting
to be "per-user" or "system-wide", and that you need to give a unique identifier
representing your program to avoid clashes with other programs using lpi_persist. The
key exists out of two parts: "chapter" and "name", but those two are just added together
to form the actual key. You must use a different IPersist class for per-user
and system-wide settings.

Here's how the implementation currently works though:
The settings are saved as an xml file on disk (with txt as extension for convenience), in a location suitable for the OS.
The name of the subfolder is determinated by the unique application ID you give (YOURAPPUID).
Currently only two OS'es are supported but this might be increased later.
In windows, it saves the files in:
- CSIDL_APPDATA\lpi\YOURAPPUID\settings.txt for per-user settings
- CSIDL_COMMON_APPDATA\lpi\YOURAPPUID\settings.txt for system-wide settings
In linux, it saves the files in:
- ~/.lpi/YOURAPPUID/settings.txt for per-user settings
- /etc/.lpi/YOURAPPUID/settings.txt for global settings
*/

namespace lpi
{

class IPersist
{
  public:
    virtual void setSetting(const std::string& chapter, const std::string& name, const std::string& value) = 0;
    virtual void removeSetting(const std::string& chapter, const std::string& name) = 0;
    virtual void clear() = 0;
    virtual bool hasSetting(const std::string& chapter, const std::string& name) const = 0;
    virtual std::string getSetting(const std::string& chapter, const std::string& name, const std::string& defaultvalue = "") const = 0;

    virtual void save() const = 0;
    virtual void load() = 0;
    
    virtual ~IPersist(){};
};

class APersist : public IPersist
{
  protected:
    std::string appuid; //uid of your application so that the settings are saved in a unique directory
    typedef std::map<std::string, std::string> Settings;
    Settings settings;
    bool global;
    
    void writeToXML(std::string& xml) const;
    void readFromXML(const std::string& xml);
    
  public:
    virtual void setSetting(const std::string& chapter, const std::string& name, const std::string& value);
    virtual void removeSetting(const std::string& chapter, const std::string& name);
    virtual void clear();
    virtual bool hasSetting(const std::string& chapter, const std::string& name) const;
    virtual std::string getSetting(const std::string& chapter, const std::string& name, const std::string& defaultvalue = "") const;

    APersist(const std::string& appuid, bool global); //pathuid must be something that can be a directory name! For safety use only letters and numbers.
    ~APersist();
};

#if defined(_WIN32)

class PersistWin32 : public APersist
{
  protected:
    std::string getPath() const;
  public:
    PersistWin32(const std::string& appuid, bool global);
    ~PersistWin32();
    
    virtual void save() const;
    virtual void load();
};

typedef PersistWin32 Persist;

#elif defined(linux)

class PersistLinux : public APersist
{
  public:
    PersistLinux(const std::string& appuid, bool global);
    ~PersistLinux();
    
    virtual void save() const;
    virtual void load();
};

typedef PersistLinux Persist;
#endif

} //namespace lpi
