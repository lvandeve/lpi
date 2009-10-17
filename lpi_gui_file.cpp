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
}

} //namespace gui
} //namespace lpi
