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
<gui>\n\
\n\
  <textures_small>\n\
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\n\
lvvFHLr+AAAAAElFTkSuQmCC\n\
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

void GUIPartDrawerInternal::initBuiltInGui(const ITextureFactory& factory)
{
  initBuiltInGui(factory, builtInGuiData);
}

void GUIPartDrawerInternal::initBuiltInGui(const ITextureFactory& factory, const std::string& xmlgui)
{
  xml::XMLTree tree;
  tree.parse(xmlgui);
  
  for(size_t i = 0; i < tree.children.size(); i++)
  {
    if(tree.children[i]->content.name == "textures_small")
    {
      initBuiltInGuiTexturesSmall64(factory, tree.children[i]->content.value);
    }
    else if(tree.children[i]->content.name == "icons_small")
    {
      initBuiltInGuiIconsSmall64(factory, tree.children[i]->content.value);
    }
  }
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



void GUIPartDrawerInternal::initBuiltInGuiIconsSmall(const ITextureFactory& factory, const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiIconsSmall(factory, png);
}

void GUIPartDrawerInternal::initBuiltInGuiIconsSmall64(const ITextureFactory& factory, const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiIconsSmall(factory, decoded64);
}

void GUIPartDrawerInternal::initBuiltInGuiIconsSmall(const ITextureFactory& factory, const std::vector<unsigned char>& png)
{
  (void)factory;
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> dataBuffer;
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data

  //image (smiley :D)
  makeTextureFromBuffer(builtInTexture[37], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 32, 32, 48);
  builtInGuiSet.smiley = builtInTexture[37];
  
  //crosshair
  makeTextureFromBuffer(builtInTexture[50], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 16, 16, 32);
  builtInGuiSet.crossHair = builtInTexture[50];
  
  //particle 1
  makeTextureFromBuffer(builtInTexture[52], &dataBuffer[0], GDW, GDH, AE_PinkKey, 0, 48, 16, 64);
  
  //particle 2
  makeTextureFromBuffer(builtInTexture[53], &dataBuffer[0], GDW, GDH, AE_PinkKey, 16, 48, 32, 64);


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
