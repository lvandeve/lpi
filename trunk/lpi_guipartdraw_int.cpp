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
PNG image, encoded in base64, containing all the GUI images
First use the function decodeBase64
Then use the function LodePNG::Decoder::decode
And you get the pixels in a buffer
*/
const std::string builtInGuiData = "\
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\n\
YII=\n\
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
  
  //horizontal line
  makeTextureFromBuffer(builtInTexture[41], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 140, 24, 144);
  makeTextureFromBuffer(builtInTexture[42], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 140, 30, 144);
  makeTextureFromBuffer(builtInTexture[43], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 140, 36, 144);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.hline[i] = builtInTexture[i + 41];
  
  //vertical line
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
  makeTextureFromBuffer(builtInTexture[80], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 160, 45, 171);
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
  
  //menu panels and separators
  builtInGuiSet.horMenuPanel = builtInGuiSet.buttonPanel;
  builtInGuiSet.verMenuPanel = builtInGuiSet.buttonPanel;
  builtInGuiSet.horMenuSeparator = builtInGuiSet.sliderVRule;
  builtInGuiSet.verMenuSeparator = builtInGuiSet.sliderHRule;
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
      guiset->verMenuSeparator->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_HMENU_SEPARATOR:
    {
      int rulerCenter = guiset->horMenuSeparator->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->horMenuSeparator->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }
    case GP_VMENU_SUBMENUINDICATOR:
    {
      const ITexture* t = guiset->subMenuIndicator;
      drawer->drawTexture(t, x1 - t->getU(), ((y0 + y1) / 2) - t->getV() / 2);
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
    case GPT_HMENU_TEXT:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, FONT_Black, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_VMENU_TEXT:
    {
      textdrawer->drawText(text, x0+4, (y0+y1)/2, FONT_Black, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
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
