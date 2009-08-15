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


#include "lpi_gui_drawer_gl.h"

#include "lodepng.h"
#include "lpi_gl.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include "lpi_gui_input_sdl.h"

namespace lpi
{
namespace gui
{

Texture builtInTexture[128];
BackPanel builtInPanel[6];
BackRule builtInRule[2];


GuiSet builtInGuiSet;

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
<gui>\n\
\n\
  <textures_small>\n\
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\n\
IDCiAIHxfxgyFP1QkQkUAAAAAElFTkSuQmCC\n\
  </textures_small>\n\
\n\
  <icons_small>\n\
iVBORw0KGgoAAAANSUhEUgAAAIAAAABACAMAAADlCI9NAAABaFBMVEX/AP8AAADnyQD////YuQAA\n\
tJHd2gAA011ojutmi+lfheXVAAAAGQoAAgAAAgEAAwEABAEAAQCaAACkAADGAAABAQECAgIEBAQF\n\
BQUGBgYDAwMHBwcODg4UFBQXFxcTExMNDQ0LCwsYGBgmJiY0NDQ8PDwlJSUPDw8MDAwcHBxRUVFr\n\
a2t4eHhqampQUFAfHx9BQUFHR0dCQkIxMTFNTU0hISEICAhcXFyIiIirq6u+vr4jIyNDQ0NwcHCX\n\
l5d6enp7e3thYWHg4ODu7u7t7e12dnbIyMi7u7uLi4u2trZ3d3ff39/39/f9/f01NTWqqqq/v7+5\n\
ubmzs7PExMR9fX3+/v69vb0+Pj5xcXHAwMDX19fR0dGdnZ1KSkoSEhI6OjqcnJzJycnNzc1fX19k\n\
ZGQQEBAuLi58fHyfn5+goKCFhYVGRkZZWVmWlpaDg4ONjY1/f39ISEgREREVFRUtLS07OztFRUUZ\n\
GRlOIfSlAAADCklEQVRo3u2Xi1NSQRTGOSJSUJnYXt4gcAF5+lYulhag5oNSQbMs097vt/Xvt6/L\n\
Xaecrsy424x84z16hmG+3+45Z/fqcDCBQ62gDxT796klUA2A/UEpAe4AUNkFZPWgcgtUA5AO7GPh\n\
YgJQay5Q5w/KCJQDKC8BJrDkuKByOnsAat2dPFxUgF4P/A8APfXUU0/8pegc83/a9xN1vgLgIhLy\n\
ASIhdxMJ+aXLHo9HyL1EthGYPUVgucsUzwdM8dxtiufY20MfnmNvL33gjP6MAMjaXSzACX9GAGTt\n\
QB9KwP07BHT54GW/bALQGpDAAIgzDxSAIOAIJoC5BcABmD8h4ABA/MHmFpjePAgFYEWg7tSc/iUU\n\
gBWB7r4V6AaAFc5WAQYg/J/AAcQa2AGw1B2A04l/yGMXwJIUgCsd+6unAdDtsw9gMsApACYD8B24\n\
hjV4fXBoqLMD1NAC4OeA13YXguVPp4ADmFPACTpT4Hb7hm8M+9ynjaFZBbA9hv10BeI5QACEc4B+\n\
bp0DmMDnc1sAQgUcJ5oAuj0J2QjYPwkFf5EApN0FzL7bu0D9baj+VQBp/kDQH0LwF8kA0MKRaCwW\n\
HwlrAgJC0gBCiWRKT6f1TCzBXBHKIi2iseT8AdBoTs8XisVSWc8lGMDY+MTk1Pj0zKgUgNmkPlcx\n\
qlWjMqcnZwnB/M1bhYXC4u07Y0gCgJZN5SvVWr1Rq1bKmWwIYGR8yVheubu6sJYbRecPEIjqJWN9\n\
o9ncqBklPe7HHXCvcH9za7tltCeRBIBgMl2sNpo7O836bjGdDOKZfLD3cPvR/uPWwpMIyAHYrROA\n\
RpUCwMHTtcOj/e1nrfa0jB7wx3EJaqQE67gE0QAuwfMXxtbm0dZqe0pGCULZTBk3YaOOmzCfymqA\n\
Xr56/WZ5ZeXw7bv3BxJ2AJ0cQ3IMzH9Y+rj36fOXrzNyDqJETi+XisVCXidjh/Xt+4+J45/Hx7+Q\n\
HACUiGXIUZxKJkLmPaBFEIoiOQB47sIj8VgsGglrf1xFcm5DQCF/MODXlFzHvwEO+F56+iRSPQAA\n\
AABJRU5ErkJggg==\n\
  </icons_small>\n\
\n\
</gui>\n\
";

void initBuiltInGui()
{
  initBuiltInGui(builtInGuiData);
}

void initBuiltInGui(const std::string& xmlgui)
{
  xml::XMLTree tree;
  tree.parse(xmlgui);
  
  for(size_t i = 0; i < tree.children.size(); i++)
  {
    if(tree.children[i]->content.name == "textures_small")
    {
      initBuiltInGuiTexturesSmall64(tree.children[i]->content.value);
    }
    else if(tree.children[i]->content.name == "icons_small")
    {
      initBuiltInGuiIconsSmall64(tree.children[i]->content.value);
    }
  }
}


void initBuiltInGuiTexturesSmall(const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiTexturesSmall(png);
}


void initBuiltInGuiTexturesSmall64(const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiTexturesSmall(decoded64);
}

void initBuiltInGuiTexturesSmall(const std::vector<unsigned char>& png)
{
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
  builtInTexture[0].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  0,  4,  4);
  builtInTexture[1].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  0, 10,  4);
  builtInTexture[2].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  0, 16,  4);
  builtInTexture[3].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  6,  4, 10);
  builtInTexture[4].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  6, 10, 10);
  builtInTexture[5].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  6, 16, 10);
  builtInTexture[6].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 12,  4, 16);
  builtInTexture[7].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 12, 10, 16);
  builtInTexture[8].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 12, 16, 16);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.windowTextures[i] = &builtInTexture[i];
  
  //button normal
  builtInTexture[ 9].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 32,  4, 36);
  builtInTexture[10].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 32, 10, 36);
  builtInTexture[11].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 32, 16, 36);
  builtInTexture[12].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 38,  4, 42);
  builtInTexture[13].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 38, 10, 42);
  builtInTexture[14].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 38, 16, 42);
  builtInTexture[15].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 44,  4, 48);
  builtInTexture[16].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 44, 10, 48);
  builtInTexture[17].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 44, 16, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonTextures[i] = &builtInTexture[i + 9];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonOverTextures[i] = &builtInTexture[i + 9];
  
  //button pressed
  builtInTexture[18].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 32, 20, 36);
  builtInTexture[19].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 32, 26, 36);
  builtInTexture[20].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 32, 32, 36);
  builtInTexture[21].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 38, 20, 42);
  builtInTexture[22].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 38, 26, 42);
  builtInTexture[23].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 38, 32, 42);
  builtInTexture[24].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 44, 20, 48);
  builtInTexture[25].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 44, 26, 48);
  builtInTexture[26].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 44, 32, 48);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonDownTextures[i] = &builtInTexture[i + 18];
  
  //button arrow up
  builtInTexture[27].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 64, 16, 80);
  builtInGuiSet.arrowN = &builtInTexture[27];
  //button arrow down
  builtInTexture[28].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 64, 48, 80);
  builtInGuiSet.arrowS = &builtInTexture[28];
  //button arrow left
  builtInTexture[29].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 64, 80, 80);
  builtInGuiSet.arrowW = &builtInTexture[29];
  //button arrow right
  builtInTexture[30].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 64, 112, 80);
  builtInGuiSet.arrowE = &builtInTexture[30];
  
  //scroller
  builtInTexture[31].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 48, 48, 64);
  builtInGuiSet.emptyButton = &builtInTexture[31];
  builtInGuiSet.scroller = &builtInTexture[31];
  //scrollbar background
  builtInTexture[32].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 48, 16, 64);
  builtInGuiSet.scrollbarBackground = &builtInTexture[32];
  builtInGuiSet.scrollBarPairCorner = &builtInTexture[32];
  
  //check box unchecked
  builtInTexture[33].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 80, 16, 96);
  builtInGuiSet.checkBox[0] = &builtInTexture[33];
  //check box checked
  builtInTexture[34].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 80, 48, 96);
  builtInGuiSet.checkBox[1] = &builtInTexture[34];
  
  //bullet unchecked
  builtInTexture[35].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 96, 16, 112);
  builtInGuiSet.bullet[0] = &builtInTexture[35];
  //bullet checked
  builtInTexture[36].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 96, 48, 112);
  builtInGuiSet.bullet[1] = &builtInTexture[36];
  
  //horizontal line
  builtInTexture[41].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 116, 16, 120, 20);
  builtInTexture[42].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 116, 20, 120, 24);
  builtInTexture[43].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 20, 116, 24);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.hline[i] = &builtInTexture[i + 41];
  
  //vertical line
  builtInTexture[44].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 120, 16, 124, 20);
  builtInTexture[45].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 120, 24, 124, 28);
  builtInTexture[46].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 24, 116, 28);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.vline[i] = &builtInTexture[i + 44];
  
  //window top bar (also a horizontal line)
  builtInTexture[47].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 0, 20, 16);
  builtInTexture[48].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 0, 26, 16);
  builtInTexture[49].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 0, 32, 16);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.windowTop[i] = &builtInTexture[i + 47];
  
  //round button
  builtInTexture[51].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 48, 112, 64);
  builtInGuiSet.roundButton = &builtInTexture[51];
  builtInGuiSet.slider = &builtInTexture[51];
  
  //close button
  builtInTexture[54].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 16, 16, 32);
  builtInGuiSet.closeButton = &builtInTexture[54];

  //minimize button
  //55
  
  //maximize button
  //56
  
  //white button
  //57
  
  //resizer of window (corner at bottom right)
  builtInTexture[58].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 0, 128, 16);
  builtInGuiSet.resizer = &builtInTexture[58];
  
  //tab unselected
  builtInTexture[60].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  112,  4,  116);
  builtInTexture[61].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  112, 10,  116);
  builtInTexture[62].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  112, 16,  116);
  builtInTexture[63].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0,  118,  4, 122);
  builtInTexture[64].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6,  118, 10, 122);
  builtInTexture[65].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12,  118, 16, 122);
  builtInTexture[66].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  0, 124,  4, 128);
  builtInTexture[67].create(&dataBuffer[0], GDW, GDH, AE_PinkKey,  6, 124, 10, 128);
  builtInTexture[68].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 12, 124, 16, 128);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabUnSelected[i] = &builtInTexture[i + 60];
  
  //tab selected
  builtInTexture[69].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16,  112, 20,  116);
  builtInTexture[70].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22,  112, 26,  116);
  builtInTexture[71].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28,  112, 32,  116);
  builtInTexture[72].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16,  118, 20, 122);
  builtInTexture[73].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22,  118, 23, 122);
  builtInTexture[74].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28,  118, 32, 122);
  builtInTexture[75].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 124, 20, 128);
  builtInTexture[76].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 22, 124, 26, 128);
  builtInTexture[77].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 28, 124, 32, 128);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabSelected[i] = &builtInTexture[i + 69];
  
  //small check box unchecked
  builtInTexture[78].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 128, 11, 139);
  builtInGuiSet.smallCheckBox[0] = &builtInTexture[78];
  //small check box checked
  builtInTexture[79].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 128, 43, 139);
  builtInGuiSet.smallCheckBox[1] = &builtInTexture[79];
  
  //panels
  builtInPanel[0].makeTextured(builtInGuiSet.windowTextures[0]/*, RGB_White*/);
  builtInGuiSet.windowPanel = &builtInPanel[0];
  builtInPanel[1].makeTextured(builtInGuiSet.buttonTextures[0]/*, RGB_White*/);
  builtInGuiSet.buttonPanel = &builtInPanel[1];
  builtInPanel[2].makeTextured(builtInGuiSet.buttonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonOverPanel = &builtInPanel[2];
  builtInPanel[3].makeTextured(builtInGuiSet.buttonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonDownPanel = &builtInPanel[3];
  builtInPanel[4].makeTextured(builtInGuiSet.tabUnSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabUnSelectedPanel = &builtInPanel[4];
  builtInPanel[5].makeTextured(builtInGuiSet.tabSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabSelectedPanel = &builtInPanel[5];
  
  //rules (= 1D versions of panels)
  builtInRule[0] = BackRule(1);
  builtInGuiSet.sliderHRule = &builtInRule[0];
  builtInRule[1] = BackRule(2);
  builtInGuiSet.sliderVRule = &builtInRule[1];
  
  //colors
  builtInGuiSet.mainColor = RGB_White;
  builtInGuiSet.mouseOverColor = RGB_Brightred;
  builtInGuiSet.mouseDownColor = RGB_Grey;
  
  //markup
  builtInGuiSet.panelButtonMarkup[0] = TS_B;
  builtInGuiSet.panelButtonMarkup[1] = TS_B;
  builtInGuiSet.panelButtonMarkup[2] = TS_B;
  builtInGuiSet.textButtonMarkup[0] = TS_Shadow;
  builtInGuiSet.textButtonMarkup[1] = TS_RShadow;
  builtInGuiSet.textButtonMarkup[2] = TS_RShadow;
}



void initBuiltInGuiIconsSmall(const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiIconsSmall(png);
}

void initBuiltInGuiIconsSmall64(const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiIconsSmall(decoded64);
}

void initBuiltInGuiIconsSmall(const std::vector<unsigned char>& png)
{
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> dataBuffer;
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data

  //image (smiley :D)
  builtInTexture[37].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 32, 32, 48);
  builtInGuiSet.smiley = &builtInTexture[37];
  
  //crosshair
  builtInTexture[50].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 16, 16, 32);
  builtInGuiSet.crossHair = &builtInTexture[50];
  
  //particle 1
  builtInTexture[52].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 48, 16, 64);
  
  //particle 2
  builtInTexture[53].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 48, 32, 64);


}

Texture& TextureSet::operator[](int index)
{
  return texture[index];
}


////////////////////////////////////////////////////////////////////////////////

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
  this->t00 = &emptyTexture;
  this->t01 = &emptyTexture;
  this->t02 = &emptyTexture;
  this->t10 = &emptyTexture;
  this->t11 = &emptyTexture;
  this->t12 = &emptyTexture;
  this->t20 = &emptyTexture;
  this->t21 = &emptyTexture;
  this->t22 = &emptyTexture;
}

BackPanel::BackPanel(int style)
{
  switch(style)
  {
    case 1:
      this->enableSides = 1;
      this->enableCenter = 1;
      this->t00 = &builtInTexture[0];
      this->t01 = &builtInTexture[1];
      this->t02 = &builtInTexture[2];
      this->t10 = &builtInTexture[3];
      this->t11 = &builtInTexture[4];
      this->t12 = &builtInTexture[5];
      this->t20 = &builtInTexture[6];
      this->t21 = &builtInTexture[7];
      this->t22 = &builtInTexture[8];
      break;
    default: break;
  }
}


void BackPanel::makeUntextured()
{
  this->enableSides = 0;
  this->enableCenter = 0;
  this->t00 = &emptyTexture;
  this->t01 = &emptyTexture;
  this->t02 = &emptyTexture;
  this->t10 = &emptyTexture;
  this->t11 = &emptyTexture;
  this->t12 = &emptyTexture;
  this->t20 = &emptyTexture;
  this->t21 = &emptyTexture;
  this->t22 = &emptyTexture;
}

void BackPanel::makeTextured9(const Texture* t00, const Texture* t01, const Texture* t02, const Texture* t10, const Texture* t11, const Texture* t12, const Texture* t20, const Texture* t21, const Texture* t22)
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

void BackPanel::makeTextured(const Texture* t00)
{
  this->enableSides = 1;
  this->enableCenter = 1;
  this->t00 = &t00[0];
  this->t01 = &t00[1];
  this->t02 = &t00[2];
  this->t10 = &t00[3];
  this->t11 = &t00[4];
  this->t12 = &t00[5];
  this->t20 = &t00[6];
  this->t21 = &t00[7];
  this->t22 = &t00[8];
}

/*The draw function of the BackPanel: draws the 9 different textures tiled or
non-tiled at the positions needed to get a window at the correct position
with the correct size*/
void BackPanel::draw(int x, int y, int width, int height, const ColorRGB& colorMod) const
{
  //draw rectangle  
  if(!enableCenter)
  {
    drawRectangle(x, y, x + width, y + height, colorMod);
  }
  //if center texture enabled, draw center texture instead, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
  else
  {
    if(t11 != 0) t11->drawRepeated(x + t00->getU(), y + t00->getV(), x + width - t02->getU(), y + height - t20->getV(), 1.0, 1.0, colorMod);
  }
  
  //draw sides
  if(enableSides)
  {
    t01->drawRepeated(x + t00->getU(),         y                       , x + width - t02->getU(), y + t00->getV()         , 1.0, 1.0, colorMod);
    t21->drawRepeated(x + t00->getU(),         y + height - t20->getV(), x + width - t02->getU(), y + height              , 1.0, 1.0, colorMod);
    t10->drawRepeated(x,                       y + t00->getV()         , x + t00->getU()        , y + height - t20->getV(), 1.0, 1.0, colorMod);
    t12->drawRepeated(x + width - t02->getU(), y + t00->getV()         , x + width              , y + height - t20->getV(), 1.0, 1.0, colorMod);
  }
    
  //draw corners
  if(enableSides)
  {
    t00->draw(x                      , y                       , colorMod); //top left 
    t02->draw(x + width - t02->getU(), y                       , colorMod); //top right
    t20->draw(x                      , y + height - t20->getV(), colorMod); //bottom left
    t22->draw(x + width - t02->getU(), y + height - t20->getV(), colorMod); //bottom right
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
  this->colorMod = RGB_White;
  this->enableSides = 0;
  this->t0 = &emptyTexture;
  this->t1 = &emptyTexture;
  this->t2 = &emptyTexture;
}

BackRule::BackRule(int style)
{
  switch(style)
  {
    case 1: //a default horizontal BackRule
      this->direction = H;
      this->colorMod = RGB_White;
      this->enableSides = 1;
      this->t0 = &builtInTexture[41];
      this->t1 = &builtInTexture[42];
      this->t2 = &builtInTexture[43];
      break;
    case 2: //a default vertical BackRule
      this->direction = V;
      this->colorMod = RGB_White;
      this->enableSides = 1;
      this->t0 = &builtInTexture[44];
      this->t1 = &builtInTexture[45];
      this->t2 = &builtInTexture[46];
      break;
    default: break;
  }
}

void BackRule::makeHorizontal(const Texture* t0, Texture* t1, Texture* t2,
       const ColorRGB& colorMod)
{
  this->direction = H;
  this->colorMod = colorMod;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeHorizontal1(const Texture* t0, const ColorRGB& colorMod)
{
  this->direction = H;
  this->colorMod = colorMod;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::makeVertical(const Texture* t0, Texture* t1, Texture* t2,
       const ColorRGB& colorMod)
{
  this->direction = V;
  this->colorMod = colorMod;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeVertical1(const Texture* t0, const ColorRGB& colorMod)
{
  this->direction = V;
  this->colorMod = colorMod;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::draw(int x, int y, int length) const
{
  if(direction == H)
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    t1->drawRepeated(x + t0->getU(), y, x + length - t2->getU(), y + t0->getV(), 1.0, 1.0, colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      t0->draw(x, y, colorMod); //left
      t2->draw(x + length - t2->getU(), y, colorMod); //right
    }
  }
  else
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    t1->drawRepeated(x, y + t0->getV(), x + t0->getU(), y + length - t2->getV(), 1.0, 1.0, colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      t0->draw(x, y, colorMod); //left
      t2->draw(x, y + length - t2->getV(), colorMod); //right
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void GUIDrawerGL::init()
{
  initBuiltInGui();
}

GUIDrawerGL::GUIDrawerGL(GuiSet* set) : guiset(set)
{
}


void GUIDrawerGL::drawLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  lpi::drawLine(x0, y0, x1, y1, color);
}

void GUIDrawerGL::drawRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color, bool filled)
{
  lpi::drawRectangle(x0, y0, x1, y1, color, filled);
}

void GUIDrawerGL::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const ColorRGB& color, bool filled)
{
  lpi::drawTriangle(x0, y0, x1, y1, x2, y2, color, filled);
}

void GUIDrawerGL::drawQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color, bool filled)
{
  lpi::drawQuad(x0, y0, x1, y1, x2, y2, x3, y3, color, filled);
}

void GUIDrawerGL::drawGradientQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, const ColorRGB& color0, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3)
{
  lpi::gradientQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, color1, color2, color3);
}

void GUIDrawerGL::drawText(const std::string& text, int x, int y, const Markup& markup)
{
  lpi::print(text, x, y, markup);
}

void GUIDrawerGL::drawTextCentered(const std::string& text, int x, int y, const Markup& markup)
{
  lpi::printCentered(text, x, y, markup);
}

void GUIDrawerGL::drawTexture(int x, int y, const Texture* texture, const ColorRGB& colorMod)
{
  texture->draw(x, y, colorMod);
}

void GUIDrawerGL::drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, bool /*inactive*/, const ColorRGB& color)
{
  switch(part)
  {
    case GP_WINDOW_PANEL:
    {
      guiset->windowPanel->draw(x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    case GP_TAB_UNSELECTED:
    {
      guiset->tabUnSelectedPanel->draw(x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    case GP_TAB_SELECTED:
    {
      guiset->tabSelectedPanel->draw(x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    default:
      return;
  }
}

void GUIDrawerGL::setSmallestScissor(int x0, int y0, int x1, int y1)
{
  lpi::setSmallestScissor(x0, y0, x1, y1);
}

void GUIDrawerGL::resetScissor()
{
  lpi::resetScissor();
}


IGUIInput& GUIDrawerGL::getInput()
{
  return lpi::gGUIInput;
}

} //namespace gui
} //namespace lpi


namespace lpi
{

gui::GUIDrawerGL gGUIDrawer(&lpi::gui::builtInGuiSet);

} //namespace lpi
