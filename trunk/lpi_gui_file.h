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
    FileList(const IGUIDrawer& geom);
    
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
    void deselectAll() { list.deselectAll(); }
};

/*
FileDialog is for selecting one or more files. It can be used both as "save" dialog and
as "load" dialog. The dialog itself isn't what will overwrite or load the file, the dialog
is there just to get a filename that the user selects.

TODO's:
[X] if it's a save dialog, ask confirmation if file exists
[X] ensure getting and setting current folder works (for remembering folder to initially open)
[ ] filtering by extension type (requires drop down list to choose from)
[ ] sort files alphabetically
[ ] show and sort files by date, size, ... (requires extensions to IFileBrowse and using a table instead of a simple List)
[ ] show some links to default or bookmarked paths on the left
[ ] double click on filename same functionality as pressing OK
[ ]
[ ]
*/
class FileDialog : public Dialog
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
    YesNoWindow overwriteQuestion;
    bool askingOverwrite;
    
    static std::string REMEMBER_PATH;
    
  public:

    FileDialog(const IGUIDrawer& geom, IFileBrowse* browser, bool save, bool multi);
    ~FileDialog();

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    virtual void manageHoverImpl(IHoverManager& hover);
    
    //the path is the folder the dialog is currently showing
    void setPath(const std::string& path); //it's allowed to give a path that includes a filename, only the folder part of it will be used.
    std::string getPath();
    
    size_t getNumFiles() const;
    std::string getFileName(size_t i); //returns full path
    std::string getFileName(); //returns first file if getNumFiles() > 0, empty string otherwise. This is useful if it's not a multi-file dialog.

};

} //namespace gui
} //namespace lpi
