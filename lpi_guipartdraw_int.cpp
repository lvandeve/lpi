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



#include "lpi_guipartdraw_int.h"

#include "lodepng.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include "lpi_input_sdl.h"

#include <iostream>

namespace lpi
{
namespace gui
{

const std::string& getBuiltInGuiTextureData()
{
  /*
  PNG image, encoded in base64, containing all the GUI images
  First use the function decodeBase64
  Then use the function LodePNG::Decoder::decode
  And you get the pixels in a buffer
  */
  static const std::string builtInGuiData = "\
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\n\
AElFTkSuQmCC\n\
";

  return builtInGuiData;
}

const std::string& getBuiltInIconData()
{
static const std::string builtInIconData = "\
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\n\
wHow1h2BlvQInFMLmO9qPQH7jz/1z6XMG7j1lH+sFjDfVXoCltIe5GsGbus51vywzu49Ac+Zuwa3\n\
poft7dYTsG/82cs1fxc8kofrCQjMd5WegMBt2r0nIHC77qIhCLCPrv/4UL0lPffOPXb4eOB2fc4A\n\
zvXcOz6mNdBdNAT36Vcp06f5ttpx1az54T419wHUZwMOzTlRyFEFuA96AqqnXnA9RwEg2FUDwEU7\n\
cFuuFgBd6UrfCwG4JTYBINhVAuC49i+lmAXADbnuPgDjHm7K7gEwXPsfmQXAbbAPAILtGgD12r/+\n\
2SwAfpZ9ABBstwAY2/avl80C4Ofs9tFgfelLN2OVr4En/JxdPxvQ4Ibb5igABBMAEGzzfgDA/TAD\n\
gGACAIIJAAi2+WHAW+t5pp566o0zA4BgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJAAgm\n\
ACCYAIBgAgCCCQAIpicgBDMDgGACAIIJAAimJ6B66gXXMwOAYAIAggkACCYAIJgAgGACAIIJAAgm\n\
ACCYAIBgAgCCCQAIJgAgmACAYAIAggkACKYnIAQzA4BgAgCCCQAIpiegeuoF1zMDgGACAIIJAAgm\n\
ACCYAIBgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJAAimJyAEMwOAYAIAggkACKYnoHrq\n\
BdczA4BgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIpicgBDMD\n\
gGACAIIJAAimJ6B66gXXMwOAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACA\n\
YAIAggkACKYnIAQzA4BgAgCCCQAIpiegeuoF1zMDgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACA\n\
YAIAggkACCYAIJgAgGACAIIJAAimJyAEMwOAYAIAggkACKYnoHrqBdczA4BgAgCCCQAIJgAgmACA\n\
YAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIpicgBDMDgGACAIIJAAimJ6B66gXXMwOA\n\
YAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACKYnIAQzA4BgAgCC\n\
CQAIpiegeuoF1zMDgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJ\n\
AAimJyAEMwOAYAIAggkACKYnoHrqBdczA4BgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJ\n\
AAgmACCYAIBgAgCCCQAIpicgBDMDgGACAIIJAAimJ6B66gXXMwOAYAIAggkACCYAIJgAgGACAIIJ\n\
AAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACKYnIAQzA4BgAgCCCQAIpiegeuoF1zMDgGACAIIJ\n\
AAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJAAimJyAEMwOAYAIAggkACKYn\n\
oHrqBdczA4BgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIpicg\n\
BDMDgGACAIIJAAimJ6B66gXXMwOAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIJgAg\n\
mACAYAIAggkACKYnIAQzA4BgAgCCCQAIpiegeuoF1zMDgGACAIIJAAgmACCYAIBgAgCCCQAIJgAg\n\
mACAYAIAggkACCYAIJgAgGACAIIJAAimJyAEMwOAYAIAggkACKYnoHrqBdczA4BgAgCCCQAIJgAg\n\
mACAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIpicgBDMDgGACAIIJAAimJ6B66gXX\n\
MwOAYAIAggkACCYAIJgAgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACKYnIAQzA4Bg\n\
AgCCCQAIpiegeuoF1zMDgGACAIIJAAgmACCYAIBgAgCCCQAIJgAgmACAYAIAggkACCYAIJgAgGAC\n\
AIIJAAimJyAEMwOAYAIAggkACCYAIJgAgGCfXYHf+u/dRZ+712+3AY/jqZT3wd8a7GO3A4/hZBPg\n\
rX/5/Kpvn3pc/ZxzderH1s87t1zXqV/Hudd17nVCim/7AJ6711lr/foxz93rt4F0rs5xhjF83rnl\n\
OXWH929RDx7VbjsB5w6qpWvfJYN/i3rwyL4FwNiUuDV9ru8fDqZWnePat15DT5mq26q3dEpvE4Bk\n\
3T/l0NsJCJk+LwZyGBDyuBoQgjkTEIL9Hx1vJSU16qibAAAAAElFTkSuQmCC";

  return builtInIconData;
}

void GUIPartDrawerInternal::initBuiltInGui(const ITextureFactory& factory)
{
  initBuiltInGuiTextures64(factory, getBuiltInGuiTextureData());
}
void GUIPartDrawerInternal::initBuiltInIcons()
{
  initBuiltInIcons64(getBuiltInIconData());
}

void GUIPartDrawerInternal::initBuiltInGuiTextures(const ITextureFactory& factory, const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiTextures(factory, png);
}


void GUIPartDrawerInternal::initBuiltInGuiTextures64(const ITextureFactory& factory, const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiTextures(factory, decoded64);
}

void GUIPartDrawerInternal::initBuiltInGuiTextures(const ITextureFactory& factory, const std::vector<unsigned char>& png)
{
  (void)factory;
  
  
  //these are normally defined as static const in headers, but if GUIDrawer is declared outside of any function, due to order of loading this may be called before ColorRGB's for this translation unit are initialized. So define copies here.
  static const ColorRGB RGB_Black(  0,   0,   0, 255);
  static const ColorRGB RGB_White        (255, 255, 255, 255);
  static const ColorRGB RGB_Magenta      (255,   0, 255, 255);
  static const ColorRGB RGB_Red          (255,   0,   0, 255);
  static const ColorRGB RGB_Gray         (128, 128, 128, 255);
  static const ColorRGB RGB_Brightred    (255, 192, 192, 255);
  static const AlphaEffect AE_PinkKey(128, 255, RGB_Magenta);
  
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> dataBuffer;
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data

  /*
    Below the built in gui textures are loaded from the buffer.
    Coordinates gotten by looking at the texture that's encoded here, in a painting program.
  */
  
  //window panel
  makeTextureFromBuffer(builtInTexture[ 0], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 20, 24, 24);
  makeTextureFromBuffer(builtInTexture[ 1], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 20, 30, 24);
  makeTextureFromBuffer(builtInTexture[ 2], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 20, 36, 24);
  makeTextureFromBuffer(builtInTexture[ 3], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 26, 24, 30);
  makeTextureFromBuffer(builtInTexture[ 4], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 26, 30, 30);
  makeTextureFromBuffer(builtInTexture[ 5], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 26, 36, 30);
  makeTextureFromBuffer(builtInTexture[ 6], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 32, 24, 36);
  makeTextureFromBuffer(builtInTexture[ 7], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 32, 30, 36);
  makeTextureFromBuffer(builtInTexture[ 8], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 32, 36, 36);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.windowTextures[i] = builtInTexture[i];
  
  //button normal
  makeTextureFromBuffer(builtInTexture[ 9], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 60, 24, 64);
  makeTextureFromBuffer(builtInTexture[10], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 60, 30, 64);
  makeTextureFromBuffer(builtInTexture[11], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 60, 36, 64);
  makeTextureFromBuffer(builtInTexture[12], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 66, 24, 70);
  makeTextureFromBuffer(builtInTexture[13], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 66, 30, 70);
  makeTextureFromBuffer(builtInTexture[14], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 66, 36, 70);
  makeTextureFromBuffer(builtInTexture[15], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 72, 24, 76);
  makeTextureFromBuffer(builtInTexture[16], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 72, 30, 76);
  makeTextureFromBuffer(builtInTexture[17], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 72, 36, 76);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonTextures[i] = builtInTexture[i + 9];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonOverTextures[i] = builtInTexture[i + 9];
  
  //button pressed
  makeTextureFromBuffer(builtInTexture[18], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 60, 44, 64);
  makeTextureFromBuffer(builtInTexture[19], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 60, 50, 64);
  makeTextureFromBuffer(builtInTexture[20], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 60, 56, 64);
  makeTextureFromBuffer(builtInTexture[21], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 66, 44, 70);
  makeTextureFromBuffer(builtInTexture[22], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 66, 50, 70);
  makeTextureFromBuffer(builtInTexture[23], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 66, 56, 70);
  makeTextureFromBuffer(builtInTexture[24], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 72, 44, 76);
  makeTextureFromBuffer(builtInTexture[25], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 72, 50, 76);
  makeTextureFromBuffer(builtInTexture[26], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 72, 56, 76);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonDownTextures[i] = builtInTexture[i + 18];
  
  //button arrow up
  makeTextureFromBuffer(builtInTexture[27], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 100, 76, 116);
  builtInGuiSet.arrowN = builtInTexture[27];
  //button arrow down
  makeTextureFromBuffer(builtInTexture[28], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 100, 116, 116);
  builtInGuiSet.arrowS = builtInTexture[28];
  //button arrow left
  makeTextureFromBuffer(builtInTexture[29], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 80, 76, 96);
  builtInGuiSet.arrowW = builtInTexture[29];
  //button arrow right
  makeTextureFromBuffer(builtInTexture[30], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 80, 116, 96);
  builtInGuiSet.arrowE = builtInTexture[30];
  
  //scroller
  makeTextureFromBuffer(builtInTexture[31], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 80, 56, 96);
  builtInGuiSet.emptyButton = builtInTexture[31];
  builtInGuiSet.scroller = builtInTexture[31];
  //scrollbar background
  makeTextureFromBuffer(builtInTexture[32], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 80, 36, 96);
  builtInGuiSet.scrollbarBackground = builtInTexture[32];
  builtInGuiSet.scrollBarPairCorner = builtInTexture[32];
  
  //check box unchecked
  makeTextureFromBuffer(builtInTexture[33], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 180, 36, 196);
  builtInGuiSet.checkBox[0] = builtInTexture[33];
  //check box checked
  makeTextureFromBuffer(builtInTexture[34], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 180, 76, 196);
  builtInGuiSet.checkBox[1] = builtInTexture[34];
  
  //bullet unchecked
  makeTextureFromBuffer(builtInTexture[35], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 220, 36, 236);
  builtInGuiSet.bullet[0] = builtInTexture[35];
  //bullet checked
  makeTextureFromBuffer(builtInTexture[36], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 220, 76, 236);
  builtInGuiSet.bullet[1] = builtInTexture[36];
  
  //slider horizontal line
  makeTextureFromBuffer(builtInTexture[41], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 140, 24, 144);
  makeTextureFromBuffer(builtInTexture[42], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 140, 30, 144);
  makeTextureFromBuffer(builtInTexture[43], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 140, 36, 144);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.hline[i] = builtInTexture[i + 41];
  
  //slider vertical line
  makeTextureFromBuffer(builtInTexture[44], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 140, 64, 144);
  makeTextureFromBuffer(builtInTexture[45], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 146, 64, 150);
  makeTextureFromBuffer(builtInTexture[46], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 152, 64, 156);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.vline[i] = builtInTexture[i + 44];
  
  //window top bar (also a horizontal line)
  makeTextureFromBuffer(builtInTexture[47], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 20, 104, 36);
  makeTextureFromBuffer(builtInTexture[48], &dataBuffer[0], GDW, GDH, AE_PinkKey, 106, 20, 110, 36);
  makeTextureFromBuffer(builtInTexture[49], &dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 20, 116, 36);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.windowTop[i] = builtInTexture[i + 47];
  
  //round button
  makeTextureFromBuffer(builtInTexture[51], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 140, 56, 156);
  builtInGuiSet.roundButton = builtInTexture[51];
  builtInGuiSet.slider = builtInTexture[51];
  
  //close button
  makeTextureFromBuffer(builtInTexture[54], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 40, 36, 56);
  builtInGuiSet.closeButton = builtInTexture[54];

  //minimize button
  //55
  
  //maximize button
  //56
  
  //????
  //57
  
  //resizer of window (corner at bottom right)
  makeTextureFromBuffer(builtInTexture[58], &dataBuffer[0], GDW, GDH, AE_PinkKey, 140, 20, 156, 36);
  builtInGuiSet.resizer = builtInTexture[58];
  
  //tab unselected
  makeTextureFromBuffer(builtInTexture[60], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 260, 24, 264);
  makeTextureFromBuffer(builtInTexture[61], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 260, 30, 264);
  makeTextureFromBuffer(builtInTexture[62], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 260, 36, 264);
  makeTextureFromBuffer(builtInTexture[63], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 266, 24, 270);
  makeTextureFromBuffer(builtInTexture[64], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 266, 30, 270);
  makeTextureFromBuffer(builtInTexture[65], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 266, 36, 270);
  makeTextureFromBuffer(builtInTexture[66], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 272, 24, 276);
  makeTextureFromBuffer(builtInTexture[67], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 272, 30, 276);
  makeTextureFromBuffer(builtInTexture[68], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 272, 36, 276);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabUnSelected[i] = builtInTexture[i + 60];
  
  //tab selected
  makeTextureFromBuffer(builtInTexture[69], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 260, 44, 264);
  makeTextureFromBuffer(builtInTexture[70], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 260, 50, 264);
  makeTextureFromBuffer(builtInTexture[71], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 260, 56, 264);
  makeTextureFromBuffer(builtInTexture[72], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 266, 44, 270);
  makeTextureFromBuffer(builtInTexture[73], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 266, 50, 270);
  makeTextureFromBuffer(builtInTexture[74], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 266, 56, 270);
  makeTextureFromBuffer(builtInTexture[75], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 272, 44, 276);
  makeTextureFromBuffer(builtInTexture[76], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 272, 50, 276);
  makeTextureFromBuffer(builtInTexture[77], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 272, 56, 276);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabSelected[i] = builtInTexture[i + 69];
  
  //small check box unchecked
  makeTextureFromBuffer(builtInTexture[78], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 200, 31, 211);
  builtInGuiSet.smallCheckBox[0] = builtInTexture[78];
  //small check box checked
  makeTextureFromBuffer(builtInTexture[79], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 200, 71, 211);
  builtInGuiSet.smallCheckBox[1] = builtInTexture[79];
  
  //horizontal small slider button
  makeTextureFromBuffer(builtInTexture[80], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 160, 48, 171);
  builtInGuiSet.smallSliderH = builtInTexture[80];
  
  //border panel
  makeTextureFromBuffer(builtInTexture[81], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 280, 44, 284);
  makeTextureFromBuffer(builtInTexture[82], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 280, 50, 284);
  makeTextureFromBuffer(builtInTexture[83], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 280, 56, 284);
  makeTextureFromBuffer(builtInTexture[84], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 286, 44, 290);
  makeTextureFromBuffer(builtInTexture[85], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 286, 50, 290);
  makeTextureFromBuffer(builtInTexture[86], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 286, 56, 290);
  makeTextureFromBuffer(builtInTexture[87], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 292, 44, 296);
  makeTextureFromBuffer(builtInTexture[88], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 292, 50, 296);
  makeTextureFromBuffer(builtInTexture[89], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 292, 56, 296);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.borderPanelTextures[i] = builtInTexture[i + 81];

  //fgbgcolor arrows
  makeTextureFromBuffer(builtInTexture[99], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 280, 36, 296);
  builtInGuiSet.fgBgColorArrows = builtInTexture[99];

  //submenu indicator arrow
  makeTextureFromBuffer(builtInTexture[100], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 320, 76, 336);
  builtInGuiSet.subMenuIndicator = builtInTexture[100];

  //invisible button panel
  makeTextureFromBuffer(builtInTexture[118], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 280, 44, 284);
  makeTextureFromBuffer(builtInTexture[119], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 280, 50, 284);
  makeTextureFromBuffer(builtInTexture[120], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 280, 56, 284);
  makeTextureFromBuffer(builtInTexture[121], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 286, 44, 290);
  makeTextureFromBuffer(builtInTexture[122], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 286, 50, 290);
  makeTextureFromBuffer(builtInTexture[123], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 286, 56, 290);
  makeTextureFromBuffer(builtInTexture[124], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 292, 44, 296);
  makeTextureFromBuffer(builtInTexture[125], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 292, 50, 296);
  makeTextureFromBuffer(builtInTexture[126], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 292, 56, 296);  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonTextures[i] = builtInTexture[i + 118];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonOverTextures[i] = builtInTexture[i + 118];
  
  //invisible button panel pressed
  makeTextureFromBuffer(builtInTexture[127], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 280, 64, 284);
  makeTextureFromBuffer(builtInTexture[128], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 280, 70, 284);
  makeTextureFromBuffer(builtInTexture[129], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 280, 76, 284);
  makeTextureFromBuffer(builtInTexture[130], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 286, 64, 290);
  makeTextureFromBuffer(builtInTexture[131], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 286, 70, 290);
  makeTextureFromBuffer(builtInTexture[132], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 286, 76, 290);
  makeTextureFromBuffer(builtInTexture[133], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 292, 64, 296);
  makeTextureFromBuffer(builtInTexture[134], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 292, 70, 296);
  makeTextureFromBuffer(builtInTexture[135], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 292, 76, 296);  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonTextures[i] = builtInTexture[i + 118];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonDownTextures[i] = builtInTexture[i + 127];
  

  //hor menu panel
  makeTextureFromBuffer(builtInTexture[136], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 300, 24, 304);
  makeTextureFromBuffer(builtInTexture[137], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 300, 30, 304);
  makeTextureFromBuffer(builtInTexture[138], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 300, 36, 304);
  makeTextureFromBuffer(builtInTexture[139], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 306, 24, 310);
  makeTextureFromBuffer(builtInTexture[140], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 306, 30, 310);
  makeTextureFromBuffer(builtInTexture[141], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 306, 36, 310);
  makeTextureFromBuffer(builtInTexture[142], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 312, 24, 316);
  makeTextureFromBuffer(builtInTexture[143], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 312, 30, 316);
  makeTextureFromBuffer(builtInTexture[144], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 312, 36, 316);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.horMenuPanelTextures[i] = builtInTexture[i + 136];
  
  //ver menu panel
  makeTextureFromBuffer(builtInTexture[145], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 320, 24, 324);
  makeTextureFromBuffer(builtInTexture[146], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 320, 30, 324);
  makeTextureFromBuffer(builtInTexture[147], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 320, 36, 324);
  makeTextureFromBuffer(builtInTexture[148], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 326, 24, 330);
  makeTextureFromBuffer(builtInTexture[149], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 326, 30, 330);
  makeTextureFromBuffer(builtInTexture[150], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 326, 36, 330);
  makeTextureFromBuffer(builtInTexture[151], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 332, 24, 336);
  makeTextureFromBuffer(builtInTexture[152], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 332, 30, 336);
  makeTextureFromBuffer(builtInTexture[153], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 332, 36, 336);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.verMenuPanelTextures[i] = builtInTexture[i + 145];
  
  //small slider horizontal line
  makeTextureFromBuffer(builtInTexture[154], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 160, 24, 164);
  makeTextureFromBuffer(builtInTexture[155], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 160, 30, 164);
  makeTextureFromBuffer(builtInTexture[156], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 160, 36, 164);
  
  //ver menu seperator slider vertical line
  makeTextureFromBuffer(builtInTexture[157], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 320, 44, 324);
  makeTextureFromBuffer(builtInTexture[158], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 320, 50, 324);
  makeTextureFromBuffer(builtInTexture[159], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 320, 56, 324);

  
  //panels
  builtInPanel[0].makeTextured(&builtInGuiSet.windowTextures[0]/*, RGB_White*/);
  builtInGuiSet.windowPanel = &builtInPanel[0];
  builtInPanel[1].makeTextured(&builtInGuiSet.buttonTextures[0]/*, RGB_White*/);
  builtInGuiSet.buttonPanel = &builtInPanel[1];
  builtInPanel[2].makeTextured(&builtInGuiSet.buttonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonOverPanel = &builtInPanel[2];
  builtInPanel[3].makeTextured(&builtInGuiSet.buttonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonDownPanel = &builtInPanel[3];
  builtInPanel[4].makeTextured(&builtInGuiSet.tabUnSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabUnSelectedPanel = &builtInPanel[4];
  builtInPanel[5].makeTextured(&builtInGuiSet.tabSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabSelectedPanel = &builtInPanel[5];
  builtInPanel[6].makeTextured(&builtInGuiSet.borderPanelTextures[0]/*, RGB_White*/);
  builtInGuiSet.borderPanel = &builtInPanel[6];
  //7, 8, 9, 10??? who cares this number system will be refactored away soon anyway
  builtInPanel[11].makeTextured(&builtInGuiSet.invisibleButtonTextures[0]/*, RGB_White*/);
  builtInGuiSet.invisibleButtonPanel = &builtInPanel[11];
  builtInPanel[12].makeTextured(&builtInGuiSet.invisibleButtonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonOverPanel = &builtInPanel[12];
  builtInPanel[13].makeTextured(&builtInGuiSet.invisibleButtonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonDownPanel = &builtInPanel[13];
  builtInPanel[14].makeTextured(&builtInGuiSet.horMenuPanelTextures[0]/*, RGB_White*/);
  builtInGuiSet.horMenuPanel = &builtInPanel[14];
  builtInPanel[15].makeTextured(&builtInGuiSet.verMenuPanelTextures[0]/*, RGB_White*/);
  builtInGuiSet.verMenuPanel = &builtInPanel[15];

  //rules (= 1D versions of panels)
  builtInRule[0].makeHorizontal(builtInTexture[41], builtInTexture[42], builtInTexture[43]);
  builtInGuiSet.sliderHRule = &builtInRule[0];
  builtInRule[1].makeVertical(builtInTexture[44], builtInTexture[45], builtInTexture[46]);
  builtInGuiSet.sliderVRule = &builtInRule[1];
  builtInRule[2].makeHorizontal(builtInTexture[47], builtInTexture[48], builtInTexture[49]);
  builtInGuiSet.windowTopRule = &builtInRule[2];
  builtInRule[3].makeHorizontal(builtInTexture[154], builtInTexture[155], builtInTexture[156]);
  builtInGuiSet.smallSliderHRule = &builtInRule[3];
  builtInRule[4].makeHorizontal(builtInTexture[157], builtInTexture[158], builtInTexture[159]);
  builtInGuiSet.verMenuSeparator = &builtInRule[4];
  
  //colors
  builtInGuiSet.mainColor = RGB_White;
  builtInGuiSet.mouseOverColor = RGB_Brightred;
  builtInGuiSet.mouseDownColor = RGB_Grey;
  builtInGuiSet.windowTopColor = RGB_Lightblue;
  builtInGuiSet.barColor = RGB_Grey;
  
  //font
  builtInGuiSet.defaultFont.color = RGB_Black;
  builtInGuiSet.messageFont.color = RGB_Black;
  builtInGuiSet.menuFont[0].color = RGB_Black;
  builtInGuiSet.menuFont[1].color = RGB_White;
  builtInGuiSet.panelButtonFont[0].color = RGB_Black;
  builtInGuiSet.panelButtonFont[1].color = RGB_Black;
  builtInGuiSet.panelButtonFont[2].color = RGB_Black;
  builtInGuiSet.textButtonFont[0].color = RGB_White;
  builtInGuiSet.textButtonFont[0].shadow = true;
  builtInGuiSet.textButtonFont[0].shadowColor = RGB_Black;
  builtInGuiSet.textButtonFont[1].color = RGB_Red;
  builtInGuiSet.textButtonFont[1].shadow = true;
  builtInGuiSet.textButtonFont[1].shadowColor = RGB_Black;
  builtInGuiSet.textButtonFont[2].color = RGB_Red;
  builtInGuiSet.textButtonFont[2].shadow = true;
  builtInGuiSet.textButtonFont[2].shadowColor = RGB_Black;
  builtInGuiSet.windowTopFont.color = RGB_White;
  builtInGuiSet.windowTopFont.shadow = true;
  builtInGuiSet.windowTopFont.shadowColor = RGB_Black;
  builtInGuiSet.tooltipFont = Font("lpi6", RGB_Black);
  
  //menu panels and separators
  builtInGuiSet.horMenuSeparator = builtInGuiSet.sliderVRule;
}

void GUIPartDrawerInternal::setDefaultTypeFace(const std::string& typeface)
{
  builtInGuiSet.panelButtonFont[0].typeface = typeface;
  builtInGuiSet.panelButtonFont[1].typeface = typeface;
  builtInGuiSet.panelButtonFont[2].typeface = typeface;
  builtInGuiSet.textButtonFont[0].typeface = typeface;
  builtInGuiSet.textButtonFont[1].typeface = typeface;
  builtInGuiSet.textButtonFont[2].typeface = typeface;
  builtInGuiSet.windowTopFont.typeface = typeface;
}

void GUIPartDrawerInternal::initBuiltInIcons64(const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInIcons(decoded64);
}

void GUIPartDrawerInternal::initBuiltInIcons(const std::vector<unsigned char>& png)
{
  static const ColorRGB RGB_Magenta      (255,   0, 255, 255);
  static const AlphaEffect AE_PinkKey(128, 255, RGB_Magenta);
  
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> dataBuffer;
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data
  
  makeTextureFromBuffer(&icons.iconFile, &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 20, 56, 36);
  makeTextureFromBuffer(&icons.iconFolder, &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 20, 36, 36);
  makeTextureFromBuffer(&icons.iconNew, &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 60, 36, 76);
  makeTextureFromBuffer(&icons.iconOpen, &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 60, 56, 76);
  makeTextureFromBuffer(&icons.iconSave, &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 60, 76, 76);
  makeTextureFromBuffer(&icons.iconSaveAs, &dataBuffer[0], GDW, GDH, AE_PinkKey, 80, 60, 96, 76);
  makeTextureFromBuffer(&icons.iconSaveAll, &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 60, 116, 76);
  makeTextureFromBuffer(&icons.iconClose, &dataBuffer[0], GDW, GDH, AE_PinkKey, 120, 60, 136, 76);
  makeTextureFromBuffer(&icons.iconCut, &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 80, 36, 96);
  makeTextureFromBuffer(&icons.iconCopy, &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 80, 56, 96);
  makeTextureFromBuffer(&icons.iconPaste, &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 80, 76, 96);
  makeTextureFromBuffer(&icons.iconUndo, &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 100, 36, 116);
  makeTextureFromBuffer(&icons.iconRedo, &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 100, 56, 116);
}

////////////////////////////////////////////////////////////////////////////////
//BACKPANEL/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool panelGradient = true; //TODO: make this a separate setting per panel

/*
The BackPanel functions.
BackPanel is a class containing 9 textures, being able to draw textured panels of any size with those.
See the .h file for meaning of vareous variables of the classes
*/

//Constructors
BackPanel::BackPanel()
: enableSides(false)
, enableCenter(false)
//, gradient(true)
{
  t00 = getDefaultEmptyTexture();
  t01 = getDefaultEmptyTexture();
  t02 = getDefaultEmptyTexture();
  t10 = getDefaultEmptyTexture();
  t11 = getDefaultEmptyTexture();
  t12 = getDefaultEmptyTexture();
  t20 = getDefaultEmptyTexture();
  t21 = getDefaultEmptyTexture();
  t22 = getDefaultEmptyTexture();
}


void BackPanel::makeUntextured()
{
  enableSides = false;
  enableCenter = false;
  t00 = getDefaultEmptyTexture();
  t01 = getDefaultEmptyTexture();
  t02 = getDefaultEmptyTexture();
  t10 = getDefaultEmptyTexture();
  t11 = getDefaultEmptyTexture();
  t12 = getDefaultEmptyTexture();
  t20 = getDefaultEmptyTexture();
  t21 = getDefaultEmptyTexture();
  t22 = getDefaultEmptyTexture();
}

void BackPanel::makeTextured9(const ITexture* t00, const ITexture* t01, const ITexture* t02, const ITexture* t10, const ITexture* t11, const ITexture* t12, const ITexture* t20, const ITexture* t21, const ITexture* t22)
{
  enableSides = true;
  enableCenter = true;
  this->t00 = t00;
  this->t01 = t01;
  this->t02 = t02;
  this->t10 = t10;
  this->t11 = t11;
  this->t12 = t12;
  this->t20 = t20;
  this->t21 = t21;
  this->t22 = t22;
}

void BackPanel::makeTextured(const ITexture** t00)
{
  this->enableSides = 1;
  this->enableCenter = 1;
  this->t00 = t00[0];
  this->t01 = t00[1];
  this->t02 = t00[2];
  this->t10 = t00[3];
  this->t11 = t00[4];
  this->t12 = t00[5];
  this->t20 = t00[6];
  this->t21 = t00[7];
  this->t22 = t00[8];
}

/*The draw function of the BackPanel: draws the 9 different textures tiled or
non-tiled at the positions needed to get a window at the correct position
with the correct size*/
void BackPanel::draw(IDrawer2D& drawer, int x, int y, int width, int height, const ColorRGB& colorMod) const
{
  if(/*gradient*/panelGradient)
  {
#define G 232
    static ColorRGB c00 = RGB_White;
    static ColorRGB c01 = ColorRGB(G,G,G);
    static ColorRGB c10 = RGB_White;
    static ColorRGB c11 = ColorRGB(G,G,G);
    const ColorRGB& c = colorMod;
    
    //draw rectangle  
    if(!enableCenter)
    {
      drawer.drawGradientRectangle(x, y, x + width, y + height, c*c00, c*c10, c*c11, c*c10);
    }
    //if center texture enabled, draw center texture instead, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    else
    {
      if(t11 != 0) drawer.drawTextureRepeatedGradient(t11, x + t00->getU(), y + t00->getV(), x + width - t02->getU(), y + height - t20->getV(), c*c00, c*c01, c*c10, c*c11);
    }
    
    //draw sides
    if(enableSides)
    {
      drawer.drawTextureRepeatedGradient(t01, x + t00->getU(),         y                       , x + width - t02->getU(), y + t00->getV()         , c*c00, c*c00, c*c10, c*c10);
      drawer.drawTextureRepeatedGradient(t21, x + t00->getU(),         y + height - t20->getV(), x + width - t02->getU(), y + height              , c*c01, c*c01, c*c11, c*c11);
      drawer.drawTextureRepeatedGradient(t10, x,                       y + t00->getV()         , x + t00->getU()        , y + height - t20->getV(), c*c00, c*c01, c*c00, c*c01);
      drawer.drawTextureRepeatedGradient(t12, x + width - t02->getU(), y + t00->getV()         , x + width              , y + height - t20->getV(), c*c10, c*c11, c*c10, c*c11);
    }
      
    //draw corners
    if(enableSides)
    {
      drawer.drawTexture(t00, x                      , y                       , c*c00); //top left 
      drawer.drawTexture(t02, x + width - t02->getU(), y                       , c*c10); //top right
      drawer.drawTexture(t20, x                      , y + height - t20->getV(), c*c01); //bottom left
      drawer.drawTexture(t22, x + width - t02->getU(), y + height - t20->getV(), c*c11); //bottom right
    }
  }
  else
  {
    //draw rectangle  
    if(!enableCenter)
    {
      drawer.drawRectangle(x, y, x + width, y + height, colorMod, true);
    }
    //if center texture enabled, draw center texture instead, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    else
    {
      if(t11 != 0) drawer.drawTextureRepeated(t11, x + t00->getU(), y + t00->getV(), x + width - t02->getU(), y + height - t20->getV(), colorMod);
    }
    
    //draw sides
    if(enableSides)
    {
      drawer.drawTextureRepeated(t01, x + t00->getU(),         y                       , x + width - t02->getU(), y + t00->getV()         , colorMod);
      drawer.drawTextureRepeated(t21, x + t00->getU(),         y + height - t20->getV(), x + width - t02->getU(), y + height              , colorMod);
      drawer.drawTextureRepeated(t10, x,                       y + t00->getV()         , x + t00->getU()        , y + height - t20->getV(), colorMod);
      drawer.drawTextureRepeated(t12, x + width - t02->getU(), y + t00->getV()         , x + width              , y + height - t20->getV(), colorMod);
    }
      
    //draw corners
    if(enableSides)
    {
      drawer.drawTexture(t00, x                      , y                       , colorMod); //top left 
      drawer.drawTexture(t02, x + width - t02->getU(), y                       , colorMod); //top right
      drawer.drawTexture(t20, x                      , y + height - t20->getV(), colorMod); //bottom left
      drawer.drawTexture(t22, x + width - t02->getU(), y + height - t20->getV(), colorMod); //bottom right
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//BACKRULE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
a BackRule is a collection of 3 textures (2 corners and 1 tileable center) that
can form a horizontal or vertical line
*/

//Constructors
BackRule::BackRule()
{
  this->enableSides = 0;
  this->t0 = getDefaultEmptyTexture();
  this->t1 = getDefaultEmptyTexture();
  this->t2 = getDefaultEmptyTexture();
}


void BackRule::makeHorizontal(const ITexture* t0, ITexture* t1, ITexture* t2)
{
  this->direction = H;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeHorizontal1(const ITexture* t0)
{
  this->direction = H;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::makeVertical(const ITexture* t0, ITexture* t1, ITexture* t2)
{
  this->direction = V;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeVertical1(const ITexture* t0)
{
  this->direction = V;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::draw(IDrawer2D& drawer, int x, int y, int length, const ColorRGB& colorMod) const
{
  if(direction == H)
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    drawer.drawTextureRepeated(t1, x + t0->getU(), y, x + length - t2->getU(), y + t0->getV(), colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      drawer.drawTexture(t0, x, y, colorMod); //left
      drawer.drawTexture(t2, x + length - t2->getU(), y, colorMod); //right
    }
  }
  else
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    drawer.drawTextureRepeated(t1, x, y + t0->getV(), x + t0->getU(), y + length - t2->getV(), colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      drawer.drawTexture(t0, x, y, colorMod); //left
      drawer.drawTexture(t2, x, y + length - t2->getV(), colorMod); //right
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

GUIPartDrawerInternal::GUIPartDrawerInternal(const ITextureFactory& factory, IDrawer2D* drawer, ITextDrawer* textdrawer)
: drawer(drawer)
, textdrawer(textdrawer)
{
  builtInTexture.resize(180);
  for(size_t i = 0; i < builtInTexture.size(); i++) builtInTexture[i] = factory.createNewTexture();
  initBuiltInGui(factory);
  initBuiltInIcons();
  guiset = &builtInGuiSet;
}

GUIPartDrawerInternal::~GUIPartDrawerInternal()
{
  for(size_t i = 0; i < builtInTexture.size(); i++) delete builtInTexture[i];
}


void GUIPartDrawerInternal::drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GP_WINDOW_PANEL:
    {
      guiset->windowPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_WINDOW_TOP:
    {
      guiset->windowTopRule->draw(*drawer, x0, y0, x1 - x0, guiset->windowTopColor);
      break;
    }
    case GP_WINDOW_CLOSE:
    {
      ColorRGB color = mod.mouseover ? guiset->mouseOverColor : RGB_White;
      drawer->drawTexture(guiset->closeButton, x1 - guiset->closeButton->getU(), y0, color);
      break;
    }
    case GP_WINDOW_RESIZER:
    {
      drawer->drawTexture(guiset->resizer, x1 - guiset->resizer->getU(), y1 - guiset->resizer->getV());
      break;
    }
    case GP_BUTTON_PANEL:
    {
      if(mod.mousedown) guiset->buttonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else if(mod.mouseover) guiset->buttonOverPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else guiset->buttonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_BUTTON_PANEL_UP:
    {
      guiset->buttonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_BUTTON_PANEL_DOWN:
    {
      guiset->buttonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL:
    {
      if(mod.mousedown) guiset->invisibleButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else if(mod.mouseover) guiset->invisibleButtonOverPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else guiset->invisibleButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL_UP:
    {
      guiset->invisibleButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL_DOWN:
    {
      guiset->invisibleButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_PANEL_BORDER:
    {
      guiset->borderPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_TAB_UNSELECTED:
    {
      guiset->tabUnSelectedPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_TAB_SELECTED:
    {
      guiset->tabSelectedPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_CHECKBOX_OFF:
    {
      if(y1 - y0 >= 16) drawer->drawTexture(guiset->checkBox[0], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[0], x0, y0);
      break;
    }
    case GP_CHECKBOX_ON:
    {
      if(y1 - y0 >= 16) drawer->drawTexture(guiset->checkBox[1], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[1], x0, y0);
      break;
    }
    case GP_BULLET_OFF:
    {
      drawer->drawTexture(guiset->bullet[0], x0, y0);
      break;
    }
    case GP_BULLET_ON:
    {
      drawer->drawTexture(guiset->bullet[1], x0, y0);
      break;
    }
    case GP_SLIDER_HBUTTON:
    {
      drawer->drawTexture(guiset->slider, x0, y0);
      break;
    }
    case GP_SLIDER_VBUTTON:
    {
      drawer->drawTexture(guiset->slider, x0, y0);
      break;
    }
    case GP_SLIDER_HBACK:
    {
      int rulerCenter = guiset->sliderHRule->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->sliderHRule->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_SLIDER_VBACK:
    {
      int rulerCenter = guiset->sliderVRule->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->sliderVRule->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }
    case GP_SMALL_SLIDER_HBUTTON:
    {
      drawer->drawTexture(guiset->smallSliderH, x0, y0);
      break;
    }
    case GP_SMALL_SLIDER_VBUTTON:
    {
      //TODO: graphic image doesn't exist yet.
      break;
    }
 
    case GP_SMALL_SLIDER_HBACK:
    {
      int rulerCenter = guiset->smallSliderHRule->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->smallSliderHRule->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_SMALL_SLIDER_VBACK:
    {
      int rulerCenter = guiset->sliderVRule->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->sliderVRule->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }
    case GP_SCROLLBAR_N:
    {
      drawer->drawTexture(guiset->arrowN, x0, y0);
      break;
    }
    case GP_SCROLLBAR_E:
    {
      drawer->drawTexture(guiset->arrowE, x0, y0);
      break;
    }
    case GP_SCROLLBAR_S:
    {
      drawer->drawTexture(guiset->arrowS, x0, y0);
      break;
    }
    case GP_SCROLLBAR_W:
    {
      drawer->drawTexture(guiset->arrowW, x0, y0);
      break;
    }
    case GP_SCROLLBAR_SCROLLER:
    {
      drawer->drawTexture(guiset->scroller, x0, y0);
      break;
    }
    case GP_SCROLLBAR_SCROLLER_OUT_OF_RANGE:
    {
      drawer->drawTexture(guiset->scroller, x0, y0, RGB_Red);
      break;
    }
    case GP_SCROLLBAR_HBACK:
    {
      drawer->drawTextureRepeated(guiset->scrollbarBackground, x0, y0, x1, y1);
      break;
    }
    case GP_SCROLLBAR_VBACK:
    {
      drawer->drawTextureRepeated(guiset->scrollbarBackground, x0, y0, x1, y1);
      break;
    }
    case GP_SCROLLBARPAIR_CORNER:
    {
      const ITexture* t = guiset->scrollBarPairCorner;
      drawer->drawTexture(t, x1 - t->getU(), y1 - t->getV());
      break;
    }
    case GP_TOOLTIP_BACKGROUND:
    {
      drawer->drawRectangle(x0, y0, x1, y1, ColorRGB(255, 255, 128, 224), true);
      drawer->drawRectangle(x0, y0, x1, y1, RGB_Black, false);
      break;
    }
    case GP_FG_BG_COLOR_ARROWS:
    {
      drawer->drawTexture(guiset->fgBgColorArrows, x1 - guiset->fgBgColorArrows->getU(), y0);
      break;
    }
    case GP_HMENU_PANEL:
    {
      guiset->horMenuPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_VMENU_PANEL:
    {
      guiset->verMenuPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_VMENU_SEPARATOR:
    {
      int rulerCenter = guiset->verMenuSeparator->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->verMenuSeparator->draw(*drawer, x0 + 4, centerPos - rulerCenter, x1 - x0 - 8);
      break;
    }
    case GP_HMENU_SEPARATOR:
    case GP_TOOLBAR_SEPARATOR:
    {
      int rulerCenter = guiset->horMenuSeparator->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->horMenuSeparator->draw(*drawer, centerPos - rulerCenter, y0 + 2, y1 - y0 - 4);
      break;
    }
    case GP_SPINNER_UP:
    case GP_SPINNER_DOWN:
    case GP_DROPDOWN_BUTTON:
    {
      drawGUIPart(GP_BUTTON_PANEL, x0, y0, x1, y1, mod);
      int tx0 = (x0 + x1) / 2 - 4;
      int ty0 = (y0 + y1) / 2 - 2;
      int tx1 = (x0 + x1) / 2 + 4;
      int ty1 = (y0 + y1) / 2 + 2;
      if(part == GP_SPINNER_UP) drawer->drawTriangle((tx0+tx1)/2, ty0+1, tx0, ty1, tx1, ty1, RGB_Black, true);
      else drawer->drawTriangle((tx0+tx1)/2, ty1, tx1, ty0, tx0, ty0, RGB_Black, true);
      
      break;
    }
    case GP_TOOLBAR_BUTTON:
    {
      if(mod.mousedown) guiset->buttonDownPanel->draw(*drawer, x0+1, y0+1, x1 - x0 - 2, y1 - y0 - 2);
      else if(mod.mouseover) guiset->buttonOverPanel->draw(*drawer, x0+1, y0+1, x1 - x0 - 2, y1 - y0 - 2);
      //else drawer->drawRectangle(x0+1, y0+1, x1-1, y1-1, guiset->barColor, true);
      break;
    }
    case GP_STATUSBAR:
    {
      drawer->drawRectangle(x0, y0, x1, y1, guiset->barColor, true);
      drawer->drawRectangle(x0, y0, x1, y1, RGB_White, false);
      break;
    }
    case GP_STATUSBAR_SEPARATOR:
    {
      drawer->drawLine(x0, y0, x0, y1, RGB_White);
      break;
    }
    default:
      return;
  }
}

void GUIPartDrawerInternal::drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GPC_WINDOW_PANEL:
    {
      guiset->windowPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    default:
      return;
  }
}

void GUIPartDrawerInternal::drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GPT_TEXT_BUTTON_TEXT:
    {
      if(mod.mousedown) textdrawer->drawText(text, (x0+x1)/2 + 2, (y0+y1)/2 + 2, guiset->textButtonFont[2], TextAlign(HA_CENTER, VA_CENTER));
      else if(mod.mouseover) textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[1], TextAlign(HA_CENTER, VA_CENTER));
      else textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[0], TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_PANEL_BUTTON_TEXT:
    {
      if(mod.mousedown) textdrawer->drawText(text, (x0+x1)/2 + 1, (y0+y1)/2 + 1, guiset->panelButtonFont[2], TextAlign(HA_CENTER, VA_CENTER));
      else if(mod.mouseover) textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->panelButtonFont[1], TextAlign(HA_CENTER, VA_CENTER));
      else textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->panelButtonFont[0], TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_WINDOW_TITLE:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->windowTopFont, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_TOOLTIP:
    {
      x0 += 10; //shift a bit to right to avoid having mouse cursor in front of the tip
      int w, h;
      textdrawer->calcTextRectSize(w, h, text, guiset->tooltipFont);
      drawGUIPart(GP_TOOLTIP_BACKGROUND, x0 - 2, y0 - 2, x0 + w + 2, y0 + h + 2, mod);
      textdrawer->drawText(text, x0, y0, guiset->tooltipFont);
      break;
    }
    case GPT_DEFAULT_TEXT00:
    {
      textdrawer->drawText(text, x0, y0, guiset->defaultFont, TextAlign(HA_LEFT, VA_TOP));
      break;
    }
    case GPT_DEFAULT_TEXT01:
    {
      textdrawer->drawText(text, x0, (y0+y1)/2, guiset->defaultFont, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    case GPT_DEFAULT_TEXT02:
    {
      textdrawer->drawText(text, x0, y1, guiset->defaultFont, TextAlign(HA_LEFT, VA_BOTTOM));
      break;
    }
    case GPT_DEFAULT_TEXT10:
    {
      textdrawer->drawText(text, (x0+x1)/2, y0, guiset->defaultFont, TextAlign(HA_CENTER, VA_TOP));
      break;
    }
    case GPT_DEFAULT_TEXT11:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->defaultFont, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_DEFAULT_TEXT12:
    {
      textdrawer->drawText(text, (x0+x1)/2, y1, guiset->defaultFont, TextAlign(HA_CENTER, VA_BOTTOM));
      break;
    }
    case GPT_DEFAULT_TEXT20:
    {
      textdrawer->drawText(text, x1, y0, guiset->defaultFont, TextAlign(HA_RIGHT, VA_TOP));
      break;
    }
    case GPT_DEFAULT_TEXT21:
    {
      textdrawer->drawText(text, x1, (y0+y1)/2, guiset->defaultFont, TextAlign(HA_RIGHT, VA_CENTER));
      break;
    }
    case GPT_DEFAULT_TEXT22:
    {
      textdrawer->drawText(text, x1, y1, guiset->defaultFont, TextAlign(HA_RIGHT, VA_BOTTOM));
      break;
    }
    case GPT_MESSAGE_TEXT:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->messageFont, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_HMENU_TEXT:
    case GPT_HMENU_SUBMENUTEXT:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->menuFont[0], TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_VMENU_TEXT:
    {
      if(mod.mouseover)
      {
        drawer->drawRectangle(x0+2, y0+2, x1-2, y1-2, ColorRGB(0, 0, 128), true);
        textdrawer->drawText(text, x0+4, (y0+y1)/2, guiset->menuFont[1], TextAlign(HA_LEFT, VA_CENTER));
      }
      else textdrawer->drawText(text, x0+4, (y0+y1)/2, guiset->menuFont[0], TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    case GPT_VMENU_SUBMENUTEXT:
    {
      const ITexture* t = guiset->subMenuIndicator;
      if(mod.mouseover)
      {
        drawer->drawRectangle(x0+2, y0+2, x1-2, y1-2, ColorRGB(0, 0, 128), true);
        textdrawer->drawText(text, x0+4, (y0+y1)/2, guiset->menuFont[1], TextAlign(HA_LEFT, VA_CENTER));
        drawer->drawTexture(t, x1 - t->getU(), ((y0 + y1) / 2) - t->getV() / 2);
      }
      else
      {
        textdrawer->drawText(text, x0+4, (y0+y1)/2, guiset->menuFont[0], TextAlign(HA_LEFT, VA_CENTER));
        drawer->drawTexture(t, x1 - t->getU(), ((y0 + y1) / 2) - t->getV() / 2, RGB_Black);
      }
      
      
      break;
    }
    case GPT_STATUSBAR_TEXT:
    {
      x0 += 2;
      textdrawer->drawText(text, x0, (y0 + y1) / 2, guiset->defaultFont, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    case GPT_TEXTINPUTLINE:
    {
      x0 += 2;
      textdrawer->drawText(text, x0, (y0 + y1) / 2, guiset->textInputLineFont, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    case GPT_TEXTINPUTLINE_TITLE:
    {
      x0 += 2;
      textdrawer->drawText(text, x0, (y0 + y1) / 2, guiset->textInputLineTitleFont, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    
    default:
      return;
  }
}

void GUIPartDrawerInternal::getGUIPartTextSize(int& w, int& h, GUIPart part, const std::string& text) const
{
  switch(part)
  {
    case GPT_TEXT_BUTTON_TEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->textButtonFont[0]);
      break;
    }
    case GPT_PANEL_BUTTON_TEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->panelButtonFont[0]);
      break;
    }
    case GPT_WINDOW_TITLE:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->windowTopFont);
      break;
    }
    case GPT_TOOLTIP:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->tooltipFont);
      break;
    }
    case GPT_DEFAULT_TEXT00:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT01:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT02:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT10:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT11:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT12:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT20:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT21:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_DEFAULT_TEXT22:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_MESSAGE_TEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->messageFont);
      break;
    }
    case GPT_HMENU_TEXT:
    case GPT_HMENU_SUBMENUTEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->menuFont[0]);
      break;
    }
    case GPT_VMENU_TEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->menuFont[0]);
      break;
    }
    case GPT_VMENU_SUBMENUTEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->menuFont[0]);
      break;
    }
    case GPT_STATUSBAR_TEXT:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->defaultFont);
      break;
    }
    case GPT_TEXTINPUTLINE:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->textInputLineFont);
      break;
    }
    case GPT_TEXTINPUTLINE_TITLE:
    {
      textdrawer->calcTextRectSize(w, h, text, guiset->textInputLineTitleFont);
      break;
    }
    default: w = h = 0; break;
  }
}

size_t GUIPartDrawerInternal::getGUIPartSizeX(GUIPart part) const
{
  switch(part)
  {
    case GP_WINDOW_CLOSE: return guiset->closeButton->getU();
    case GP_WINDOW_RESIZER: return guiset->resizer->getU();
    case GP_SLIDER_HBUTTON: return guiset->slider->getU();
    case GP_SLIDER_VBUTTON: return guiset->slider->getU();
    case GP_SMALL_SLIDER_HBUTTON: return guiset->smallSliderH->getU();
    //case GP_SMALL_SLIDER_VBUTTON: return guiset->smallSliderV->getU();
    case GP_SLIDER_HBACK: return guiset->sliderHRule->t0->getU();
    case GP_SLIDER_VBACK: return guiset->sliderVRule->t0->getU();
    case GP_SMALL_SLIDER_HBACK: return guiset->smallSliderHRule->t0->getU();
    case GP_SMALL_SLIDER_VBACK: return guiset->sliderVRule->t0->getU();
    case GP_SCROLLBAR_N: return guiset->arrowN->getU();
    case GP_SCROLLBAR_E: return guiset->arrowE->getU();
    case GP_SCROLLBAR_S: return guiset->arrowS->getU();
    case GP_SCROLLBAR_W: return guiset->arrowW->getU();
    case GP_SCROLLBAR_SCROLLER: return guiset->scroller->getU();
    case GP_SCROLLBAR_HBACK: return guiset->scrollbarBackground->getU();
    case GP_SCROLLBAR_VBACK: return guiset->scrollbarBackground->getU();
    case GP_SCROLLBARPAIR_CORNER: return guiset->scrollBarPairCorner->getU();
    case GP_FG_BG_COLOR_ARROWS: return guiset->fgBgColorArrows->getU();
    default: return 0;
  }
}

size_t GUIPartDrawerInternal::getGUIPartSizeY(GUIPart part) const
{
  switch(part)
  {
    case GP_WINDOW_TOP: return guiset->windowTopRule->t0->getV();
    case GP_WINDOW_CLOSE: return guiset->closeButton->getV();
    case GP_WINDOW_RESIZER: return guiset->resizer->getV();
    case GP_SLIDER_HBUTTON: return guiset->slider->getV();
    case GP_SLIDER_VBUTTON: return guiset->slider->getV();
    case GP_SMALL_SLIDER_HBUTTON: return guiset->smallSliderH->getV();
    //case GP_SMALL_SLIDER_VBUTTON: return guiset->smallSliderV->getV();
    case GP_SLIDER_HBACK: return guiset->sliderHRule->t0->getV();
    case GP_SLIDER_VBACK: return guiset->sliderVRule->t0->getV();
    case GP_SMALL_SLIDER_HBACK: return guiset->smallSliderHRule->t0->getV();
    case GP_SMALL_SLIDER_VBACK: return guiset->sliderVRule->t0->getV();
    case GP_SCROLLBAR_N: return guiset->arrowN->getV();
    case GP_SCROLLBAR_E: return guiset->arrowE->getV();
    case GP_SCROLLBAR_S: return guiset->arrowS->getV();
    case GP_SCROLLBAR_W: return guiset->arrowW->getV();
    case GP_SCROLLBAR_SCROLLER: return guiset->scroller->getV();
    case GP_SCROLLBAR_HBACK: return guiset->scrollbarBackground->getV();
    case GP_SCROLLBAR_VBACK: return guiset->scrollbarBackground->getV();
    case GP_SCROLLBARPAIR_CORNER: return guiset->scrollBarPairCorner->getV();
    case GP_FG_BG_COLOR_ARROWS: return guiset->fgBgColorArrows->getV();
    default: return 0;
  }
}

void GUIPartDrawerInternal::createIcon(ITexture& texture, GUIIcon icon, int size) const
{
  (void)size; //only 16x16 supported for now
  
  const TextureBuffer* source = 0;
  switch(icon)
  {
    case GI_FILE: source = &icons.iconFile; break;
    case GI_FOLDER: source = &icons.iconFolder; break;
    case GI_NEW: source = &icons.iconNew; break;
    case GI_OPEN: source = &icons.iconOpen; break;
    case GI_SAVE: source = &icons.iconSave; break;
    case GI_SAVE_AS: source = &icons.iconSaveAs; break;
    case GI_SAVE_ALL: source = &icons.iconSaveAll; break;
    case GI_CLOSE: source = &icons.iconClose; break;
    case GI_CUT: source = &icons.iconCut; break;
    case GI_COPY: source = &icons.iconCopy; break;
    case GI_PASTE: source = &icons.iconPaste; break;
    case GI_UNDO: source = &icons.iconUndo; break;
    case GI_REDO: source = &icons.iconRedo; break;
    default: source = 0;
  }

  if(source != 0)
  {
    size_t u = source->getU();
    size_t v = source->getV();

    texture.setSize(u, v);
    for(size_t y = 0; y < v; y++)
    for(size_t x = 0; x < u; x++)
    for(size_t c = 0; c < 4; c++)
    {
      texture.getBuffer()[u*y*4+x*4+c] = source->getBuffer()[u*y*4+x*4+c];
      texture.update();
    }
  }
}

} // namespace gui
} // namespace lpi
