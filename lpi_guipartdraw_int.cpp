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

////////////////////////////////////////////////////////////////////////////////
//TEXTURESET////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
PNG images, encoded in base64, containing all the GUI images
First use the function decodeBase64
Then use the function LodePNG::Decoder::decode
And you get the pixels in a buffer
*/
const std::string builtInGuiData = "\
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\n\
B2KkKmkPBP//AAAAAElFTkSuQmCC\n\
";

void GUIPartDrawerInternal::initBuiltInGui(const ITextureFactory& factory)
{
  initBuiltInGuiTexturesSmall64(factory, builtInGuiData);
}


void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiTexturesSmall(factory, png);
}


void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall64(const ITextureFactory& factory, const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiTexturesSmall(factory, decoded64);
}

void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::vector<unsigned char>& png)
{
  (void)factory;
  
  LodePNG::Decoder pngdec;

  
  std::vector<unsigned char> dataBuffer;
  
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data

  /*
    Below the built in gui textures are loaded from the buffer.
    Coordinates gotten by looking at the texture that's encoded here, in a painting program.
  */
  
  //panel
  makeTextureFromBuffer(builtInTexture[ 0], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  0,  4,  4);
  makeTextureFromBuffer(builtInTexture[ 1], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  0, 10,  4);
  makeTextureFromBuffer(builtInTexture[ 2], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  0, 16,  4);
  makeTextureFromBuffer(builtInTexture[ 3], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  6,  4, 10);
  makeTextureFromBuffer(builtInTexture[ 4], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  6, 10, 10);
  makeTextureFromBuffer(builtInTexture[ 5], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  6, 16, 10);
  makeTextureFromBuffer(builtInTexture[ 6], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 12,  4, 16);
  makeTextureFromBuffer(builtInTexture[ 7], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 12, 10, 16);
  makeTextureFromBuffer(builtInTexture[ 8], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 12, 16, 16);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.windowTextures[i] = builtInTexture[i];
  
  //button normal
  makeTextureFromBuffer(builtInTexture[ 9], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 32,  4, 36);
  makeTextureFromBuffer(builtInTexture[10], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 32, 10, 36);
  makeTextureFromBuffer(builtInTexture[11], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 32, 16, 36);
  makeTextureFromBuffer(builtInTexture[12], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 38,  4, 42);
  makeTextureFromBuffer(builtInTexture[13], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 38, 10, 42);
  makeTextureFromBuffer(builtInTexture[14], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 38, 16, 42);
  makeTextureFromBuffer(builtInTexture[15], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 44,  4, 48);
  makeTextureFromBuffer(builtInTexture[16], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 44, 10, 48);
  makeTextureFromBuffer(builtInTexture[17], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 44, 16, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonTextures[i] = builtInTexture[i + 9];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonOverTextures[i] = builtInTexture[i + 9];
  
  //button pressed
  makeTextureFromBuffer(builtInTexture[18], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 32, 20, 36);
  makeTextureFromBuffer(builtInTexture[19], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 32, 26, 36);
  makeTextureFromBuffer(builtInTexture[20], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 32, 32, 36);
  makeTextureFromBuffer(builtInTexture[21], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 38, 20, 42);
  makeTextureFromBuffer(builtInTexture[22], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 38, 26, 42);
  makeTextureFromBuffer(builtInTexture[23], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 38, 32, 42);
  makeTextureFromBuffer(builtInTexture[24], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 44, 20, 48);
  makeTextureFromBuffer(builtInTexture[25], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 44, 26, 48);
  makeTextureFromBuffer(builtInTexture[26], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 44, 32, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonDownTextures[i] = builtInTexture[i + 18];
  
  //button arrow up
  makeTextureFromBuffer(builtInTexture[27], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 64, 16, 80);
  builtInGuiSet.arrowN = builtInTexture[27];
  //button arrow down
  makeTextureFromBuffer(builtInTexture[28], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 64, 48, 80);
  builtInGuiSet.arrowS = builtInTexture[28];
  //button arrow left
  makeTextureFromBuffer(builtInTexture[29], &dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 64, 80, 80);
  builtInGuiSet.arrowW = builtInTexture[29];
  //button arrow right
  makeTextureFromBuffer(builtInTexture[30], &dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 64, 112, 80);
  builtInGuiSet.arrowE = builtInTexture[30];
  
  //scroller
  makeTextureFromBuffer(builtInTexture[31], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 48, 48, 64);
  builtInGuiSet.emptyButton = builtInTexture[31];
  builtInGuiSet.scroller = builtInTexture[31];
  //scrollbar background
  makeTextureFromBuffer(builtInTexture[32], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 48, 16, 64);
  builtInGuiSet.scrollbarBackground = builtInTexture[32];
  builtInGuiSet.scrollBarPairCorner = builtInTexture[32];
  
  //check box unchecked
  makeTextureFromBuffer(builtInTexture[33], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 80, 16, 96);
  builtInGuiSet.checkBox[0] = builtInTexture[33];
  //check box checked
  makeTextureFromBuffer(builtInTexture[34], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 80, 48, 96);
  builtInGuiSet.checkBox[1] = builtInTexture[34];
  
  //bullet unchecked
  makeTextureFromBuffer(builtInTexture[35], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 96, 16, 112);
  builtInGuiSet.bullet[0] = builtInTexture[35];
  //bullet checked
  makeTextureFromBuffer(builtInTexture[36], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 96, 48, 112);
  builtInGuiSet.bullet[1] = builtInTexture[36];
  
  //horizontal line
  makeTextureFromBuffer(builtInTexture[41], &dataBuffer[0], GDW, GDH, AE_PinkKey, 116, 16, 120, 20);
  makeTextureFromBuffer(builtInTexture[42], &dataBuffer[0], GDW, GDH, AE_PinkKey, 116, 20, 120, 24);
  makeTextureFromBuffer(builtInTexture[43], &dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 20, 116, 24);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.hline[i] = builtInTexture[i + 41];
  
  //vertical line
  makeTextureFromBuffer(builtInTexture[44], &dataBuffer[0], GDW, GDH, AE_PinkKey, 120, 16, 124, 20);
  makeTextureFromBuffer(builtInTexture[45], &dataBuffer[0], GDW, GDH, AE_PinkKey, 120, 24, 124, 28);
  makeTextureFromBuffer(builtInTexture[46], &dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 24, 116, 28);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.vline[i] = builtInTexture[i + 44];
  
  //window top bar (also a horizontal line)
  makeTextureFromBuffer(builtInTexture[47], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 0, 20, 16);
  makeTextureFromBuffer(builtInTexture[48], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 0, 26, 16);
  makeTextureFromBuffer(builtInTexture[49], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 0, 32, 16);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.windowTop[i] = builtInTexture[i + 47];
  
  //round button
  makeTextureFromBuffer(builtInTexture[51], &dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 48, 112, 64);
  builtInGuiSet.roundButton = builtInTexture[51];
  builtInGuiSet.slider = builtInTexture[51];
  
  //close button
  makeTextureFromBuffer(builtInTexture[54], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 16, 16, 32);
  builtInGuiSet.closeButton = builtInTexture[54];

  //minimize button
  //55
  
  //maximize button
  //56
  
  //white button
  //57
  
  //resizer of window (corner at bottom right)
  makeTextureFromBuffer(builtInTexture[58], &dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 0, 128, 16);
  builtInGuiSet.resizer = builtInTexture[58];
  
  //tab unselected
  makeTextureFromBuffer(builtInTexture[60], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  112,  4,  116);
  makeTextureFromBuffer(builtInTexture[61], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  112, 10,  116);
  makeTextureFromBuffer(builtInTexture[62], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  112, 16,  116);
  makeTextureFromBuffer(builtInTexture[63], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  118,  4, 122);
  makeTextureFromBuffer(builtInTexture[64], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  118, 10, 122);
  makeTextureFromBuffer(builtInTexture[65], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  118, 16, 122);
  makeTextureFromBuffer(builtInTexture[66], &dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 124,  4, 128);
  makeTextureFromBuffer(builtInTexture[67], &dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 124, 10, 128);
  makeTextureFromBuffer(builtInTexture[68], &dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 124, 16, 128);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabUnSelected[i] = builtInTexture[i + 60];
  
  //tab selected
  makeTextureFromBuffer(builtInTexture[69], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16,  112, 20,  116);
  makeTextureFromBuffer(builtInTexture[70], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22,  112, 26,  116);
  makeTextureFromBuffer(builtInTexture[71], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28,  112, 32,  116);
  makeTextureFromBuffer(builtInTexture[72], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16,  118, 20, 122);
  makeTextureFromBuffer(builtInTexture[73], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22,  118, 23, 122);
  makeTextureFromBuffer(builtInTexture[74], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28,  118, 32, 122);
  makeTextureFromBuffer(builtInTexture[75], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 124, 20, 128);
  makeTextureFromBuffer(builtInTexture[76], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 124, 26, 128);
  makeTextureFromBuffer(builtInTexture[77], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 124, 32, 128);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabSelected[i] = builtInTexture[i + 69];
  
  //small check box unchecked
  makeTextureFromBuffer(builtInTexture[78], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 128, 11, 139);
  builtInGuiSet.smallCheckBox[0] = builtInTexture[78];
  //small check box checked
  makeTextureFromBuffer(builtInTexture[79], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 128, 43, 139);
  builtInGuiSet.smallCheckBox[1] = builtInTexture[79];
  
  //horizontal small slider button
  makeTextureFromBuffer(builtInTexture[80], &dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 112, 101, 123);
  builtInGuiSet.smallSliderH = builtInTexture[80];
  
  //border panel
  makeTextureFromBuffer(builtInTexture[81], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 32, 36, 36);
  makeTextureFromBuffer(builtInTexture[82], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 32, 42, 36);
  makeTextureFromBuffer(builtInTexture[83], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 32, 48, 36);
  makeTextureFromBuffer(builtInTexture[84], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 38, 36, 42);
  makeTextureFromBuffer(builtInTexture[85], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 38, 42, 42);
  makeTextureFromBuffer(builtInTexture[86], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 38, 48, 42);
  makeTextureFromBuffer(builtInTexture[87], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 44, 36, 48);
  makeTextureFromBuffer(builtInTexture[88], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 44, 42, 48);
  makeTextureFromBuffer(builtInTexture[89], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 44, 48, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.borderPanelTextures[i] = builtInTexture[i + 81];
  
  //white panel
  makeTextureFromBuffer(builtInTexture[90], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 32, 52, 36);
  makeTextureFromBuffer(builtInTexture[91], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 32, 58, 36);
  makeTextureFromBuffer(builtInTexture[92], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 32, 64, 36);
  makeTextureFromBuffer(builtInTexture[93], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 38, 52, 42);
  makeTextureFromBuffer(builtInTexture[94], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 38, 58, 42);
  makeTextureFromBuffer(builtInTexture[95], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 38, 64, 42);
  makeTextureFromBuffer(builtInTexture[96], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 44, 52, 48);
  makeTextureFromBuffer(builtInTexture[97], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 44, 58, 48);
  makeTextureFromBuffer(builtInTexture[98], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 44, 64, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.whitePanelTextures[i] = builtInTexture[i + 90];
  
  //fgbgcolor arrows
  makeTextureFromBuffer(builtInTexture[99], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 160, 16, 176);
  builtInGuiSet.fgBgColorArrows = builtInTexture[99];
  
  //white button panel
  makeTextureFromBuffer(builtInTexture[100], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 160, 52, 164);
  makeTextureFromBuffer(builtInTexture[101], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 160, 58, 164);
  makeTextureFromBuffer(builtInTexture[102], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 160, 64, 164);
  makeTextureFromBuffer(builtInTexture[103], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 166, 52, 170);
  makeTextureFromBuffer(builtInTexture[104], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 166, 58, 170);
  makeTextureFromBuffer(builtInTexture[105], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 166, 64, 170);
  makeTextureFromBuffer(builtInTexture[106], &dataBuffer[0], GDW, GDH, AE_PinkKey, 48, 172, 52, 176);
  makeTextureFromBuffer(builtInTexture[107], &dataBuffer[0], GDW, GDH, AE_PinkKey, 54, 172, 58, 176);
  makeTextureFromBuffer(builtInTexture[108], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 172, 64, 176);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.whiteButtonTextures[i] = builtInTexture[i + 100];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.whiteButtonOverTextures[i] = builtInTexture[i + 100];
  
  //white button panel pressed
  makeTextureFromBuffer(builtInTexture[109], &dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 160, 68, 164);
  makeTextureFromBuffer(builtInTexture[110], &dataBuffer[0], GDW, GDH, AE_PinkKey, 70, 160, 74, 164);
  makeTextureFromBuffer(builtInTexture[111], &dataBuffer[0], GDW, GDH, AE_PinkKey, 76, 160, 80, 164);
  makeTextureFromBuffer(builtInTexture[112], &dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 166, 68, 170);
  makeTextureFromBuffer(builtInTexture[113], &dataBuffer[0], GDW, GDH, AE_PinkKey, 70, 166, 74, 170);
  makeTextureFromBuffer(builtInTexture[114], &dataBuffer[0], GDW, GDH, AE_PinkKey, 76, 166, 80, 170);
  makeTextureFromBuffer(builtInTexture[115], &dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 172, 68, 176);
  makeTextureFromBuffer(builtInTexture[116], &dataBuffer[0], GDW, GDH, AE_PinkKey, 70, 172, 74, 176);
  makeTextureFromBuffer(builtInTexture[117], &dataBuffer[0], GDW, GDH, AE_PinkKey, 76, 172, 80, 176);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.whiteButtonDownTextures[i] = builtInTexture[i + 109];

  //invisible button panel
  makeTextureFromBuffer(builtInTexture[118], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 160, 20, 164);
  makeTextureFromBuffer(builtInTexture[119], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 160, 26, 164);
  makeTextureFromBuffer(builtInTexture[120], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 160, 32, 164);
  makeTextureFromBuffer(builtInTexture[121], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 166, 20, 170);
  makeTextureFromBuffer(builtInTexture[122], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 166, 26, 170);
  makeTextureFromBuffer(builtInTexture[123], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 166, 32, 170);
  makeTextureFromBuffer(builtInTexture[124], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 172, 20, 176);
  makeTextureFromBuffer(builtInTexture[125], &dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 172, 26, 176);
  makeTextureFromBuffer(builtInTexture[126], &dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 172, 32, 176);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonTextures[i] = builtInTexture[i + 118];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonOverTextures[i] = builtInTexture[i + 118];
  
  //invisible button panel pressed
  makeTextureFromBuffer(builtInTexture[127], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 160, 36, 164);
  makeTextureFromBuffer(builtInTexture[128], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 160, 42, 164);
  makeTextureFromBuffer(builtInTexture[129], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 160, 48, 164);
  makeTextureFromBuffer(builtInTexture[130], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 166, 36, 170);
  makeTextureFromBuffer(builtInTexture[131], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 166, 42, 170);
  makeTextureFromBuffer(builtInTexture[132], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 166, 48, 170);
  makeTextureFromBuffer(builtInTexture[133], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 172, 36, 176);
  makeTextureFromBuffer(builtInTexture[134], &dataBuffer[0], GDW, GDH, AE_PinkKey, 38, 172, 42, 176);
  makeTextureFromBuffer(builtInTexture[135], &dataBuffer[0], GDW, GDH, AE_PinkKey, 44, 172, 48, 176);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonDownTextures[i] = builtInTexture[i + 127];

  
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
  builtInPanel[7].makeTextured(&builtInGuiSet.whitePanelTextures[0]/*, RGB_White*/);
  builtInGuiSet.whitePanel = &builtInPanel[7];
  builtInPanel[8].makeTextured(&builtInGuiSet.whiteButtonTextures[0]/*, RGB_White*/);
  builtInGuiSet.whiteButtonPanel = &builtInPanel[8];
  builtInPanel[9].makeTextured(&builtInGuiSet.whiteButtonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.whiteButtonOverPanel = &builtInPanel[9];
  builtInPanel[10].makeTextured(&builtInGuiSet.whiteButtonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.whiteButtonDownPanel = &builtInPanel[10];
  builtInPanel[11].makeTextured(&builtInGuiSet.invisibleButtonTextures[0]/*, RGB_White*/);
  builtInGuiSet.invisibleButtonPanel = &builtInPanel[11];
  builtInPanel[12].makeTextured(&builtInGuiSet.invisibleButtonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonOverPanel = &builtInPanel[12];
  builtInPanel[13].makeTextured(&builtInGuiSet.invisibleButtonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonDownPanel = &builtInPanel[13];

  //rules (= 1D versions of panels)
  builtInRule[0].makeHorizontal(builtInTexture[41], builtInTexture[42], builtInTexture[43]);
  builtInGuiSet.sliderHRule = &builtInRule[0];
  builtInRule[1].makeVertical(builtInTexture[44], builtInTexture[45], builtInTexture[46]);
  builtInGuiSet.sliderVRule = &builtInRule[1];
  builtInGuiSet.smallSliderHRule = &builtInRule[0]; //TODO: give it its own rule, which uses the other texture in the GUI texture set
  builtInRule[2].makeHorizontal(builtInTexture[47], builtInTexture[48], builtInTexture[49]);
  builtInGuiSet.windowTopRule = &builtInRule[2];
  
  //colors
  builtInGuiSet.mainColor = RGB_White;
  builtInGuiSet.mouseOverColor = RGB_Brightred;
  builtInGuiSet.mouseDownColor = RGB_Grey;
  
  //font
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
}

////////////////////////////////////////////////////////////////////////////////
//BACKPANEL/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The BackPanel functions.
BackPanel is a class containing 9 textures, being able to draw textured panels of any size with those.
See the .h file for meaning of vareous variables of the classes
*/

//Constructors
BackPanel::BackPanel()
{
  this->enableSides = 0;
  this->enableCenter = 0;
  this->t00 = emptyTexture;
  this->t01 = emptyTexture;
  this->t02 = emptyTexture;
  this->t10 = emptyTexture;
  this->t11 = emptyTexture;
  this->t12 = emptyTexture;
  this->t20 = emptyTexture;
  this->t21 = emptyTexture;
  this->t22 = emptyTexture;
}


void BackPanel::makeUntextured()
{
  this->enableSides = 0;
  this->enableCenter = 0;
  this->t00 = emptyTexture;
  this->t01 = emptyTexture;
  this->t02 = emptyTexture;
  this->t10 = emptyTexture;
  this->t11 = emptyTexture;
  this->t12 = emptyTexture;
  this->t20 = emptyTexture;
  this->t21 = emptyTexture;
  this->t22 = emptyTexture;
}

void BackPanel::makeTextured9(const ITexture* t00, const ITexture* t01, const ITexture* t02, const ITexture* t10, const ITexture* t11, const ITexture* t12, const ITexture* t20, const ITexture* t21, const ITexture* t22)
{
  this->enableSides = 1;
  this->enableCenter = 1;
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
  this->t0 = emptyTexture;
  this->t1 = emptyTexture;
  this->t2 = emptyTexture;
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
  builtInTexture.resize(160);
  for(size_t i = 0; i < builtInTexture.size(); i++) builtInTexture[i] = factory.createNewTexture();
  initBuiltInGui(factory);
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
      guiset->windowTopRule->draw(*drawer, x0, y0, x1 - x0, RGB_Lightblue);
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
    case GP_WHITE_PANEL:
    {
      guiset->whitePanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
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
      if(x1 - x0 >= 16) drawer->drawTexture(guiset->checkBox[0], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[0], x0, y0);
      break;
    }
    case GP_CHECKBOX_ON:
    {
      if(x1 - x0 >= 16) drawer->drawTexture(guiset->checkBox[1], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[1], x0, y0);
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
      int rulerCenter = guiset->sliderHRule->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->sliderHRule->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
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
      drawer->drawRectangle(x0, y0, x1, y1, RGBA_Lightyellow(224), true);
      drawer->drawRectangle(x0, y0, x1, y1, RGB_Black, false);
      break;
    }
    case GP_FG_BG_COLOR_ARROWS:
    {
      drawer->drawTexture(guiset->fgBgColorArrows, x1 - guiset->fgBgColorArrows->getU(), y0);
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
    case GPC_WHITE_PANEL:
    {
      guiset->whitePanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    case GPC_WHITE_BUTTON_PANEL:
    {
      if(mod.mousedown) guiset->whiteButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      else if(mod.mouseover) guiset->whiteButtonOverPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      else guiset->whiteButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    case GPC_WHITE_BUTTON_PANEL_UP:
    {
      guiset->whiteButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    case GPC_WHITE_BUTTON_PANEL_DOWN:
    {
      guiset->whiteButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
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
    };
    default:
      return;
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
    case GP_SMALL_SLIDER_HBACK: return guiset->sliderHRule->t0->getU();
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
    case GP_SMALL_SLIDER_HBACK: return guiset->sliderHRule->t0->getV();
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

} // namespace gui
} // namespace lpi
