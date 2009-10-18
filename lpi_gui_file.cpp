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

FileList::FileList(const IGUIPartGeom& geom)
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
    list.addItem(filenames[i], &icon_file);
    types.push_back(IT_FILE);
  }
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

////////////////////////////////////////////////////////////////////////////////

FileDialog::FileDialog(const IGUIPartGeom& geom, IFileBrowse* browser)
: list(geom)
, browser(browser)
{
  addTop(geom);
  addTitle("File");
  addResizer(geom);
  addCloseButton(geom);
  
  pushTop(&ok, Sticky(1.0,-84, 1.0,-28, 1.0,-4, 1.0,-4));
  pushTop(&cancel, Sticky(1.0,-172, 1.0,-28, 1.0,-88, 1.0,-4));
  pushTop(&up, Sticky(1.0,-24, 0.0,4, 1.0,-4, 0.0,24));
  pushTop(&list, Sticky(0.0,4, 0.0,40, 1.0,-4, 1.0,-56));
  pushTop(&path, Sticky(0.0,4, 0.0,4, 1.0,-28, 0.0,24));
  pushTop(&file, Sticky(0.0,4, 1.0,-52, 1.0,-4, 1.0,-32));
  path.make(0, 0, 256);
  file.make(0, 0, 256);

  resize(0, 0, 600, 400);
  
  ok.makeTextPanel(0, 0, "ok");
  cancel.makeTextPanel(0, 0, "cancel");
  up.makeTextPanel(0, 0, "up");
  
#ifdef WIN32
  setPath("C:\\");
#else
  setPath("/");
#endif
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

void FileDialog::handleImpl(const IInput& input)
{
  Window::handleImpl(input);
  
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
  std::string filename;
  for(size_t i = 0; i < selectedFiles.size(); i++)
  {
    if(i > 0) filename += ", ";
    filename += selectedFiles[i];
  }
  file.setText(filename);
}

void FileDialog::setPath(const std::string& path)
{
  this->path.setText(path);
  list.generateListForDir(path, *browser);
}

size_t FileDialog::getNumFiles() const
{
  return selectedFiles.size();
}

std::string FileDialog::getFileName(size_t i)
{
  return browser->getChild(path.getText(), selectedFiles[i]);
}

} //namespace gui
} //namespace lpi