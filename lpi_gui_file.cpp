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

#include "lpi_gui_file.h"
#include "lpi_os.h"
#include "lpi_persist.h"
#include <iostream>

namespace lpi
{
namespace gui
{

FileList::FileList(const IGUIDrawer& geom)
: ElementWrapper(&list)
, list(geom)
{
  icon_file.texture = geom.createTexture();
  icon_dir.texture = geom.createTexture();
  geom.createIcon(*icon_file.texture, GI_FILE);
  geom.createIcon(*icon_dir.texture, GI_FOLDER);
}

void FileList::generateListForDir(const std::string& _dir, const IFileBrowse& filebrowser)
{
  list.clear();
  types.clear();
  
  std::string dir = _dir;
  filebrowser.fixSlashes(dir);
  filebrowser.ensureDirectoryEndSlash(dir);
  
  std::vector<std::string> dirs;
  filebrowser.getDirectories(dirs, dir);
  for(size_t i = 0; i < dirs.size(); i++)
  {
    list.addItem(dirs[i], &icon_dir);
    types.push_back(IT_DIR);
  }
  std::vector<std::string> filenames;
  filebrowser.getFiles(filenames, dir);
  for(size_t i = 0; i < filenames.size(); i++)
  {
    bool allow = false;
    if(!allowedExtensions.empty())
    {
      std::string ext = getFileNameExtPart(filenames[i], false);
      for(size_t j = 0; j < allowedExtensions.size(); j++)
      {
        if(equalsIgnoreCase(ext, allowedExtensions[j])) { allow = true; break; }
      }
    }
    else allow = true;
    if(!allow) continue;
    list.addItem(filenames[i], &icon_file);
    types.push_back(IT_FILE);
  }
  
  sort();
}

void FileList::getSelectedFiles(std::vector<std::string>& files)
{
  for(size_t i = 0; i < list.getNumItems(); i++)
  {
    if(list.isSelected(i) && getType(i) == IT_FILE) files.push_back(list.getValue(i));
  }
}

void FileList::getSelectedDirectories(std::vector<std::string>& dirs)
{
  for(size_t i = 0; i < list.getNumItems(); i++)
  {
    if(list.isSelected(i) && getType(i) == IT_DIR) dirs.push_back(list.getValue(i));
  }
}

FileList::ItemType FileList::getType(size_t i)
{
  return types[i];
}

void FileList::swap(size_t item1, size_t item2)
{
  list.swap(item1, item2);
  std::swap(types[item1], types[item2]);
}

void FileList::sort()
{
  //implemented with combsort here

  int amount = getNumItems();

  int gap = amount;
  bool swapped = false;
  while(gap > 1 || swapped)
  {
    gap = (gap * 10) / 13; //shrink factor 1.3
    if(gap == 9 || gap == 10) gap = 11;
    if (gap < 1) gap = 1;
    swapped = false;
    for (int i = 0; i < amount - gap; i++)
    {
      int j = i + gap;
      //check if item i < item j
      bool smaller = false;
      if(types[i] == IT_DIR && types[j] != IT_DIR) smaller = true;
      else if(types[i] != IT_DIR && types[j] == IT_DIR) smaller = false;
      else smaller = list.getValue(i) < list.getValue(j);
      
      if(!smaller)
      {
        swap(i, j);
        swapped = true;
      }
    }
  }
}

void FileList::setAllowedExtensions(const std::vector<std::string>& allowedExtensions)
{
  this->allowedExtensions = allowedExtensions;
}

////////////////////////////////////////////////////////////////////////////////


void FileDialogPersist::add(const std::string& path)
{
  for(size_t i = 0; i < sug.size(); i++)
  {
    if(sug[i] == path) return;
  }
  sug.push_back(path);
  if(sug.size() >= MAXPATHS)
  {
    sug.erase(sug.begin());
  }
}
    
std::string FileDialog::REMEMBER_PATH; //for remembering it during one session at least
bool FileDialog::REMEMBER_AUTO_ADD_EXTENSION = true;

FileDialog::FileDialog(const IGUIDrawer& geom, IFileBrowse* browser, bool save, bool multi, FileDialogPersist* persist)
: list(geom)
, save(save)
, multiselection(multi)
, browser(browser)
, persist(persist)
, overwriteQuestion(geom, "File exists. Overwrite?", "Confirm")
, askingOverwrite(false)
, extensionChooser(geom)
, suggestedFolders(geom)
{
  addTop(geom);
  addTitle(save ? "Save File" : "Open File");
  addResizer(geom);
  addCloseButton(geom);
  
  ok.makeTextPanel(0, 0, save ? "save" : "open");
  cancel.makeTextPanel(0, 0, "cancel");
  up.makeTextPanel(0, 0, "up");
  if(save)
  {
    autoAddExtension.make(0, 0, REMEMBER_AUTO_ADD_EXTENSION);
    autoAddExtension.makeLabel("Automatically add extension to filename", autoAddExtension.getSizeX(), autoAddExtension.getSizeY() / 2 - 4);
  }
  
  int s = save ? 20 : 0; //y shift for auto add extension checkbox (variable has short name on purpose)

  pushTop(&ok, Sticky(1.0,-84, 1.0,-50-s, 1.0,-4, 1.0,-30-s));
  pushTop(&cancel, Sticky(1.0,-84, 1.0,-24-s, 1.0,-4, 1.0,-4-s));
  pushTop(&up, Sticky(1.0,-84, 0.0,4, 1.0,-4, 0.0,24));
  pushTop(&path, Sticky(0.0,48, 0.0,4, 1.0,-88, 0.0,24));
  pushTop(&suggestedFolders, Sticky(0.0,48, 0.0,28, 1.0,-88, 0.0,48));
  pushTop(&list, Sticky(0.0,4, 0.0,52, 1.0,-4, 1.0,-56-s));
  pushTop(&file, Sticky(0.0,48, 1.0,-50-s, 1.0,-88, 1.0,-30-s));
  pushTop(&extensionChooser, Sticky(0.0,48, 1.0,-24-s, 1.0,-88, 1.0,-4-s));
  if(save) pushTop(&autoAddExtension, Sticky(0.0,48, 1.0,-20, 1.0,-88, 1.0,-4));
  path.make(0, 0, 256);
  file.make(0, 0, 256);

  resize(0, 0, 600, 400);
  
  overwriteQuestion.setEnabled(false);
  
  if(persist)
  {
    if(persist->sug.empty())
    {
      std::string home = browser->getDefaultDir(lpi::IFileBrowse::DD_HOME);
      if(!home.empty()) persist->add(home);
#ifdef LPI_OS_WINDOWS
      persist->add("C:\\");
#else
      persist->add("/");
      persist->add("/mnt/");
#endif
    }
    
    for(size_t i = 0; i < persist->sug.size(); i++)
    {
      suggestedFolders.addItem(persist->sug[persist->sug.size() - i - 1]);
    }
  }
  else //no persistent data to get directory history from. Instead just suggest some locations based on the OS
  {
    std::string home = browser->getDefaultDir(lpi::IFileBrowse::DD_HOME);
    if(!home.empty()) suggestedFolders.addItem(home);
#ifdef LPI_WIN32
    suggestedFolders.addItem("C:\\");
#else
    suggestedFolders.addItem("/");
    suggestedFolders.addItem("/mnt/");
#endif
  }
  
  if(REMEMBER_PATH.empty())
  {
#ifdef LPI_OS_WINDOWS
    setPath("C:\\");
#else
    setPath("/");
#endif
  }
  else setPath(REMEMBER_PATH);
  
  file.activate(true);
  suggestedFolders.setSelectedItem((size_t)(-1)); //never display anything by default or it is confusing to the user
  suggestedFolders.hasChanged(); //reset the "has changed" value to false
  
  list.setAllowMultiSelection(multi);
}

FileDialog::~FileDialog()
{
  std::string temp = getPath();
  if(!temp.empty())
  {
    browser->ensureDirectoryEndSlash(temp);
    REMEMBER_PATH = temp;
  }
}

void FileDialog::drawImpl(IGUIDrawer& drawer) const
{
  drawWindow(drawer);
  
  drawer.drawRectangle(list.getX0(), list.getY0(), list.getX1(), list.getY1(), RGB_White, true);
  drawer.drawRectangle(list.getX0(), list.getY0(), list.getX1(), list.getY1(), RGB_Black, false);
  drawer.drawRectangle(path.getX0(), path.getY0(), path.getX1(), path.getY1(), RGB_White, true);
  drawer.drawRectangle(path.getX0(), path.getY0(), path.getX1(), path.getY1(), RGB_Black, false);
  drawer.drawRectangle(file.getX0(), file.getY0(), file.getX1(), file.getY1(), RGB_White, true);
  drawer.drawRectangle(file.getX0(), file.getY0(), file.getX1(), file.getY1(), RGB_Black, false);
  
  drawer.drawText("dir:", path.getX0() - 4, path.getCenterY(), lpi::FONT_White, lpi::TextAlign(HA_RIGHT, VA_CENTER));
  drawer.drawText("sug:", suggestedFolders.getX0() - 4, suggestedFolders.getCenterY(), lpi::FONT_White, lpi::TextAlign(HA_RIGHT, VA_CENTER));
  drawer.drawText("file:", file.getX0() - 4, file.getCenterY(), lpi::FONT_White, lpi::TextAlign(HA_RIGHT, VA_CENTER));
  drawer.drawText(save ? "type:":"ext:", extensionChooser.getX0() - 4, extensionChooser.getCenterY(), lpi::FONT_White, lpi::TextAlign(HA_RIGHT, VA_CENTER));

  drawElements(drawer);
}

void FileDialog::manageHoverImpl(IHoverManager& hover)
{
  if(overwriteQuestion.isEnabled())
    hover.addHoverElement(&overwriteQuestion);
  Dialog::manageHoverImpl(hover);
}

void FileDialog::handleImpl(const IInput& input)
{
  if(askingOverwrite)
  {
    if(overwriteQuestion.done())
    {
      askingOverwrite = false;
      if(overwriteQuestion.getValue())
      {
        setEnabled(false);
        result = OK;
      }
    }
  }
  else
  {
    Window::handleImpl(input);
    
    bool okThroughDoubleClickOnFile = false;

    if(path.enteringDone() && !path.getText().empty())
    {
      list.generateListForDir(path.getText(), *browser);
    }
    if(list.mouseDoubleClicked(input))
    {
      size_t i = list.getMouseItem(input);
      if(i < list.getNumItems())
      {
        lpi::gui::FileList::ItemType type = list.getType(i);
        if(type == lpi::gui::FileList::IT_DIR)
        {
          std::string dir = path.getText();
          std::string value = list.getValue(i);
          if(value == ".")
          {
            //nothing; it's the same dir
          }
          else if(value == "..")
          {
            std::string parent = browser->getParent(dir);
            setPath(parent);
          }
          else
          {
            dir = browser->getChild(dir, value);
            path.setText(dir);
            list.generateListForDir(dir, *browser);
          }
        }
        else if(type == lpi::gui::FileList::IT_FILE)
        {
          okThroughDoubleClickOnFile = true;
        }
      }
    }

    if(up.clicked(input))
    {
      std::string parent = browser->getParent(path.getText());
      setPath(parent);
    }

    //std::string filename = list.getSelectedItem() < list.getNumItems() ? list.getValue(list.getSelectedItem()) : "";
    selectedFiles.clear();
    list.getSelectedFiles(selectedFiles);
    if(file.isControlActive())
    {
      list.deselectAll();
    }
    else if(!selectedFiles.empty())
    {
      std::string filename;
      for(size_t i = 0; i < selectedFiles.size(); i++)
      {
        if(i > 0) filename += ", ";
        filename += selectedFiles[i];
      }
      file.setText(filename);
    }

    if(ok.clicked(input) || okThroughDoubleClickOnFile || file.enter())
    {
      if(save)
      {
        REMEMBER_AUTO_ADD_EXTENSION = autoAddExtension.isChecked();
        if(REMEMBER_AUTO_ADD_EXTENSION) doAutoAddExtension();
      }

      if(save && browser->fileExists(getFileName()))
      {
        overwriteQuestion.moveCenterTo(getCenterX(), getCenterY());
        overwriteQuestion.setEnabled(true);
        askingOverwrite = true;
      }
      else
      {
        setEnabled(false);
        result = OK;
      }
      
      std::string sugFolder = path.getText();
      browser->ensureDirectoryEndSlash(sugFolder);
      if(persist) persist->add(sugFolder);
    }
    if(cancel.clicked(input) || closeButton.clicked(input))
    {
      setEnabled(false);
      result = CANCEL;
      if(save) REMEMBER_AUTO_ADD_EXTENSION = autoAddExtension.isChecked();
    }
    
    if(extensionChooser.hasChanged() && extensionChooser.getSelectedItem() < extensionChooser.getNumItems())
    {
      setAllowedExtensions(extensionSets[extensionChooser.getSelectedItem()]);
    }
    
    if(suggestedFolders.hasChanged() && suggestedFolders.getSelectedItem() < suggestedFolders.getNumItems())
    {
      setPath(suggestedFolders.getValue(suggestedFolders.getSelectedItem()));
      suggestedFolders.setSelectedItem((size_t)(-1));
    }
  }
}

void FileDialog::doAutoAddExtension()
{
  /*
  The first extension of the active extension set is added if the extension
  is not already any of the ones in the extension set.
  */
  
  if(extensionSets.empty()) return;
  
  std::vector<std::string>& exts = extensionSets[getExtensionSet()];
  
  if(exts.empty()) return;
  
  std::string filename = file.getText();
  
  
  bool hasExt = false;
  for(size_t i = 0; i < exts.size(); i++)
  {
    if(extEqualsIgnoreCase(filename, exts[i]))
    {
      hasExt = true;
      break;
    }
  }
  if(!hasExt)
  {
    filename += ".";
    filename += exts[0];
  }
  
  file.setText(filename);
}

std::string FileDialog::getPath()
{
  return path.getText();
}

void FileDialog::setPath(const std::string& path)
{
  std::string folder = getFileNamePathPart(path);
  this->path.setText(folder);
  list.generateListForDir(folder, *browser);
}

size_t FileDialog::getNumFiles() const
{
  if(multiselection)
  {
    return selectedFiles.empty() ? 1 : selectedFiles.size();
  }
  else return 1;
}

std::string FileDialog::getFileName(size_t i)
{
  if(multiselection && !selectedFiles.empty()) return browser->getChild(path.getText(), selectedFiles[i]);
  else return browser->getChild(path.getText(), file.getText());
}

std::string FileDialog::getFileName()
{
  return getNumFiles() > 0 ? getFileName(0) : "";
}

void FileDialog::setAllowedExtensions(const std::vector<std::string>& allowedExtensions)
{
  list.setAllowedExtensions(allowedExtensions);
  list.generateListForDir(path.getText(), *browser); //regenerate the list
}

size_t FileDialog::addExtensionSet(const std::string& name, const std::vector<std::string>& extensions)
{
  extensionChooser.addItem(name);
  extensionSets.push_back(extensions);
  return extensionSets.size() - 1;
}

size_t FileDialog::getExtensionSet() const
{
  return extensionChooser.getSelectedItem();
}

////////////////////////////////////////////////////////////////////////////////


RecentFiles::RecentFiles(size_t maxnum)
: maxnum(maxnum)
{
}

bool RecentFiles::hasFile(const std::string& file) const
{
  for(size_t i = 0; i < files.size(); i++) if(files[i] == file) return true;
  return false;
}

size_t RecentFiles::getNumFiles() const
{
  return files.size();
}

std::string RecentFiles::getFile(size_t i) const
{
  return files[i];
}

std::string RecentFiles::getFileDisplayName(size_t i) const
{
  std::string result = files[i];
  result = lpi::getFileNameFileExtPart(result);
  return result;
}

void RecentFiles::addFile(const std::string& file) //adds the file unless it is already in the list
{
  for(size_t i = 0; i < files.size(); i++)
  {
    if(files[i] == file)
    {
      files.erase(files.begin() + i);
      break;
    }
  }
  
  files.push_back(file);
  if(files.size() > maxnum) files.erase(files.begin());
}

////////////////////////////////////////////////////////////////////////////////

void RecentFilesMenu::regenerate(const lpi::gui::IGUIDrawer& geom)
{
  clear();
  for(size_t i = 0; i < recent.getNumFiles(); i++) addCommand(recent.getFileDisplayName(getNumFiles() - i - 1), geom);
}

bool RecentFilesMenu::hasFile(const std::string& file) const
{
  return recent.hasFile(file);
}

size_t RecentFilesMenu::getNumFiles() const
{
  return recent.getNumFiles();
}

std::string RecentFilesMenu::getFile(size_t i) const
{
  return recent.getFile(getNumFiles() - i - 1);
}

void RecentFilesMenu::addFile(const std::string& file, const IGUIDrawer& geom)
{
  recent.addFile(file);
  regenerate(geom);
}

RecentFiles& RecentFilesMenu::getRecent() { return recent; }
const RecentFiles& RecentFilesMenu::getRecent() const { return recent; }

} //namespace gui
} //namespace lpi
