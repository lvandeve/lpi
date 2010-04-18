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

#include <iostream>

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
    
    std::vector<std::string> allowedExtensions; //if empty, all extensions are allowed
    

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
    
    void swap(size_t item1, size_t item2); //swapping location of two items, e.g. for sorting
    void sort();
    
    void setAllowedExtensions(const std::vector<std::string>& allowedExtensions);
};

struct FileDialogPersist
{
  static const size_t MAXPATHS = 10;
  std::vector<std::string> sug;
  
  void add(const std::string& path);
  
   /*
    Note: you still have to implement functions yourself to add these to your lpi::Persist object. Example
    of such functions:
    
    void fileDialogToPersist(lpi::Persist& persist, const lpi::gui::FileDialogPersist& fd, const std::string& name)
    {
      bool more = true;
      size_t i = 0;
      while(more)
      {
        std::stringstream ss;
        ss << name << ".filedialogsug." << i;
        if(persist.hasSetting(ss.str()))
        {
          fd.sug.push_back(persist.getSetting(ss.str()));
          i++;
        }
        else more = false;
      }
    }

    void persistToFileDialog(lpi::gui::FileDialogPersist& recent, const lpi::Persist& fd, const std::string& name)
    {
      for(size_t i = 0; i < fd.sug.size(); i++)
      {
        std::stringstream ss;
        ss << name << ".filedialogsug." << i;
        persist.setSetting(ss.str(), fd.sug[i]);
      }
    }
    */
};

/*
FileDialog is for selecting one or more files. It can be used both as "save" dialog and
as "load" dialog. The dialog itself isn't what will overwrite or load the file, the dialog
is there just to get a filename that the user selects.

TODO's:
[X] if it's a save dialog, ask confirmation if file exists
[X] ensure getting and setting current folder works (for remembering folder to initially open)
[X] filtering by extension type (requires drop down list to choose from)
[X] sort files alphabetically
[ ] show and sort files by date, size, ... (requires extensions to IFileBrowse and using a table instead of a simple List)
[+] show some links to default or bookmarked paths on the left (home folder of linux, my documents of windows, some remembered ones...)
[X] double click on filename same functionality as pressing OK
[X] pressing enter after typing in file inputline same functionality as pressing OK
[ ] ensure that it works with multiple file choosing
[ ] add "create directory" button
[ ] BUG: when using modal file dialog, and dropdown list is over ok button, you click ok through the list
[X] add some helpful text labels indicating what they are left of the "path", "file" and "extensionChooser"
[ ] add some helpful tooltips over the various areas (such as sug, path, file)
[X] let the close button do the same as the cancel button
[ ] also make a folder browsing dialog
[X] in save dialog, add a "automatically add extension" checkbox. If true and you type filename without extension from the list, it adds the first extension from the chosen type to the filename.
[X] instead of bookmarked paths and such, use a dropdown with 10 suggested locations, which are actually the 10 most recent directories where you pressed "ok". Then each folder the user is currently busy in is guaranteed to be only 2 mouse clicks away without any thinking or bookmarking needed!
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
    FileDialogPersist* persist;
    std::vector<std::string> selectedFiles;
    YesNoWindow overwriteQuestion;
    bool askingOverwrite;
    Checkbox autoAddExtension; //when saving
    
    std::vector<std::vector<std::string> > extensionSets; //each "set" represents a list of allowed extensions. To indicate any file ("*.*"), use an EMPTY std::vector<std::string>, do NOT use a vector containing "*" for it because it'll literally try to find for extensions "dot asterix". If there are no extension sets at all, simply anything is shown. If there are multiple sets, they are chooseable in a dropdown box.
    DropDownList extensionChooser; //each extension set
    DropDownList suggestedFolders;
    
    static std::string REMEMBER_PATH; //the least that can be done is automatically let all dialogs default to the last path if none is specified so that during one session of the application the user has to browse there max once. In programs using FileDialog I recommend remembering the path in a Persist and managing different dialogs for different file sources separatly instead of relying on this static behaviour.
    static bool REMEMBER_AUTO_ADD_EXTENSION;
    
    void doAutoAddExtension();
    
  public:

    FileDialog(const IGUIDrawer& geom, IFileBrowse* browser, bool save, bool multi, FileDialogPersist* persist);
    ~FileDialog();

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    virtual void manageHoverImpl(IHoverManager& hover);
    
    //the path is the folder the dialog is currently showing
    //TODO: setting the actual file name should also be possible (e.g. when saving a file...)
    void setPath(const std::string& path); //it's allowed to give a path that includes a filename, only the folder part of it will be used.
    std::string getPath();
    
    size_t getNumFiles() const;
    std::string getFileName(size_t i); //returns full path
    std::string getFileName(); //returns first file if getNumFiles() > 0, empty string otherwise. This is useful if it's not a multi-file dialog.

    void setAllowedExtensions(const std::vector<std::string>& allowedExtensions); //this has not much to do with addExtensionSet. This is called when the user selects one from the dropdown list. But can also be called programatically (e.g. when just setting a fixed filter and not using the dropdown list at all)

    size_t addExtensionSet(const std::string& name, const std::vector<std::string>& extensions); //give empty vector to mean "any"
    size_t getExtensionSet() const; //check which type or extension set was selected. This is useful for choosing to which file type your data has to be encoded (better than parsing the extension of getFileName())
};

class RecentFiles //convenient helper class
{
  protected:
    std::vector<std::string> files;
    size_t maxnum;

  public:

    RecentFiles(size_t maxnum = 8);
    bool hasFile(const std::string& file) const;
    size_t getNumFiles() const;
    std::string getFile(size_t i) const;
    std::string getFileDisplayName(size_t i) const;
    void addFile(const std::string& file); //adds the file unless it is already in the list
    
    /*
    Note: you still have to implement functions yourself to add these to your lpi::Persist object. Example
    of such functions:
    
    void recentFilesToPersist(lpi::Persist& persist, const lpi::gui::RecentFiles& recent, const std::string& name)
    {
      for(size_t i = 0; i < recent.getNumFiles(); i++)
      {
        std::stringstream ss;
        ss << name << "." << i;
        persist.setSetting(ss.str(), recent.getFile(i));
      }
    }

    void persistToRecentFiles(lpi::gui::RecentFiles& recent, const lpi::Persist& persist, const std::string& name)
    {
      bool more = true;
      size_t i = 0;
      while(more)
      {
        std::stringstream ss;
        ss << name << "." << i;
        if(persist.hasSetting(ss.str()))
        {
          recent.addFile(persist.getSetting(ss.str()));
          i++;
        }
        else more = false;
      }
    }
    */
};

class RecentFilesMenu : public lpi::gui::MenuVertical //convenient helper class
{
  protected:
    RecentFiles recent;
    
  public:
  
    RecentFilesMenu(size_t maxnum = 8) : recent(maxnum) {}

    void regenerate(const lpi::gui::IGUIDrawer& geom);
    
    bool hasFile(const std::string& file) const;
    size_t getNumFiles() const;
    std::string getFile(size_t i) const;
    void addFile(const std::string& file, const IGUIDrawer& geom); //adds the file unless it is already in the list
    
    RecentFiles& getRecent();
    const RecentFiles& getRecent() const;
};

} //namespace gui
} //namespace lpi
