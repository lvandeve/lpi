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
#include "lpi_screen.h"
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
  this->t0 = &emptyTexture;
  this->t1 = &emptyTexture;
  this->t2 = &emptyTexture;
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
  builtInTexture.resize(128);
  for(size_t i = 0; i < 128; i++) builtInTexture[i] = factory.createNewTexture();
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
      drawer->drawTexture(guiset->closeButton, x1 - guiset->closeButton->getU(), y0);
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
    default:
      return;
  }
}

void GUIPartDrawerInternal::drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GPT_TEXT_BUTTON:
    {
      if(mod.mousedown) textdrawer->drawText(text, (x0+x1)/2 + 2, (y0+y1)/2 + 2, guiset->textButtonFont[2], TextAlign(HA_CENTER, VA_TOP));
      else if(mod.mouseover) textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[1], TextAlign(HA_CENTER, VA_TOP));
      else textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[0], TextAlign(HA_CENTER, VA_TOP));
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
    default: return 0;
  }
}

} // namespace gui
} // namespace lpi
