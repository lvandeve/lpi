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

namespace lpi
{
namespace gui
{

FileList::FileList(const IGUIDrawer& geom)
: ElementWrapper(&list)
, list(geom)
{
  generateIcons();
}

void FileList::generateIcons()
{
  const static std::string icon_file64 = "\
iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAAWElEQVR42mP8z/CfgRTARJJqIGCB\n\
sxiBEBX8x2Y5C4qK/wgVjEDAwIipB5+TgPoxrSXgB0w9LHhUgxwF8x7cbTg1oPmHWCdhguGgASWU\n\
kEMDF2D8T+vUCgAm3xYpZF64CAAAAABJRU5ErkJggg==";
  icon_file.texture = new TextureBuffer;
  lpi::loadTextureFromBase64PNG(icon_file.texture, icon_file64, lpi::AE_PinkKey);

  const static std::string icon_dir64 = "\
iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAAR0lEQVR42mP8z/CfgRTARJJqumhg\n\
gVCMQIgK/uPwGwuc9eCGHpytoHEJlxEsWI1B1gw3YtCG0iDUwAgJXcxQxwRQlf9pnVoBz+cSHrEY\n\
KNQAAAAASUVORK5CYII=";
  icon_dir.texture = new TextureBuffer;
  lpi::loadTextureFromBase64PNG(icon_dir.texture, icon_dir64, lpi::AE_PinkKey);
}

void FileList::generateListForDir(const std::string& dir, const IFileBrowse& filebrowser)
{
  list.clear();
  types.clear();
  
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

std::string FileDialog::REMEMBER_PATH;

FileDialog::FileDialog(const IGUIDrawer& geom, IFileBrowse* browser, bool save, bool multi)
: list(geom)
, save(save)
, multiselection(multi)
, browser(browser)
, overwriteQuestion(geom, "File exists. Overwrite?", "Confirm")
, askingOverwrite(false)
{
  addTop(geom);
  addTitle("File");
  addResizer(geom);
  addCloseButton(geom);
  
  ok.makeTextPanel(0, 0, save ? "save" : "open");
  cancel.makeTextPanel(0, 0, "cancel");
  up.makeTextPanel(0, 0, "up");

  pushTop(&ok, Sticky(1.0,-84, 1.0,-28, 1.0,-4, 1.0,-4));
  pushTop(&cancel, Sticky(1.0,-172, 1.0,-28, 1.0,-88, 1.0,-4));
  pushTop(&up, Sticky(1.0,-24, 0.0,4, 1.0,-4, 0.0,24));
  pushTop(&list, Sticky(0.0,4, 0.0,40, 1.0,-4, 1.0,-56));
  pushTop(&path, Sticky(0.0,4, 0.0,4, 1.0,-28, 0.0,24));
  pushTop(&file, Sticky(0.0,4, 1.0,-52, 1.0,-4, 1.0,-32));
  path.make(0, 0, 256);
  file.make(0, 0, 256);

  resize(0, 0, 600, 400);
  
  overwriteQuestion.setEnabled(false);
  
  if(REMEMBER_PATH.empty())
  {
#ifdef WIN32
    setPath("C:\\");
#else
    setPath("/");
#endif
  }
  else setPath(REMEMBER_PATH);
}

FileDialog::~FileDialog()
{
  std::string temp = getPath();
  if(!temp.empty()) REMEMBER_PATH = temp;
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

  drawElements(drawer);
}

void FileDialog::manageHoverImpl(IHoverManager& hover)
{
  if(overwriteQuestion.isEnabled())
    hover.addHoverElement(&overwriteQuestion);
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

    if(ok.clicked(input) || okThroughDoubleClickOnFile)
    {
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
    }
    if(cancel.clicked(input))
    {
      setEnabled(false);
      result = CANCEL;
    }
  }
}

std::string FileDialog::getPath()
{
  return path.getText();
}

void FileDialog::setPath(const std::string& path)
{
  std::string folder = getFileNamePathPart(path);
  this->path.setText(path);
  list.generateListForDir(path, *browser);
}

size_t FileDialog::getNumFiles() const
{
  if(multiselection) return selectedFiles.size();
  else return 1;
}

std::string FileDialog::getFileName(size_t i)
{
  if(multiselection) return browser->getChild(path.getText(), selectedFiles[i]);
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

////////////////////////////////////////////////////////////////////////////////


RecentFiles::RecentFiles()
: maxnum(8)
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
  if(!hasFile(file))
  {
    files.push_back(file);
    if(files.size() > maxnum) files.erase(files.begin());
  }
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
