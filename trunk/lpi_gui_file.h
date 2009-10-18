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

#pragma once

#include "lpi_gui_ex.h"
#include "lpi_gui_text.h"
#include "lpi_filebrowse.h"

namespace lpi
{
namespace gui
{

class FileList : public ElementWrapper
{
  public:
    enum ItemType
    {
      IT_FILE,
      IT_DIR/*,
      IT_LINK*/ //not used yet
    };
    
  private:

    List list;
    
    std::vector<ItemType> types;
    IFileBrowse* browser;
    HTexture icon_file;
    HTexture icon_dir;
    
    void generateIcons();

  public:
    FileList(const IGUIPartGeom& geom);
    
    void generateListForDir(const std::string& dir, const IFileBrowse& filebrowser);
    
    ItemType getType(size_t i);
    
    void getSelectedFiles(std::vector<std::string>& files); //only returns files, not directories that are selected
    void getSelectedDirectories(std::vector<std::string>& dirs); //only returns selected directories, not files
    
    void setAllowMultiSelection(bool set) { list.setAllowMultiSelection(set); }
    size_t getMouseItem(const IInput& input) const { return list.getMouseItem(input); }
    
    size_t getNumItems() const { return list.getNumItems(); }
    size_t getSelectedItem() const { return list.getSelectedItem(); }
    bool isSelected(size_t i) const { return list.isSelected(i); }
    const std::string& getValue(size_t i) const { return list.getValue(i); }
};

/*
FileDialog is for selecting one or more files. It can be used both as "save" dialog and
as "load" dialog. The dialog itself isn't what will overwrite or load the file, the dialog
is there just to get a filename that the user selects.
*/
class FileDialog : public Window
{
  private:
    FileList list; //the list of files in the current directory
    Button ok;
    Button cancel;
    Button up; //button to go one directory higher
    InputLine path; //the current directory (in which the files are shown)
    InputLine file; //the name of the currently selected file, only active when it's a save or single selection dialog
    bool save; //if false, it's the load files dialog, otherwise save files
    bool multiselection; //if false, only one file can be selected, if true multiple
    IFileBrowse* browser;
    std::vector<std::string> selectedFiles;
    
  public:

    FileDialog(const IGUIPartGeom& geom, IFileBrowse* browser);

    bool pressedOk(const IInput& input) { return ok.clicked(input); }
    bool pressedCancel(const IInput& input) { return cancel.clicked(input); }
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    void setPath(const std::string& path);
    
    size_t getNumFiles() const;
    std::string getFileName(size_t i); //returns full path

};

} //namespace gui
} //namespace lpi
