/*
Copyright (c) 2005-2007 Lode Vandevenne
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


#include "lpi_gui.h"

#include "lodepng.h"
#include "lpi_general.h"
#include "lpi_screen.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"

#include "lpi_xml.h"

#include <SDL/SDL.h>

#include <iostream>

namespace lpi
{
namespace gui
{

Texture builtInTexture[128];
BackPanel builtInPanel[4];
BackRule builtInRule[2];
GuiSet builtInGuiSet;

MouseState::MouseState()
{
  justdown_prev = 0;
  justdownhere_prev = 0;
  grabbed_grabbed = 0;
  grabbed_prev = 0;
  downhere_bool1 = 0;
  downhere_bool2 = 0;
  justuphere_bool1 = 0;
  justuphere_bool2 = 0;
}

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
iVBORw0KGgoAAAANSUhEUgAAAIAAAABgCAMAAADipIp7AAAAclBMVEUAAAABAQGAgICEhISFhYWJ\n\
iYmKioqNjY2Pj4+RkZGUlJSVlZWZmZmampqenp6ioqKmpqanp6erq6usrKyvr6+xsbGzs7O2tra3\n\
t7e7u7u8vLzAwMDHx8fPz8/X19fg4ODo6Ojw8PD4+Pj/AAD/AP////9MjY55AAAD5UlEQVRo3u2Z\n\
27baOAyG/6gpZRialKEMpdpnWe//inNhK3GclCST01p7oQuxtAXShy1L3gSqqiK1BgCRWqe2m1mg\n\
zCzCphVNGyAiETK9AUCe5yK5aTjnUEmIMdIeCbDb7UR2puEcrpfzqSyOh/0Ozo23Y1EHSaQFsN/v\n\
Rfam5wVQ6gB4enoSeTINHA4HkYPpWQGUurbg+flZ5Nk0cDweRY6mhwIgsbkDQKmzBl5eXkReTANF\n\
UYgUpuEcrqGkqgTsJUoIoAHAzC0AJee0A+D19VXk1TRQlqVIadqvAAAgSsDMzJF91x/npw6At7c3\n\
kTfTwOl0EjmZRv837FuhRv6OInx/fxd5Nw2cz2eRs2n07/GgIrT8HQAfHx8iH6aBy+UicjE90ymw\n\
/F010JwNwPV6Fbmangegyk8DAG63m8jN9CytOMo/ACDpjDPMnzi/w+bj2G0sGwGohhfF2Pk9k5Bz\n\
5JwSYdz8nnMRwjF0Dlf4hKhap7d5h1VqANcLTmVxRDQ8TmVx5EVWQP3Wq6p/teFSFmgMl7LgZbaA\n\
yDny4ojqGkim26ljfi9YA6sCxH2wBlhvC8Lx0wCgqxdhVQka9YFNjmFSA+s1onYNrNuKVcmpktrr\n\
6sPIzn/UBzaS6krWvJAQNf8dp/5m3pDoj8P86ZWMKLmi9ebnW120uV9R4HtR/vApoA0Jfn/3WgoA\n\
f/39vSh//PMT3QBoGrMD4JsB/IsuADQt6ZHRAPhaA/xCGwCJuTnA3FuALzHAb6QAbfsBsDXA4xRs\n\
3og+YSseOYzS8UvJ7wWLj2NOpDdBR0BfqqFgybl2Eaf3uxEArXOeLnl6gSFtAcRb0tqiqQDJjcp1\n\
ADSKslWkk1egtQUpQPNYto7pVIDeRpY0plaj+nQA6Ra0hlNqPwA+HcDjFGzeiFZvxQMApg2jvnHc\n\
vkPquHHcO+8XttG3xIvbD4BPA4DE5oaN5P3cAkASkEcCIAHgBCCJz5wAIAnAjQDV0/BgV0/DowT3\n\
Pn/XD77Vz/vtDfa83+wh/mPiTwGQAHAEsOg3HLQCA/a4p0am1sD2p+DRiLYD2HwcTw7oJU+uMYPt\n\
9F4+2g7x0Mww3AY4/gmnK+F9f4jHjQwj7CEAWeadWeb9RN5HFABysMYZcjAn9h/9wwAyn78CIJ8/\n\
AORg1ShD7mPEtiLxk/kHbUGWZUF5PxEZBix/TZBbiNpWAA2/h88H10CWZX4Zgt8/9/M1kFcRfMS8\n\
jmC2AjVByG8EA4sw5K/8lp+RRwEYeZyfg63hyFZ+oppgMsA6K3BvC9aogftFuPwp6DuGK/SBnka0\n\
dCfsb8XTZsEcw2jCNJxpHP9v+Q/413JTkDduzAAAAABJRU5ErkJggg==\n\
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
  builtInTexture[35].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 64, 80, 80, 96);
  builtInGuiSet.bullet[0] = &builtInTexture[35];
  //bullet checked
  builtInTexture[36].create(&dataBuffer[0], GDW, GDH, AE_PinkKey, 96, 80, 112, 96);
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
  
  //panels
  builtInPanel[0].makeTextured(builtInGuiSet.windowTextures[0], RGB_White);
  builtInGuiSet.windowPanel = &builtInPanel[0];
  builtInPanel[1].makeTextured(builtInGuiSet.buttonTextures[0], RGB_White);
  builtInGuiSet.buttonPanel = &builtInPanel[1];
  builtInPanel[2].makeTextured(builtInGuiSet.buttonOverTextures[0], RGB_Grey);
  builtInGuiSet.buttonOverPanel = &builtInPanel[2];
  builtInPanel[3].makeTextured(builtInGuiSet.buttonDownTextures[0], RGB_Grey);
  builtInGuiSet.buttonDownPanel = &builtInPanel[3];
  
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
  this->colorMod = RGB_White;
  this->fillColor = RGB_White;
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
      this->colorMod = RGB_White;
      this->fillColor = RGB_White;
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

BackPanel::BackPanel(const ColorRGB& color)
{
  makeUntextured(color);
}

void BackPanel::makeUntextured(const ColorRGB& fillColor)
{
  this->colorMod = RGB_White;
  this->fillColor = fillColor;
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

void BackPanel::makeTextured9(const Texture* t00, const Texture* t01, const Texture* t02, const Texture* t10, const Texture* t11, const Texture* t12, const Texture* t20, const Texture* t21, const Texture* t22,
       const ColorRGB& colorMod)
{
  this->colorMod = colorMod;
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

void BackPanel::makeTextured(const Texture* t00, const ColorRGB& colorMod)
{
  this->colorMod = colorMod;
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
void BackPanel::draw(int x, int y, int width, int height) const
{
  //draw rectangle  
  if(!enableCenter)
  {
    drawRectangle(x, y, x + width, y + height, fillColor);
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
//BASICELEMENT
////////////////////////////////////////////////////////////////////////////////

int BasicElement::mouseGetRelPosX() const
{
  return globalMouseX - x0;
}

int BasicElement::mouseGetRelPosY() const
{
  return globalMouseY - y0;
}

bool BasicElement::mouseOver() const
{
  bool over = 0;
  
  //some variables used during case calculations
  int relX, relY, symX, symY;
  
  //only if the mouse is in the rectangle, and the current object is on top, the other shapes should be tested
  if(globalMouseX >= x0 && globalMouseX < x1 && globalMouseY >= y0 && globalMouseY < y1)
  switch(shape)
  {
    case 0: //rectangle
      over = 1;
      break;
    case 1: //triangle pointing up
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symX = std::abs(getSizex() / 2 - relX); //it's symmetrical
      if(relY >= (2 * symX * getSizey()) / getSizex()) over = 1;
      break;
    case 2: //triangle pointing right
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symY = std::abs(getSizey() / 2 - relY); //it's symmetrical
      if(getSizex() - relX >= (2 * symY * getSizex()) / getSizey()) over = 1;
      break;
    case 3: //triangle pointing down
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symX = std::abs(getSizex() / 2 - relX); //it's symmetrical
      if(getSizey() - relY >= (2 * symX * getSizey()) / getSizex()) over = 1;
    case 4: //triangle pointing left
      relX = globalMouseX - x0;
      relY = globalMouseY - y0;
      symY = std::abs(getSizey() / 2 - relY); //it's symmetrical
      if(relX >= (2 * symY * getSizey()) / getSizey()) over = 1;
      break;
    default: //rectangle
      over = 1;
      break;
  }

  return over;
}

bool BasicElement::mouseDown(MouseButton button) const
{
  return mouseOver() && getGlobalMouseButton(button);
}

bool BasicElement::mouseDownHere(MouseState& state, MouseButton button) const
{
  bool down = getGlobalMouseButton(button);//mouseDown(button);
  bool over = mouseOver();
  
  if(!down)
  {
    state.downhere_bool1 = false;
    state.downhere_bool2 = false;
  }
  
  if(down && state.downhere_bool2 == false) //state.downhere_bool2 means justOver (it's the prevstate)
  {
    state.downhere_bool2 = true;
    state.downhere_bool1 = over; //true means it was inside when just clicked, false that it was not
  }
  
  return state.downhere_bool1 && over;
}

bool BasicElement::mouseDownHere(MouseButton button)
{
  return mouseDownHere(_mouseState[button], button);
}

bool BasicElement::mouseGrabbed(MouseState& state, MouseButton button) const
{
  if(!mouseGrabbable()) return false;
  
  //grab
  if(mouseJustDownHere(state.grabbed_prev, button))
  {
    state.grabbed_grabbed = true;
    state.grabx = globalMouseX;
    state.graby = globalMouseY;
    state.grabrelx = mouseGetRelPosX();
    state.grabrely = mouseGetRelPosY();
  }
  
  //ungrab
  if(!getGlobalMouseButton(button))
  {
    state.grabbed_grabbed = false;
    state.grabbed_prev = false;
  }

  return state.grabbed_grabbed;
}

bool BasicElement::mouseGrabbed(MouseButton button)
{
  return mouseGrabbed(_mouseState[button], button);
}

void BasicElement::mouseGrab(MouseState& state) const
{
  state.grabbed_grabbed = true;
  state.grabx = globalMouseX;
  state.graby = globalMouseY;
  state.grabrelx = mouseGetRelPosX();
  state.grabrely = mouseGetRelPosY();
}

void BasicElement::mouseGrab(MouseButton button)
{
  mouseGrab(_mouseState[button]);
}


void BasicElement::mouseUngrab(MouseState& state) const
{
  state.grabbed_grabbed = false;
  state.grabbed_prev = false;
}

void BasicElement::mouseUngrab(MouseButton button)
{
  mouseUngrab(_mouseState[button]);
}

bool BasicElement::mouseJustDown(bool& prevstate, MouseButton button) const
{
  bool nowDown = mouseOver() && getGlobalMouseButton(button);
  
  if(nowDown)
  {
    if(!prevstate)
    {
      prevstate = true;
      return true;
    }
    else return false;
  }
  else
  {
    prevstate = false;
    return false;
  }
}

bool BasicElement::mouseJustDown(MouseState& state, MouseButton button) const
{
  return mouseJustDown(state.justdown_prev, button);
}

bool BasicElement::mouseJustDown(MouseButton button)
{
  return mouseJustDown(_mouseState[button], button);
}

bool BasicElement::mouseJustDownHere(bool& prevstate, MouseButton button) const
{
  bool nowDown = mouseOver() && getGlobalMouseButton(button);
  
  if(nowDown)
  {
    if(!prevstate)
    {
      prevstate = true;
      return true;
    }
    else return false;
  }
  else
  {
    prevstate = false;
  }
  
  if(!mouseOver()) prevstate = true; //so that it can't return true anymore after mouse was not on this

  return false;
}

bool BasicElement::mouseJustDownHere(MouseState& state, MouseButton button) const
{
  return mouseJustDownHere(state.justdownhere_prev, button);
}

bool BasicElement::mouseJustDownHere(MouseButton button)
{
  return mouseJustDownHere(_mouseState[button], button);
}

bool BasicElement::mouseJustUpHere(MouseState& state, MouseButton button) const
{
  if(mouseJustDownHere(state.justuphere_bool1, button))
  {
    state.justuphere_bool2 = true;
  }

  if(state.justuphere_bool2 && !getGlobalMouseButton(button))
  {
    state.justuphere_bool2 = false;
    if(mouseOver()) return true;
    else return false;
  }
  
  return false;
}

bool BasicElement::mouseJustUpHere(MouseButton button)
{
  return mouseJustUpHere(_mouseState[button], button);
}

bool BasicElement::pressed(MouseButton button)
{
  return mouseActive() && mouseJustDownHere(button);
}

bool BasicElement::clicked(MouseButton button)
{
  return mouseActive() &&  mouseJustUpHere(button);
}

bool BasicElement::mouseScrollUp() const
{
  return mouseActive() && mouseOver() && globalMouseWheelUp;
}

bool BasicElement::mouseScrollDown() const
{
  return mouseActive() && mouseOver() && globalMouseWheelDown;
}

bool BasicElement::mouseDoubleClicked(MouseState& state, MouseButton button) const
{
  bool dclick = 0;
  
  bool down = mouseOver() && getGlobalMouseButton(button); //is the button down

  switch(state.doubleClickState)
  {
    case 0:
      if(down)
      {
        state.doubleClickTime = getTicks() / 1000.0;
        state.doubleClickState = 1;
      }
      break;
    case 1:
      if(!down)
      {
        state.doubleClickState = 2;
      }
      break;
    case 2:
      if(down)
      {
        state.doubleClickState = 3;
      }
      break;
    case 3:
      if(!down)
      {
        state.doubleClickState = 0;
        if(getTicks() / 1000.0 - state.doubleClickTime < doubleClickTime) dclick = 1;
      }
      break;
  }
  
  if
  (
     (state.doubleClickState > 0 && (getTicks() / 1000.0) - state.doubleClickTime  > doubleClickTime)
  || (!mouseOver())
  )
  {
    state.doubleClickState = 0;
    state.doubleClickTime = 0;
  }
  
  return dclick;
}

bool BasicElement::mouseDoubleClicked(MouseButton button)
{
  return mouseDoubleClicked(_mouseState[button], button);
}

BasicElement::BasicElement() : x0(0),
                               y0(0),
                               x1(0),
                               y1(0),
                               shape(0),
                               doubleClickTime(500)
{}

////////////////////////////////////////////////////////////////////////////////
//TOOLTIPMANAGER                                                              //
////////////////////////////////////////////////////////////////////////////////

void ToolTipManager::registerMe(const Element* element)
{
  this->element = element;
}

void ToolTipManager::draw() const
{
  if(enabled && element)
    element->drawToolTip();
}

ToolTipManager defaultTooltipManager;

////////////////////////////////////////////////////////////////////////////////
//GUIELEMENT////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
GuiElement is the mother class of all the other GUI classes below.
*/

Element::Element() : selfActivate(false),
                     minSizex(0),
                     minSizey(0),
                     labelX(0),
                     labelY(0),
                     tooltipenabled(false),
                     hasBackgroundRectangle(false),
                     elementOver(false),
                     notDrawnByContainer(false)
{
  totallyEnable();
}

void Element::drawBorder(const ColorRGB& color) const
{
  drawLine(x0    , y0    , x0    , y1 - 1, color);
  drawLine(x1 - 1, y0    , x1 - 1, y1 - 1, color);
  drawLine(x0    , y0    , x1 - 1, y0     , color);
  drawLine(x0    , y1 - 1, x1 - 1, y1 - 1, color);
}

void Element::drawLabel() const
{
  if(label.length() > 0)
  print(label, x0 + labelX, y0 + labelY, labelMarkup);
}

void Element::drawToolTip() const
{
  if(tooltipenabled && mouseOver())
  {
    int numlines = 1;
    int linelength = 0;
    int longestline = 0;
    for(size_t i = 0; i < tooltip.size(); i++)
    {
      if(tooltip[i] == 10 || (tooltip[i] == 13 && (i == 0 || tooltip[i - 1] != 10)))
      {
        numlines++;
        if(linelength > longestline) longestline = linelength;
        linelength = 0;
      }
      else linelength++;
    }
    if(linelength > longestline) longestline = linelength;
    int sizex = longestline * TS_Black6.getWidth() + 4;
    int sizey = 2 +  TS_Black6.getHeight() * numlines;
    int x = globalMouseX;
    int y = globalMouseY - sizey;
    drawRectangle(x, y, x + sizex, y + sizey, RGBA_Lightyellow(224));
    printText(tooltip, x + 2, y + 2, TS_Black6);
  }
}

void Element::makeLabel(const std::string& label, int labelX, int labelY, const Markup& labelMarkup)
{
  this->label = label;
  this->labelX = labelX;
  this->labelY = labelY;
  this->labelMarkup = labelMarkup;
}

bool Element::mouseOver() const
{
  if(!present) return false;
  if(elementOver) return false; //there is another element over this element, so says the container containing this element
  
  return BasicElement::mouseOver();
}

bool Element::mouseGrabbable() const
{
  return present;
}


void Element::draw() const
{
  if(!visible) return;
  
  if(hasBackgroundRectangle) drawRectangle(x0, y0, x1, y1, backgroundRectangleColor);
  
  drawWidget();
  drawLabel();
  
  if(tooltipenabled && mouseOver()) tooltipmanager->registerMe(this);
}

/*void Element::drawWidget() const
{
  //this function is pure virtual
}*/

void Element::move(int x, int y)
{
  this->x0 += x;
  this->y0 += y;
  this->x1 += x;
  this->y1 += y;
  
  for(unsigned long i = 0;;i++)
  {
    Element* element = getAutoSubElement(i);
    if(!element) break;
    else element->move(x, y);
  }
  
  moveWidget(x, y);
}

void Element::moveWidget(int /*x*/, int /*y*/)
{
  //this function is virtual
}

void Element::moveTo(int x, int y)
{
  this->move(x - this->x0, y - this->y0);
}

void Element::moveCenterTo(int x, int y)
{
  this->moveTo(x - this->getSizex() / 2, y - this->getSizey() / 2);
}

void Element::autoActivate()
{
  if(selfActivate)
  {
    if(mouseDownHere(auto_activate_mouse_state)) active = 1;
    else
    {
      if(!mouseOver() && globalLMB && !mouseGrabbed(auto_activate_mouse_state)) active = 0; //"forceActive" enabled so that this disactivating also works if mouseActive is false!
      //without the mouseGrabbed() test, it'll become inactive when you grab the scrollbar scroller and go outside with the mouse = annoying
    }
  }
}

void Element::handle()
{
  autoActivate();
  
  if(!active) return;
  
  handleWidget();
}

void Element::handleWidget()
{
  //no stuff needed for most elements
}

void Element::setElementOver(bool state)
{
  elementOver = state;
  
  for(unsigned long i = 0;;i++)
  {
    Element* element = getAutoSubElement(i);
    if(!element) break;
    else element->setElementOver(state);
  }
}

bool Element::hasElementOver() const
{
  return elementOver;
}

bool Element::isContainer() const
{
  return 0;
}

//I have serious doubts if this function shouldn't be made obsolete: use a container the size of the screen instead and use it's function to keep elements inside...
//no wait the function is still handy here and there if you want to compare something to the screen, not it's container, or if there is no container at all
void Element::putInScreen()
{
    int newx = x0, newy = y0;
    
    if(x0 < 0) newx = 0;
    if(y0 < 0) newy = 0;
    if(x1 >= screenWidth()) newx = screenWidth() - getSizex();
    if(y1 >= screenHeight()) newy = screenWidth() - getSizey();
    
    moveTo(newx, newy);
}

void Element::resize(int x0, int y0, int x1, int y1)
{
  if(x1 - x0 < minSizex) x1 = x0 + minSizex;
  if(y1 - y0 < minSizey) y1 = y0 + minSizey;

  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  
  for(unsigned long i = 0;;i++)
  {
    Element* element = getAutoSubElement(i);
    if(!element) break;
    else element->resizeSticky(this->x0, this->y0, this->x1, this->y1);
  }
  
  resizeWidget();
}

void Element::resizeWidget()
{
  //nothing to do. Overload this for guielements that need to do something to sub elements if they resize.
}

//this function uses ix0, iy0, ix1 and iy1 so only use if those values are properly set!
void Element::resizeSticky(int x0, int y0, int x1, int y1)
{
  int newMasterSizex = x1 - x0;
  int newMasterSizey = y1 - y0;
  
  int ex0, ey0, ex1, ey1; //the new positions for this element that have to be calculated now
  
  ex0 = int(x0 + ix0 + (leftSticky * newMasterSizex) - (leftSticky * iMasterSizex));
  ey0 = int(y0 + iy0 + (topSticky * newMasterSizey) - (topSticky * iMasterSizey));
  
  ex1 = int(x0 + ix1 + (rightSticky * newMasterSizex) - (rightSticky * iMasterSizex));
  ey1 = int(y0 + iy1 + (bottomSticky * newMasterSizey) - (bottomSticky * iMasterSizey));
  
  if(leftSticky == -2.0) ex0 = this->x0;
  else if(leftSticky == -3.0) {ex0 = this->x0; if(ex0 < x0) ex1 = ex1 + (x0 - ex0), ex0 = x0;}
  if(topSticky == -2.0) ey0 = this->y0;
  else if(topSticky == -3.0) {ey0 = this->y0; if(ey0 < y0) ey1 = ey1 + (y0 - ey0), ey0 = y0;}
  if(rightSticky == -2.0) ex1 = this->x1;
  else if(rightSticky == -3.0) {ex1 = this->x1; if(ex1 > x1) ex0 = ex0 - (ex1 - x1), ex1 = x1;}
  if(bottomSticky == -2.0) ey1 = this->y1;
  else if(bottomSticky == -3.0) {ey1 = this->y1; if(ey1 > y1) ey0 = ey0 - (ey1 - y1), ey1 = y1;}

  resize(ex0, ey0, ex1, ey1);
}

////////////////////////////////////////////////////////////////////////////////
//GUICONTAINER//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Contains many elements in it and draws them and handles them, and even makes
sure you can't press the mouse "through" elements on top of other elements.

Also can have scrollbars.
*/

void Container::setScrollSizeToElements()
{
  int newx0 = x0, newy0 = y0, newx1 = x1, newy1 = y1;
  if(size() > 0)
  {
    newx0 = element[0]->getX0();
    newy0 = element[0]->getY0();
    newx1 = element[0]->getX1();
    newy1 = element[0]->getY1();
  }
  for(unsigned i = 1; i < size(); i++)
  {
    if(element[i]->getX0() < newx0) newx0 = element[i]->getX0();
    if(element[i]->getY0() < newy0) newy0 = element[i]->getY0();
    if(element[i]->getX1() > newx1) newx1 = element[i]->getX1();
    if(element[i]->getY1() > newy1) newy1 = element[i]->getY1();
  }
  
  area.resizeSticky(newx0, newy0, newx1, newy1);
}

Container::Container() : keepElementsInside(false)
{
  clear(); //clear the element list
  totallyEnable();
  
  //the default container is as big as the screen (note: don't forget to resize it if you resize the resolution of the screen!)
  x0 = 0;
  y0 = 0;
  x1 = screenWidth();
  y1 = screenHeight();
}

void Container::make(int x, int y, int sizex, int sizey, 
            int areax, int areay, int areasizex, int areasizey, //areax and areay are relative to the container!
            double areaLeftSticky, double areaTopSticky, double areaRightSticky, double areaBottomSticky)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  bars.make(x, y, sizex, sizey);
  bars.disableV();
  bars.disableH();
  bars.setSticky(0, 0, 1, 1);
  bars.saveInitialPosition(this->x0, this->y0, this->x1, this->y1);

  if(areasizex == -1)
  {
    areax = 0;
    areasizex = sizex;
  }
  
  if(areasizey == -1)
  {
    areay = 0;
    areasizey = sizey;
  }
  
  area.make(this->x0 + areax, this->y0 + areay, this->x0 + areax + areasizex, this->y0 + areay + areasizey);
  area.setSticky(areaLeftSticky, areaTopSticky, areaRightSticky, areaBottomSticky);
  area.saveInitialPosition(this->x0, this->y0, this->x1, this->y1);

  initBars();
}

void Container::setElementOver(bool state)
{
  Element::setElementOver(state);
  bars.setElementOver(state);
  for(size_t i = 0; i < size(); i++) element[i]->setElementOver(state);
}

void Container::toggleBars() //turns bars on or of if they're needed or not (depending on visible size and scroll area size) (different than MS Windows where scrollbars become inactive grey when not needed)
{
  if(area.getSizex() <= getVisibleSizex() && area.getSizey() <= getVisibleSizey())
  {
    bars.disableH();
    bars.disableV();
  }
  else if(area.getSizex() > getVisibleSizex() && area.getSizey() <= getVisibleSizey())
  {
    bars.enableH();
    bars.disableV();
  }
  else if(area.getSizex() <= getVisibleSizex() && area.getSizey() > getVisibleSizey())
  {
    bars.disableH();
    bars.enableV();
  }
  else if(area.getSizex() > getVisibleSizex() && area.getSizey() > getVisibleSizey())
  {
    bars.enableH();
    bars.enableV();
  }
}

void Container::initBars()
{
  toggleBars();

  bars.vbar.scrollSize = area.getSizey() - getVisibleSizey();
  bars.hbar.scrollSize = area.getSizex() - getVisibleSizex();
  bars.vbar.scrollPos = y0 - area.getY0();
  bars.hbar.scrollPos = x0 - area.getX0();
}

void Container::updateBars()
{
  toggleBars();

  bars.hbar.scrollSize = area.getSizex() - getVisibleSizex();
  bars.vbar.scrollSize = area.getSizey() - getVisibleSizey();
  if(bars.hbar.scrollSize < 0) bars.hbar.scrollSize = 0;
  if(bars.vbar.scrollSize < 0) bars.vbar.scrollSize = 0;
  if(bars.hbar.scrollPos > bars.hbar.scrollSize) bars.hbar.scrollPos = bars.hbar.scrollSize;
  if(bars.vbar.scrollPos > bars.vbar.scrollSize) bars.vbar.scrollPos = bars.vbar.scrollSize;
}

int Container::getVisibleX0() const
{
  return x0;
}
int Container::getVisibleY0() const
{
  return y0;
}

int Container::getVisibleX1() const
{
  if(bars.venabled) return x1 - bars.vbar.getSizex();
  else return x1;
}
int Container::getVisibleY1() const
{
  if(bars.henabled) return y1 - bars.hbar.getSizey();
  else return y1;
}

int Container::getVisibleSizex() const
{
  return getVisibleX1() - getVisibleX0();
}

int Container::getVisibleSizey() const
{
  return getVisibleY1() - getVisibleY0();
}

bool Container::mouseInVisibleZone() const
{
  if(!mouseOver()) return false;
  return globalMouseX >= getVisibleX0()
      && globalMouseX < getVisibleX1()
      && globalMouseY >= getVisibleY0()
      && globalMouseY < getVisibleY1();
}

void Container::handleWidget()
{
  //if(mouseOver())
  if(!elementOver && mouseInVisibleZone())
  {
    int topElement = -1;
    
    //priority to mouseGrabbed over mouseOver
    for(int i = size() - 1; i >= 0; i--)
    {
      element[i]->setElementOver(0);
      if(element[i]->mouseGrabbed(element[i]->getMouseStateForContainer()))
      {
        if(topElement < 0) topElement = i;
        //break;
      }
      element[i]->setElementOver(1);
    }
    
    //only now test mouseOver
    if(topElement == -1)
    for(int i = size() - 1; i >= 0; i--)
    {
      element[i]->setElementOver(0);
      if(element[i]->mouseOver())
      {
        topElement = i;
        break;
      }
      element[i]->setElementOver(1);
    }

    //make all elements unresponsive to the mouse by setting "elementover", except the topElement
    for(size_t i = 0; i < size(); i++) element[i]->setElementOver(1);
    if(topElement >= 0 && topElement < (int)size())
    {
      element[topElement]->setElementOver(0);
      if(element[topElement]->isContainer() && element[topElement]->mouseDownHere(element[topElement]->getMouseStateForContainer())) bringToTop(element[topElement]);
    }
  }
  else if(!elementOver) for(size_t i = 0; i < size(); i++) element[i]->setElementOver(1); //mouse is over the bars!
  
  if(keepElementsInside)
  for(unsigned long i = 0; i < size(); i++)
  {
    if(element[i]->mouseGrabbed(element[i]->getMouseStateForContainer())) putInside(i);
  }
  
  for(unsigned long i = 0; i < size(); i++)
  {
    element[i]->handle();
  }
  
  bars.handle();
  int scrollx = x0 - int(bars.hbar.scrollPos); //the scrollpos is 0 if a bar is not enabled
  int scrolly = y0 - int(bars.vbar.scrollPos);
  moveAreaTo(scrollx, scrolly);
}

void Container::moveAreaTo(int x, int y)
{
  for(unsigned long i = 0; i < size(); i++) element[i]->move(x - area.getX0(), y - area.getY0());
  area.moveTo(x, y);
}

void Container::drawWidget() const
{
  setSmallestScissor(getVisibleX0(), getVisibleY0(), getVisibleX1(), getVisibleY1()); //currently does same as setScissor (because otherwise there's weird bug, to reproduce: resize the red window and look at smiley in the small grey window), so elements from container in container are still drawn outside container. DEBUG THIS ASAP!!!
  
  for(unsigned long i = 0; i < size(); i++)
  {
    if(!element[i]->isNotDrawnByContainer())
    {
      element[i]->draw();
    }
  }
  
  resetScissor();
  
  bars.draw();
}

unsigned long Container::size() const
{
  return element.size();
}

void Container::remove(Element* element)
{
  for(unsigned long i = 0; i < size(); i++)
  {
    if(element == this->element[i])
    {
      this->element.erase(this->element.begin() + i);
    }
  }
}

void Container::bringToTop(Element* element) //precondition: element must already be in the list
{
  remove(element); //remove it from the list before putting it to the top
  this->element.push_back(element);
}

void Container::pushTop(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  pushTopAt(element, element->getX0() - x0, element->getY0() - y0, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::pushBottom(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  pushBottomAt(element, element->getX0() - x0, element->getY0() - y0, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::insert(size_t pos, Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  insertAt(pos, element, element->getX0() - x0, element->getY0() - y0, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::pushTopRelative(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  pushTopAt(element, element->getX0(), element->getY0(), leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::pushBottomRelative(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  pushBottomAt(element, element->getX0(), element->getY0(), leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::insertRelative(size_t pos, Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  insertAt(pos, element, element->getX0(), element->getY0(), leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::pushTopAt(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  remove(element); //prevent duplicates
  this->element.push_back(element);
  initElement(element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::pushBottomAt(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  remove(element); //prevent duplicates
  this->element.insert(this->element.begin(), element);
  initElement(element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::insertAt(size_t pos, Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  remove(element); //prevent duplicates
  this->element.insert(this->element.begin() + pos, element);
  initElement(element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Container::centerElement(Element* element)
{
  element->moveCenterTo(getCenterx(), getCentery());
}

void Container::initElement(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  element->moveTo(x0 + x, y0 + y);
  
  //sticky factors for resizing
  element->setSticky(leftSticky, topSticky, rightSticky, bottomSticky);
    
  //remembering coordinates for proper resizing
  element->saveInitialPosition(area.getX0(), area.getY0(), area.getX1(), area.getY1());
}

bool Container::isContainer() const
{
  return 1;
}

void Container::clear()
{
  element.clear();
}

void Container::putInside(unsigned long i)
{
  if(/*i >= 0 &&*/ i < size())
  {
    int ex0 = element[i]->getX0();
    int ey0 = element[i]->getY0();
    int ex1 = element[i]->getX1();
    int ey1 = element[i]->getY1();
    int esx = ex1 - ex0;
    int esy = ey1 - ey0;
    
    int newx = ex0, newy = ey0;
    
    if(ex0 < area.getX0()) newx = area.getX0();
    if(ey0 < area.getY0()) newy = area.getY0();
    if(ex1 > area.getX1()) newx = area.getX1() - esx;
    if(ey1 > area.getY1()) newy = area.getY1() - esy;
    
    //if the size of the element is too large to fit in the window, there's no reason to move it (or it'll warp all the time)
    if(element[i]->getSizex() > area.getSizex()) newx = element[i]->getX0();
    if(element[i]->getSizey() > area.getSizey()) newy = element[i]->getY0();
    
    element[i]->moveTo(newx, newy);
  }
}

void Container::moveWidget(int x, int y)
{
  for(unsigned long i = 0; i < size(); i++) element[i]->move(x, y);
  bars.move(x, y);
  area.move(x, y);
}

void Container::getRelativeElementPos(Element& element, int& ex, int& ey) const
{
  ex = element.getX0() - x0;
  ey = element.getY0() - y0;
}

void Container::resizeWidget()
{
  bars.resizeSticky(x0, y0, x1, y1);
  area.resizeSticky(x0, y0, x1, y1);
  updateBars(); //if this is done at the wrong moment, the bars may appear after resizing the container at times where it isn't desired

  for(unsigned long i = 0; i < size(); i++)
  {
    element[i]->resizeSticky(area.getX0(), area.getY0(), area.getX1(), area.getY1());
  }
}

void Container::saveInitialPositions()
{
  for(unsigned long i = 0; i < size(); i++)
  {
    element[i]->saveInitialPosition(area.getX0(), area.getY0(), area.getX1(), area.getY1());
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUIGROUP//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Group::mouseOver() const
{
  if(!present) return false;
  if(elementOver) return false; //there is another element over this element, so says the container containing this element
  
  for(int i = size() - 1; i >= 0; i--)
  {
    element[i]->setElementOver(0);
    if(element[i]->mouseOver()) return true;
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////
//GUIPANEL//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Panel functions.
Panel is a class containing a backpanel, but it also shares the Element functions
*/

Panel::Panel()
{
  this->panel.colorMod = RGB_White;
  this->panel.fillColor = RGB_White;
  this->panel.enableSides = 0;
  this->panel.enableCenter = 0;
  this->panel.t00 = &emptyTexture;
  this->panel.t01 = &emptyTexture;
  this->panel.t02 = &emptyTexture;
  this->panel.t10 = &emptyTexture;
  this->panel.t11 = &emptyTexture;
  this->panel.t12 = &emptyTexture;
  this->panel.t20 = &emptyTexture;
  this->panel.t21 = &emptyTexture;
  this->panel.t22 = &emptyTexture;
}

void Panel::makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  
  this->panel.makeUntextured(fillColor);
}

void Panel::makeTextured(int x, int y, int sizex, int sizey,
       const Texture* t00, const ColorRGB& colorMod, int shape)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->shape = shape;
  
  this->panel.makeTextured(t00, colorMod);
}

void Panel::make(int x, int y, int sizex, int sizey,
       const GuiSet* set, int shape)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->shape = shape;
  
  this->panel = *set->windowPanel;
}

//this function should become obsolete after there is a general "moveTo, resize" combo function which is planned to be made
void Panel::setSize(int x, int y, int sizex, int sizey)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
}

void Panel::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUILINE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Rule is a class containing a BackLine, but it also shares the Element functions
*/

Rule::Rule()
{
  this->line.colorMod = RGB_White;
  this->line.enableSides = 0;
  this->line.t0 = &emptyTexture;
  this->line.t1 = &emptyTexture;
  this->line.t2 = &emptyTexture;
}

void Rule::makeHorizontal(int x, int y, int length, const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(set->hline[0]->getV());
  this->totallyEnable();
  this->shape = 0;
  
  this->line.makeHorizontal1(set->hline[0], set->mainColor);
}

void Rule::makeHorizontal1(int x, int y, int length, Texture* t0, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(t0->getV());
  this->totallyEnable();
  this->shape = 0;
  
  this->line.makeHorizontal1(t0, colorMod);
}

void Rule::makeVertical(int x, int y, int length, const GuiSet* set)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(set->vline[0]->getU());
  this->totallyEnable();
  this->shape = 0;
  
  this->line.makeVertical1(set->vline[0], set->mainColor);
}

void Rule::makeVertical1(int x, int y, int length, Texture* t0, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(t0->getU());
  this->totallyEnable();
  this->shape = 0;
  
  this->line.makeVertical1(t0, colorMod);
}

void Rule::setSize(int x, int y, int length)
{
  this->x0 = x;
  this->y0 = y;
  if(line.direction == H) this->setSizex(length);
  else this->setSizey(length);
}

void Rule::drawWidget() const
{
  if(line.direction == H) line.draw(x0, y0, getSizex());
  else line.draw(x0, y0, getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUIWINDOW/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Window is a window that contains pointers to any kind of other Elements.

When the window gets drawn, all Elements get drawn as well.

When the window moves, all Elements are moved as well.

You can still operate the Elements like you normally do.

The window also contains a Panel for it's own background, and will have a bar
at the top that allows you to drag the window if you click it with the mouse.

Things like the drawing order of the window, or detecting if you click on the
window and not something behind it, you'll have to do yourself...
*/

Window::Window()
{
  this->active = 0;
  this->visible = 0;
  this->enableResizer = 0;
  this->closed = 0;
  this->resizerOverContainer = 0;
}

int Window::getRelContentStart() const
{
  int result = 0;
  if(enableTop) result = top.getY1() - y0;
  
  return result;
}

void Window::addCloseButton(int offsetX, int offsetY, const GuiSet* set)
{
  int closeX = x1 + offsetX - set->closeButton->getU();
  int closeY = y0 + offsetY;
  closeButton.makeImage(closeX, closeY, set->closeButton, set->closeButton, set->closeButton, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  closed = 0;
  closeEnabled = 1;
  
  closeButton.setSticky(1.0, 0.0, 1.0, 0.0);
  closeButton.saveInitialPosition(x0, y0, x1, y1);
}

void Window::addResizer(const GuiSet* set, bool overContainer, int offsetX, int offsetY)
{
  int resizerX = x1 - offsetX - set->resizer->getU();
  int resizerY = y1 - offsetY - set->resizer->getV();
  resizer.makeImage(resizerX, resizerY, set->resizer, set->resizer, set->resizer, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  enableResizer = true;
  
  resizer.setSticky(1.0, 1.0, 1.0, 1.0);
  resizer.saveInitialPosition(x0, y0, x1, y1);
  
  if(!overContainer)
  {
    container.resize(container.getX0(), container.getY0(), container.getX1(), container.getY1() - (y1 - resizer.getY0()));
    container.saveInitialPosition(x0, y0, x1, y1);
  }
  
  resizerOverContainer = overContainer;
}

//returns the lowest position the container bottom side is allowed to have (for example not over resizer if resizerOverContainer is false
int Window::getContainerLowest() const
{
  int pos = y1;
  if(enableResizer && !resizerOverContainer) pos = resizer.getY0();
  
  return pos;
}
//returns the highest position the container top side is allowed to have 
int Window::getContainerHighest() const
{
  int pos = y0;
  if(enableTop) pos = top.getY1();
  
  return pos;
}
//returns the leftmost position the container left side is allowed to have 
int Window::getContainerLeftmost() const
{
  return x0;
}
//returns the rightmost position the container right side is allowed to have 
int Window::getContainerRightmost() const
{
  return x1;
}

void Window::setContainerBorders(int left, int up, int right, int down)
{
  if(up < 0) up = left;
  if(right < 0) right = left;
  if(down < 0) down = left;
  
  container.resize(getContainerLeftmost() + left, getContainerHighest() + up, getContainerRightmost() - right, getContainerLowest() - down);
  container.saveInitialPosition(x0, y0, x1, y1);
}

bool Window::isContainer() const
{
  return 1;
}

void Window::makeUntextured(int x, int y, int sizex, int sizey, const ColorRGB& fillColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->enableTop = 0;
  
  this->panel.makeUntextured(fillColor);
  
  initContainer();
}

void Window::makeTextured(int x, int y, int sizex, int sizey,
       const Texture* t00, const ColorRGB& colorMod, int shape)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->shape = shape;
  
  this->enableTop = 0;
  
  this->panel.makeTextured(t00, colorMod);
  
  initContainer();
}

void Window::make(int x, int y, int sizex, int sizey,
       const GuiSet* set, int shape)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->totallyEnable();
  this->shape = shape;
  
  this->enableTop = 0;
  
  this->panel = *set->windowPanel;
  
  initContainer();
}

void Window::addTop(Texture* t0, int offsetLeft, int offsetRight, int offsetTop, const ColorRGB& colorMod)
{
  top.makeHorizontal1(x0 + offsetLeft, y0 + offsetTop, getSizex() - offsetLeft - offsetRight, t0, colorMod);
  this->enableTop = 1;
  
  top.setSticky(0.0, 0.0, 1.0, 0.0);
  top.saveInitialPosition(x0, y0, x1, y1);
  
  container.resize(container.getX0(), container.getY0() + (top.getY1() - y0), container.getX1(), container.getY1());
  container.saveInitialPosition(x0, y0, x1, y1);
}


void Window::initContainer()
{
  container.make(x0, y0, getSizex(), getSizey());
  container.setSticky(0.0, 0.0, 1.0, 1.0);
  container.saveInitialPosition(x0, y0, x1, y1);
  container.setKeepElementsInside(true);
}

//to let the scrollbars work properly, call this AFTER using setContainerBorders, addTop, addResizer and such of the window
void Window::addScrollbars(int areax, int areay, int areasizex, int areasizey, double areaStickyLeft, double areaStickyTop, double areaStickyRight, double areaStickyBottom)
{
  container.make(container.getX0(), container.getY0(), container.getSizex(), container.getSizey(), areax, areay, areasizex, areasizey, areaStickyLeft, areaStickyTop, areaStickyRight, areaStickyBottom);
}

//this function could be obsolete once there's the resize function
void Window::setSize(int x, int y, int sizex, int sizey)
{
  x0 = x;
  y0 = y;
  setSizex(sizex);
  setSizey(sizey);
}

void Window::putInside(int i)
{
  container.putInside(i);
}


void Window::handleWidget()
{
  //the close button
  if(closeEnabled && closeButton.clicked()) closed = 1;
  
  container.handle();
  
  //resize window
  if(enableResizer && resizer.mouseGrabbed()) resize(x0, y0, globalMouseX - resizer.mouseGetRelGrabX() + (x1 - resizer.getX0()), globalMouseY - resizer.mouseGetRelGrabY() + (y1 - resizer.getY0()));

  //drag window
  if(!enableTop && mouseGrabbed()) moveTo(globalMouseX - mouseGetRelGrabX(), globalMouseY - mouseGetRelGrabY()); //um this means a window without top can always be dragged?? maybe I should turn this off? in case you want a non moveable window...
  if(enableTop && top.mouseGrabbed())
  {
    moveTo(globalMouseX - top.mouseGetRelGrabX() - (top.getX0() - getX0()), globalMouseY - top.mouseGetRelGrabY() - (top.getY0() - getY0()));
  }
  
  //the scrollbar's conserveCorner should be the same as this window's resizerOverContainer
  container.bars.conserveCorner = resizerOverContainer;
  
}

void Window::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());
  if(enableTop)
  {
    top.draw(); //draw top bar before the elements, or it'll appear above windows relative to the current window
    print(title, top.getX0() + titleX, top.getY0() + titleY, titleMarkup);
  }
  
  if(closeEnabled) closeButton.draw();

  container.draw();

  if(enableResizer) resizer.draw(); //draw this after the container so the resizer is drawn over scrollbars if that is needed
}

void Window::addTitle(const std::string& title, int titleX, int titleY, const Markup& titleMarkup)
{
  this->title = title;
  this->titleX = titleX;
  this->titleY = titleY;
  this->titleMarkup = titleMarkup;
}

void Window::setTitle(const std::string& title)
{
  this->title = title;
}

unsigned long Window::size() const
{
  return container.size();
}

void Window::remove(Element* element)
{
  container.remove(element);
}

void Window::bringToTop(Element* element) //precondition: element must already be in the list
{
  container.bringToTop(element);
}

void Window::pushTopAt(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushTopAt(element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::pushBottomAt(Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushBottomAt(element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::insertAt(size_t pos, Element* element, int x, int y, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.insertAt(pos, element, x, y, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::pushTop(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushTop(element, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::pushBottom(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushBottom(element, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::insert(size_t pos, Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.insert(pos, element, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::pushTopRelative(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushTopRelative(element, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::pushBottomRelative(Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.pushBottomRelative(element, leftSticky, topSticky, rightSticky, bottomSticky);
}

void Window::insertRelative(size_t pos, Element* element, double leftSticky, double topSticky, double rightSticky, double bottomSticky)
{
  container.insertRelative(pos, element, leftSticky, topSticky, rightSticky, bottomSticky);
}

Element* Window::getAutoSubElement(unsigned long i)
{
  switch(i)
  {
    case 0: return &top;
    case 1: return &resizer;
    case 2: return &container;
    case 3: return &closeButton;
    default: return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUIBUTTON/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Button functions.
Button is a class for a button supporting different graphics on mouseover,
and that can return if the mouse is over the button, the mouse is down, or the
mouse was down and is released again.
*/

//Empty Constructor
Button::Button()
{
  this->enableImage = 0;
  this->enableImage2 = 0;
  this->enableText = 0;
  this->enablePanel = 0;

  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = "";
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = TS_Black;
  this->markup[1] = TS_Black;
  this->markup[2] = TS_Black;
  
  /*this->panel1.t00 = &emptyTexture;
  this->panel1.t01 = &emptyTexture;
  this->panel1.t02 = &emptyTexture;
  this->panel1.t10 = &emptyTexture;
  this->panel1.t11 = &emptyTexture;
  this->panel1.t12 = &emptyTexture;
  this->panel1.t20 = &emptyTexture;
  this->panel1.t21 = &emptyTexture;
  this->panel1.t22 = &emptyTexture;
  this->panel2.t00 = &emptyTexture;
  this->panel2.t01 = &emptyTexture;
  this->panel2.t02 = &emptyTexture;
  this->panel2.t10 = &emptyTexture;
  this->panel2.t11 = &emptyTexture;
  this->panel2.t12 = &emptyTexture;
  this->panel2.t20 = &emptyTexture;
  this->panel2.t21 = &emptyTexture;
  this->panel2.t22 = &emptyTexture;  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;
  this->panel1.colorMod = RGB_Black;
  this->panel2.colorMod = RGB_Black;
  this->panel1.enableSides = 0;
  this->panel2.enableSides = 0;
  this->panel1.enableCenter = 0;
  this->panel2.enableCenter = 0;*/
  
  this->active = 0;
  this->visible = 0;
  this->doubleClickTime = 500;
  
  this->shape = 0;
  this->mouseDownVisualStyle = 0;
}

//Full Constructor
void Button::make
(
  int x, int y, int sizex, int sizey, //basic properties
  bool enableImage, Texture* texture1, Texture* texture2, Texture* texture3, int imageOffsetx, int imageOffsety, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, //image
  bool enableText, const std::string& text, int textOffsetx, int textOffsety, const Markup& markup1, const Markup& markup2, const Markup& markup3, //text
  bool enablePanel, const BackPanel* panel1, const BackPanel* panel2, const BackPanel* panel3, int panelOffsetx, int panelOffsety, //panel
  int shape //special options
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  this->enableImage = enableImage;
  this->enableText = enableText;
  this->enablePanel = enablePanel;

  this->image[0] = texture1;
  this->image[1] = texture2;
  this->image[2] = texture3;
  this->imageOffsetx = imageOffsetx;
  this->imageOffsety = imageOffsety;
  this->imageColor[0] = imageColor1;
  this->imageColor[1] = imageColor2;
  this->imageColor[2] = imageColor3;
  
  this->text = text;
  this->textOffsetx = textOffsetx;
  this->textOffsety = textOffsety;
  this->markup[0] = markup1;
  this->markup[1] = markup2;
  this->markup[2] = markup3;
  
  this->panel[0] = panel1;
  this->panel[1] = panel2;
  this->panel[2] = panel3;
  this->panelOffsetx = panelOffsetx;
  this->panelOffsety = panelOffsety;
  
  /*if(panelTexture1 != &emptyTexture)
  {
    this->panel1.t00 = &panelTexture1[0]; //+ 0*sizeof(Texture);
    this->panel1.t01 = &panelTexture1[1]; //+ 1*sizeof(Texture);
    this->panel1.t02 = &panelTexture1[2]; //+ 2*sizeof(Texture);
    this->panel1.t10 = &panelTexture1[3]; //+ 3*sizeof(Texture);
    this->panel1.t11 = &panelTexture1[4]; //+ 4*sizeof(Texture);
    this->panel1.t12 = &panelTexture1[5]; //+ 5*sizeof(Texture);
    this->panel1.t20 = &panelTexture1[6]; //+ 6*sizeof(Texture);
    this->panel1.t21 = &panelTexture1[7]; //+ 7*sizeof(Texture);
    this->panel1.t22 = &panelTexture1[8]; //+ 8*sizeof(Texture);
  }
  if(panelTexture2 != &emptyTexture)
  {
    this->panel2.t00 = &panelTexture2[0]; //+ 0*sizeof(Texture);
    this->panel2.t01 = &panelTexture2[1]; //+ 1*sizeof(Texture);
    this->panel2.t02 = &panelTexture2[2]; //+ 2*sizeof(Texture);
    this->panel2.t10 = &panelTexture2[3]; //+ 3*sizeof(Texture);
    this->panel2.t11 = &panelTexture2[4]; //+ 4*sizeof(Texture);
    this->panel2.t12 = &panelTexture2[5]; //+ 5*sizeof(Texture);
    this->panel2.t20 = &panelTexture2[6]; //+ 6*sizeof(Texture);
    this->panel2.t21 = &panelTexture2[7]; //+ 7*sizeof(Texture);
    this->panel2.t22 = &panelTexture2[8]; //+ 8*sizeof(Texture);
  }

  this->panelOffsetx = panelOffsetx;
  this->panelOffsety = panelOffsety;
  this->panel1.colorMod = panelColor1;
  this->panel2.colorMod = panelColor2;
  this->panel1.enableSides = 1;
  this->panel2.enableSides = 1;
  this->panel1.enableCenter = 1;
  this->panel2.enableCenter = 1;*/
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->shape = shape;
  this->mouseDownVisualStyle = 0;
}

void Button::addFrontImage
(
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3
)
{
  this->enableImage2 = 1;
  
  this->image2[0] = texture1;
  this->image2[1] = texture2;
  this->image2[2] = texture3;
  this->imageOffsetx2 = 0;
  this->imageOffsety2 = 0;
  this->imageColor2[0] = imageColor1;
  this->imageColor2[1] = imageColor2;
  this->imageColor2[2] = imageColor3;
}

void Button::addFrontImage(const Texture* texture)
{
  addFrontImage(texture, texture, texture, imageColor[0], imageColor[1], imageColor[2]);
}

//Constructor to make a button with only images, no text
void Button::makeImage
(
  int x, int y, //basic properties
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, //image
  int shape //special options
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(texture1->getU());
  this->setSizey(texture1->getV());
  
  this->enableImage = 1;
  this->enableText = 0;
  this->enablePanel = 0;
  
  this->image[0] = texture1;
  this->image[1] = texture2;
  this->image[2] = texture3;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = imageColor1;
  this->imageColor[1] = imageColor2;
  this->imageColor[2] = imageColor3;
  
  this->text = "";
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = TS_Black;
  this->markup[1] = TS_Black;
  this->markup[2] = TS_Black;
  
  /*this->panel1.t00 = &emptyTexture;
  this->panel1.t01 = &emptyTexture;
  this->panel1.t02 = &emptyTexture;
  this->panel1.t10 = &emptyTexture;
  this->panel1.t11 = &emptyTexture;
  this->panel1.t12 = &emptyTexture;
  this->panel1.t20 = &emptyTexture;
  this->panel1.t21 = &emptyTexture;
  this->panel1.t22 = &emptyTexture;
  this->panel2.t00 = &emptyTexture;
  this->panel2.t01 = &emptyTexture;
  this->panel2.t02 = &emptyTexture;
  this->panel2.t10 = &emptyTexture;
  this->panel2.t11 = &emptyTexture;
  this->panel2.t12 = &emptyTexture;
  this->panel2.t20 = &emptyTexture;
  this->panel2.t21 = &emptyTexture;
  this->panel2.t22 = &emptyTexture;  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;
  this->panel1.colorMod = RGB_Black;
  this->panel2.colorMod = RGB_Black;
  this->panel1.enableSides = 0;
  this->panel2.enableSides = 0;
  this->panel1.enableCenter = 0;
  this->panel2.enableCenter = 0;*/
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->shape = shape;
  this->mouseDownVisualStyle = 0;
}

//Constructor to make a button with only text, no images
void Button::makeText
(
  int x, int y, //basic properties
  const std::string& text, //text
  const GuiSet* set,
  int shape //special options
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(0);
  this->setSizey(0);
  
  this->enableImage = 0;
  this->enableText = 1;
  this->enablePanel = 0;

  
  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = text;
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = set->textButtonMarkup[0];
  this->markup[1] = set->textButtonMarkup[1];
  this->markup[2] = set->textButtonMarkup[2];
  
  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->shape = shape;
  this->mouseDownVisualStyle = 0;
  
  autoTextSize();
}

//constructor for button with panel and text, auto generated text offset
void Button::makeTextPanel
(
  int x, int y, const std::string& text, int sizex, int sizey, //basic properties + text
  const GuiSet* set, //panel
  int shape //special options
)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  
  this->enableImage = 0;
  this->enableText = 1;
  this->enablePanel = 1;
  
  this->image[0] = 0;
  this->image[1] = 0;
  this->image[2] = 0;
  this->imageOffsetx = 0;
  this->imageOffsety = 0;
  this->imageColor[0] = RGB_Black;
  this->imageColor[1] = RGB_Black;
  this->imageColor[2] = RGB_Black;
  
  this->text = text;
  this->textOffsetx = 0;
  this->textOffsety = 0;
  this->markup[0] = set->panelButtonMarkup[0];
  this->markup[1] = set->panelButtonMarkup[0];
  this->markup[2] = set->panelButtonMarkup[0];
  
  this->panel[0] = set->buttonPanel;
  this->panel[1] = set->buttonOverPanel;
  this->panel[2] = set->buttonDownPanel;
  
  this->panelOffsetx = 0;
  this->panelOffsety = 0;

  this->totallyEnable();
  this->doubleClickTime = 500;
  
  this->shape = shape;
  this->mouseDownVisualStyle = 1;
  
   
  //put text in the center
  
  centerText();
}

//Draws the button, also checks for mouseover and mousedown to draw mouseover graphic
void Button::drawWidget() const
{
  int i = 0; //0 = normal, 1 = mouseOver, 2 = mouseDown
  
  if((mouseDownVisualStyle == 0 && mouseDown(LMB/*, 15*/))
  || (mouseDownVisualStyle == 1 && mouseDownHere(mutable_button_drawing_mouse_test))
  || (mouseDownVisualStyle == 2 && mouseGrabbed(mutable_button_drawing_mouse_test)))
    i = 2;
  else if(mouseOver()) i = 1;
  
  int textDownOffset = 0;
  if(i == 2) textDownOffset = 1; //on mouseDown, the text goes down a bit too
  
  if(enablePanel) panel[i]->draw(x0 + panelOffsetx, y0 + panelOffsety, getSizex(), getSizey());
  if(enableImage) image[i]->draw(x0 + imageOffsetx, y0 + imageOffsety, imageColor[i]);
  if(enableImage2) image2[i]->draw(x0 + imageOffsetx2, y0 + imageOffsety2, imageColor2[i]);
  if(enableText) printFormatted(text, x0 + textOffsetx + textDownOffset, y0 + textOffsety + textDownOffset, markup[i]);
  
  //NOTE: ik heb de print functies nu veranderd naar printFormatted! Zo kan je gekleurde texten enzo printen met # codes
}

/*This function automaticly changes sizex and sizey of the button so that the
size of the button matches the size of the text, so that it detects the mouse
as being in the button when the mouse is over the text*/
void Button::autoTextSize(int extrasize)
{
  setSizex(text.length() * markup[0].getWidth());
  setSizey(markup[0].getHeight());
  x1 += 2 * extrasize;
  y1 += 2 * extrasize;
  x0 -= extrasize;
  y0 -= extrasize;
  textOffsetx += extrasize;
  textOffsety += extrasize;
}

/*Place the text in the center of the button without having to calculate
these positions by hand*/
void Button::centerText()
{
  textOffsetx = (getSizex() / 2) - text.length() * (markup[0].getWidth() / 2); //4 = fontwidth / 2
  textOffsety = (getSizey() / 2) - (markup[0].getHeight() / 2);
}



void  Button::makeImage(int x, int y,  const Texture* texture123, const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3,  int shape)
{
  makeImage(x, y, texture123, texture123, texture123, imageColor1, imageColor2, imageColor3, shape);
}

////////////////////////////////////////////////////////////////////////////////
//GUISCROLLBAR//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Scrollbar Class Functions
Scrollbar is a scrollbar that can be used for lists of things that are too
large to fit on screen so you have to scroll through them, for example useful
for Console.

It has a length on screen (sizey) and a length of the scroller (scrollSize), in
pixels, and you can scroll up or down with the mouse.

See gui.h for meanings of all the members of ScrollBar
*/

int Scrollbar::getSliderSize() const
{
  if(direction == V)
  {
    return getSizey() - txUp->getV() - txDown->getV() - txScroller->getV(); //scroller length also subtracted from it: the scroller can only move over this distance since it's full width must always be inside the slider length
  }
  else
  {
    return getSizex() - txUp->getU() - txDown->getU() - txScroller->getU();
  }
}

int Scrollbar::getSliderStart() const
{
  if(direction == V)
  {
    return txUp->getV();
  }
  else
  {
    return txUp->getU();
  }
}

int Scrollbar::getSliderEnd() const
{
  if(direction == V)
  {
    return getSizey() - txDown->getV();
  }
  else
  {
    return getSizex() - txDown->getU();
  }
}

void Scrollbar::init()
{
  if(scrollSize == 0) scrollSize = 1;

  if(direction == V)
  {
    buttonUp.makeImage(x0, y0, txUp, txUpOver, txUpOver, colorMod, colorModOver, colorModDown);
    buttonDown.makeImage(x0, y1 - txDown->getV(), txDown, txDownOver, txDownOver, colorMod, colorModOver, colorModDown);
    scroller.makeImage(x0, int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), txScroller, txScrollerOver, txScrollerOver, colorMod, colorModOver, colorModDown);
    
    buttonUp.setSticky(0, 0, 0, 0);
    buttonDown.setSticky(0, 1, 0, 1);
    scroller.setSticky(0, -2, 0, -2);
    buttonDown.saveInitialPosition(x0, y0, x1, y1);
    buttonUp.saveInitialPosition(x0, y0, x1, y1);
    scroller.saveInitialPosition(x0, y0, x1, y1);

  }
  else
  {
    buttonUp.makeImage(x0, y0, txUp, txUpOver, txUpOver, colorMod, colorModOver, colorModDown);
    buttonDown.makeImage(x1 - txDown->getU(), y0, txDown, txDownOver, txDownOver, colorMod, colorModOver, colorModDown);
    scroller.makeImage(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), y0, txScroller, txScrollerOver, txScrollerOver, colorMod, colorModOver, colorModDown);
    
    buttonUp.setSticky(0, 0, 0, 0);
    buttonDown.setSticky(1, 0, 1, 0);
    scroller.setSticky(-2, 0, -2, 0);
    buttonDown.saveInitialPosition(x0, y0, x1, y1);
    buttonUp.saveInitialPosition(x0, y0, x1, y1);
    scroller.saveInitialPosition(x0, y0, x1, y1);
  }
  
  buttonUp.mouseDownVisualStyle = 1;
  buttonDown.mouseDownVisualStyle = 1;
  scroller.mouseDownVisualStyle = 2;
}

Scrollbar::Scrollbar()
{
  this->scrollSize = 0;
  this->scrollPos = 0;
  this->scrollSpeed = 0;
  this->txUp = &emptyTexture;
  this->txDown = &emptyTexture;
  this->txScroller = &emptyTexture;
  this->txBack = &emptyTexture;
  this->txUpOver = &emptyTexture;
  this->txDownOver = &emptyTexture;
  this->txScrollerOver = &emptyTexture;
  this->colorMod = RGB_White;
  this->colorModOver = RGB_White;
  this->colorModDown = RGB_White;
  this->absoluteSpeed = 0;
  this->speedMode = 0;
  this->enableValue = 0;
}

void Scrollbar::showValue(int x, int y, const Markup& valueMarkup, int type)
{
  this->valueMarkup = valueMarkup;
  this->valueX = x;
  this->valueY = y;
  this->enableValue = type;
}

void Scrollbar::makeVertical(int x, int y, int length,
                double scrollSize, double scrollPos, double offset, double scrollSpeed,
                const GuiSet* set, int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = V;
  this->setSizex(set->scroller->getU());
  this->setSizey(length);
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->txUp = set->arrowN;
  this->txDown = set->arrowS;
  this->txScroller = set->scroller;
  this->txBack = set->scrollbarBackground;
  this->txUpOver = set->arrowN;
  this->txDownOver = set->arrowS;
  this->txScrollerOver = set->scroller;
  this->colorMod = set->mainColor;
  this->colorModOver = set->mouseOverColor;
  this->colorModDown = set->mouseDownColor;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->totallyEnable();
  
  init();
  
  if(speedMode == 1) setRelativeScrollSpeed();
}

void Scrollbar::makeHorizontal(int x, int y, int length,
                  double scrollSize, double scrollPos, double offset, double scrollSpeed,
                  const GuiSet* set, int speedMode)
{
  this->x0 = x;
  this->y0 = y;
  this->direction = H;
  this->setSizex(length);
  this->setSizey(set->scroller->getV());
  this->scrollSize = scrollSize;
  this->scrollPos = scrollPos;
  this->scrollSpeed = scrollSpeed;
  this->offset = offset;
  
  this->txUp = set->arrowW;
  this->txDown = set->arrowE;
  this->txScroller = set->scroller;;
  this->txBack = set->scrollbarBackground;
  this->txUpOver = set->arrowW;
  this->txDownOver = set->arrowE;
  this->txScrollerOver = set->scroller;;
  this->colorMod = set->mainColor;
  this->colorModOver = set->mouseOverColor;
  this->colorModDown = set->mouseDownColor;
  
  this->absoluteSpeed = scrollSpeed;
  this->speedMode = speedMode;
  
  this->totallyEnable();
  
  init();
  
  if(speedMode == 1) setRelativeScrollSpeed();
}

double Scrollbar::getValue() const
{
  return offset + scrollPos;
}

void Scrollbar::setValue(double value)
{
  scrollPos = value - offset;
}

void Scrollbar::randomize()
{
  double r = getRandom();
  scrollPos = r * scrollSize;
}

void Scrollbar::handleWidget()
{
  int scrollDir = 0;

  if(buttonUp.mouseDownHere()) scrollDir = -1;
  if(buttonDown.mouseDownHere()) scrollDir = 1;

  if(direction == V)
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed();
    bool buttonDown_grabbed = buttonDown.mouseGrabbed();
    if(scroller.mouseGrabbed())
      scrollPos = (scrollSize * (globalMouseY - y0 - getSliderStart() - txScroller->getV() / 2)) / getSliderSize();
    else if(mouseDownHere() && !scroller.mouseGrabbed() && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (globalMouseY - y0 - getSliderStart() - txScroller->getV() / 2)) / getSliderSize();
      scroller.mouseGrab();
    }
    if(mouseScrollUp()) scrollDir = -2;
    if(mouseScrollDown()) scrollDir = 2;
  }
  else
  {
    //grabbed must be checked every frame to work properly, not from inside nested ifs
    bool buttonUp_grabbed = buttonUp.mouseGrabbed();
    bool buttonDown_grabbed = buttonDown.mouseGrabbed();
    if(scroller.mouseGrabbed())
      scrollPos = (scrollSize * (globalMouseX - x0 - getSliderStart() - txScroller->getU() / 2)) / getSliderSize();
    else if(mouseDownHere() && !scroller.mouseGrabbed() && !buttonUp_grabbed && !buttonDown_grabbed)
    {
      scrollPos = (scrollSize * (globalMouseX - x0 - getSliderStart() - txScroller->getU() / 2)) / getSliderSize();
      scroller.mouseGrab();
    }
  }
  
  if(scrollDir != 0)
  {
    scrollPos += scrollDir * absoluteSpeed * ((getSeconds() - oldTime));
  }
  oldTime = getSeconds();

  if(scrollPos < 0) scrollPos = 0;
  if(scrollPos > scrollSize) scrollPos = scrollSize;
  
  if(speedMode == 1) setRelativeScrollSpeed();
  
  if(direction == V) scroller.moveTo(scroller.getX0(), int(y0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize));
  else scroller.moveTo(int(x0 + getSliderStart() + (getSliderSize() * scrollPos) / scrollSize), scroller.getY0());
}

Element* Scrollbar::getAutoSubElement(unsigned long i)
{
  switch(i)
  {
    case 0: return &scroller;
    case 1: return &buttonUp;
    case 2: return &buttonDown;
    default: return 0;
  }
}

//from an external source, use this function only BEFORE using the handle() function or getTicks() - oldTime will be zero
void Scrollbar::scroll(int dir)
{
  scrollPos += dir * absoluteSpeed * (getTicks() - oldTime) / 1000.0;
}

void Scrollbar::drawWidget() const
{
  //in the drawRepeated functions, sizeButton is divided through two, so if the arrow buttons have a few transparent pixels, in one half of the button you see the background through, in the other half not
  if(direction == V)
  {
    txBack->drawRepeated(x0, y0 + getSliderStart(), x1, y0 + getSliderEnd(), 1.0, 1.0, colorMod);
  }
  else
  {
    txBack->drawRepeated(x0 + getSliderStart(), y0, x0 + getSliderEnd(), y1, 1.0, 1.0, colorMod);
  }
  buttonUp.draw();
  buttonDown.draw();
  if(scrollSize > 0) scroller.draw();
  
  if(enableValue == 1)
  {
    print(getValue(), x0 + valueX, y0 + valueY, valueMarkup);
  }
  else if(enableValue == 2)
  {
    print(int(getValue()), x0 + valueX, y0 + valueY, valueMarkup);
  }
}

void Scrollbar::setRelativePosition(float position)
{
  scrollPos = int(position * getSliderSize());
}

void Scrollbar::setRelativeScrollSpeed()
{
  absoluteSpeed = scrollSize / (scrollSpeed);
}

////////////////////////////////////////////////////////////////////////////////
//GUISCROLLBARPAIR//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
  a horizontal and vertical scrollbar in one, for use for areas that can be scrolled in two directions
*/

ScrollbarPair::ScrollbarPair() : venabled(false), henabled(false)
{
  this->conserveCorner = 0;
  scrollbarGuiSet = &builtInGuiSet;
}

int ScrollbarPair::getVisiblex() const
{
  if(venabled) return x1 - x0 - vbar.getSizex();
  else return x1 - x0;
}

int ScrollbarPair::getVisibley() const
{
  if(venabled) return y1 - y0 - hbar.getSizey();
  else return y1 - y0;
}

void ScrollbarPair::make(int x, int y, int sizex, int sizey, double scrollSizeH, double scrollSizeV,
        const GuiSet* set)
{
  scrollbarGuiSet = set;
  this->totallyEnable();
  this->x0 = x;
  this->y0 = y;
  this->x1 = x + sizex;
  this->y1 = y + sizey;
  this->txCorner = set->scrollBarPairCorner;
  
  vbar.makeVertical(x1 - set->arrowN->getU(), y0, sizey - set->arrowE->getV(),
          scrollSizeV, 0, 0, 1,
          set, 1);
  hbar.makeHorizontal(x0, y1 - set->arrowW->getU(), sizex - set->arrowS->getU(),
          scrollSizeH, 0, 0, 1,
          set, 1);
          
  vbar.setSticky(1, 0, 1, 1);
  hbar.setSticky(0, 1, 1, 1);
  vbar.saveInitialPosition(x0, y0, x1, y1);
  hbar.saveInitialPosition(x0, y0, x1, y1);
  
  this->venabled = 1;
  this->henabled = 1;
}

void ScrollbarPair::disableV()
{
  venabled = 0;
  
  if(henabled)
  hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex() * conserveCorner,
          hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
          scrollbarGuiSet, hbar.speedMode);
}

void ScrollbarPair::disableH()
{
  henabled = 0;
  
  if(venabled)
  vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey() * conserveCorner,
          vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
          scrollbarGuiSet, vbar.speedMode);
}

void ScrollbarPair::enableV()
{
  venabled = 1;
  
  if(henabled)
  {
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey(),
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex(),
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
  }
  else
  {
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey() * conserveCorner,
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
  }
}

void ScrollbarPair::enableH()
{
  henabled = 1;
  
  if(venabled)
  {
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex(),
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
    vbar.makeVertical(vbar.getX0(), vbar.getY0(), getSizey() - hbar.getSizey(),
              vbar.scrollSize, vbar.scrollPos, vbar.offset, vbar.scrollSpeed,
              scrollbarGuiSet, vbar.speedMode);
  }
  else
  {
    hbar.makeHorizontal(hbar.getX0(), hbar.getY0(), getSizex() - vbar.getSizex() * conserveCorner,
              hbar.scrollSize, hbar.scrollPos, hbar.offset, hbar.scrollSpeed,
              scrollbarGuiSet, hbar.speedMode);
  }
}

void ScrollbarPair::handleWidget()
{
  if(venabled) vbar.handle();
  if(henabled) hbar.handle();
}

void ScrollbarPair::drawWidget() const
{
  if(venabled) vbar.draw();
  if(henabled) hbar.draw();
  if((venabled && henabled && !conserveCorner) || ((venabled || henabled) && conserveCorner)) txCorner->draw(x1 - txCorner->getU(), y1 - txCorner->getV());
}

Element* ScrollbarPair::getAutoSubElement(unsigned long i)
{
  switch(i)
  {
    case 0: return &vbar;
    case 1: return &hbar;
    default: return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUISLIDER/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
A simplified version of the scrollbar
*/

Slider::Slider()
{
  this->visible = 0;
  this->active = 0;
}

void Slider::makeHorizontal(int x, int y, int length, double scrollSize, const GuiSet* set)
{
  this->totallyEnable();

  int sliderCenter = set->slider->getV() / 2;
  int rulerCenter = set->sliderHRule->t0->getV() / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = H;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizex(length);
  this->setSizey(set->slider->getV());
  this->ruler = set->sliderHRule;
  this->slider.makeImage(x0, y0 + centerPos - sliderCenter, set->slider, set->slider, set->slider, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  
  slider.setSticky(-2, 0, -2, 0);
  slider.saveInitialPosition(x0, y0, x1, y1);
  
  slider.mouseDownVisualStyle = 2;
}

void Slider::makeVertical(int x, int y, int length, double scrollSize, const GuiSet* set)
{
  this->totallyEnable();
  
  int sliderCenter = set->slider->getU() / 2;
  int rulerCenter = set->sliderVRule->t0->getU() / 2;
  int centerPos = sliderCenter;
  if(rulerCenter > sliderCenter) centerPos = rulerCenter;
  
  this->direction = V;
  this->scrollPos = 0;
  this->scrollSize = scrollSize;
  this->x0 = x;
  this->y0 = y;
  this->setSizey(length);
  this->setSizex(set->slider->getU());
  this->ruler = set->sliderVRule;
  this->slider.makeImage(x + centerPos - sliderCenter, y, set->slider, set->slider, set->slider, set->mainColor, set->mouseOverColor, set->mouseDownColor);
  
  slider.setSticky(0, -2, 0, -2);
  slider.saveInitialPosition(x0, y0, x1, y1);

  slider.mouseDownVisualStyle = 2;
}

//screenPos must be relative to the x or y position of the gui element!!!
double Slider::screenPosToScrollPos(int screenPos)
{
  if(direction == H)
  {
    int sliderSize = slider.image[0]->getU();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizex() - sliderSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.image[0]->getV();
    return (scrollSize * (screenPos - sliderSize / 2)) / (getSizey() - sliderSize);
  }
}

//screenPos is relative to the x or y position of the gui element!!!
int Slider::scrollPosToScreenPos(double scrollPos)
{
  if(direction == H)
  {
    int sliderSize = slider.image[0]->getU();
    return int(((getSizex() - sliderSize) * scrollPos) / scrollSize);
  }
  else //if(direction == V)
  {
    int sliderSize = slider.image[0]->getV();
    return int(((getSizey() - sliderSize) * scrollPos) / scrollSize);
  }
}

void Slider::drawWidget() const
{
  if(direction == H)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCentery();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    
    ruler->draw(x0, y0 + centerPos - rulerCenter, getSizex());
    slider.draw();
  }
  else //if(direction == V)
  {
    int rulerCenter = ruler->t0->getU() / 2;
    int centerPos = slider.getRelCenterx();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    
    ruler->draw(x0 + centerPos - rulerCenter, y0, getSizey());
    slider.draw();
  }
}

void Slider::handleWidget()
{
  if(direction == H)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCentery();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    //int sliderSize = slider.texture1->getU();
    
    if(slider.mouseGrabbed()) scrollPos = screenPosToScrollPos(mouseGetRelPosX());
    else if(mouseDownHere() && !slider.mouseOver())
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosX());
      slider.mouseGrab();
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(x0 + scrollPosToScreenPos(scrollPos), slider.getY0());
  }
  else //if(direction == V)
  {
    int rulerCenter = ruler->t0->getV() / 2;
    int centerPos = slider.getRelCenterx();
    if(rulerCenter > centerPos) centerPos = rulerCenter;
    //int sliderSize = slider.texture1->getV();
    
    if(slider.mouseGrabbed()) scrollPos = screenPosToScrollPos(mouseGetRelPosY());
    else if(mouseDownHere() && !slider.mouseOver())
    {
      scrollPos = screenPosToScrollPos(mouseGetRelPosY());
      slider.mouseGrab();
    }
    
    if(scrollPos < 0) scrollPos = 0;
    if(scrollPos > scrollSize) scrollPos = scrollSize;
    
    slider.moveTo(slider.getX0(), y0 + scrollPosToScreenPos(scrollPos));
  }
}

Element* Slider::getAutoSubElement(unsigned long i)
{
  if(i == 0) return &slider;
  else return 0;
}

double Slider::getValue() const
{
  return scrollPos;
}

void Slider::setValue(double value)
{
  scrollPos = value;
}

double Slider::getRelValue() const
{
  return scrollPos / scrollSize;
}

void Slider::setRelValue(double value)
{
  scrollPos = scrollSize * value;
}

////////////////////////////////////////////////////////////////////////////////
//DropMenu/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*
The DropMenu class.
DropMenu is a list of text buttons, each having their own text.
The DropMenu can have a panel or rectangle behind the text buttons as background.

How to use the DropMenu class:
- x and y define the position of the top left corner of the menu
- text contains the text of all buttons, separated by '|' chars.
- put two '|' chars in a row to get a separator in the menu
- draw the menu on screen with it's draw() function
- It can be used perfectly for both right click menus of in game objects, 
  as for menus at the top of the program window
*/


//Constructor to make an empty menu
DropMenu::DropMenu()
{
  this->markup1 = TS_White;
  this->markup2 = TS_White;
  
  setOptions("");
  
  autoDisable = true;
}

//Constructor to make the menu
void DropMenu::makeColored(int x, int y, const std::string& text, 
           const Markup& markup1, const Markup& markup2,
           const ColorRGB& menuColor, BackRule hrule)
{
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->panel.makeUntextured(menuColor);
  this->hrule = hrule;
  
  setOptions(text);
  putInScreen(); //reposition then does setOptions a second time to fix changed positions
}

//Constructor to make the menu
void DropMenu::make(int x, int y, const std::string& text, 
           const Markup& markup1, const Markup& markup2,
           BackPanel panel, BackRule hrule)
{
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->panel = panel;
  this->hrule = hrule;
  
  setOptions(text);
  putInScreen(); //reposition then does setOptions a second time to fix changed positions
}

void DropMenu::clearOptions()
{
  menuButton.clear();
  separator.clear();
  identity.clear();
}

/*Initialize the menu, calculate all positions, make the buttons, ...
This function is called automaticly by the constructors*/
void DropMenu::setOptions(const std::string& text)
{
  clearOptions();
  
  unsigned long maxTextSize = 0; //the size of the largest text
  
  //calculate n, the number of elements
  unsigned long pos = 0;
  unsigned long n = 0;
  while(pos < text.length())
  {
    if(text[pos] == '|') n++;
    pos++;
  }
  n++;
  
  unsigned long i = 0;
  unsigned long t = 0;

  while(i < n)
  {
    std::string buttonText = "";

    while(text[t] != '|' && t < text.length())
    {
      buttonText += text[t];
      t++;
      if(text[t] == 0) break;
    }

    separator.push_back(false);
    if(buttonText.length() == 0) separator[i] = true;
    t++;
    
    Button b;
    b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * i, buttonText);
    menuButton.push_back(b);
    
    identity.push_back(0); //no useful identity added, but it has to be increased in size to match menuButton size
    
    if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    i++;
  }
  
   
  setSizex(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2);
  setSizey(n * (markup1.getHeight() + 2) + markup1.getHeight() / 2);
}

void DropMenu::addOption(const std::string& text, int id)
{
    Button b;
    b.makeText(x0 + 2, y0 + 2 + (markup1.getHeight() + 2) * menuButton.size(), text);
    menuButton.push_back(b);
    separator.push_back(false);
    
    int separatorsize = 0;
    for(unsigned long i = 0; i < separator.size(); i++) if(separator[i]) separatorsize++;
    
    setSizey((menuButton.size() + separatorsize) * (markup1.getHeight() + 2) + markup1.getHeight() / 2);

    //int newSizex = text.length() * markup1.getWidth() + markup1.getWidth() / 2;
    int newSizex;// = text.length();
    int textheight;
    getFormattedTextSize(text, newSizex, textheight, markup1);
    newSizex +=  markup1.getWidth() / 2;
    
    if(newSizex > getSizex()) setSizex(newSizex);
    
    identity.push_back(id);
}

void DropMenu::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());

  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i]) menuButton[i].draw();
    else hrule.draw(x0 + markup1.getWidth() / 2, y0 + 2 + (markup1.getHeight() + 2) * i + markup1.getHeight() / 2, getSizex() - markup1.getWidth());
  }
}

void DropMenu::handleWidget()
{
  if(autoDisable && globalLMB && !mouseDown()) totallyDisable();
}

/*
Returns which of the options was pressed 
Note: separaters take up a number spot too

It returns:

-1 if nothing clicked
i (= 0 or a positive integer) if option i was clicked

*/

int DropMenu::check()
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked()) return i;
    }
  }
  return -1;
}

Button* DropMenu::getButton(int i)
{
  return &menuButton[i];
}

Button* DropMenu::getButton(const std::string& name)
{
  int i = 0;
  
  for(int j = 0; j < getNumButtons(); j++) if(menuButton[j].text == name) i = j;
  
  return &menuButton[i];
}


std::string DropMenu::checkText()
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked()) return menuButton[i].text;
    }
  }
  return "";
}

int DropMenu::checkIdentity()
{
  for(unsigned long i = 0; i < menuButton.size(); i++)
  {
    if(!separator[i])
    {
      if(menuButton[i].clicked()) return identity[i];
    }
  }
  return 0;
}

Element* DropMenu::getAutoSubElement(unsigned long i)
{
  if(i < menuButton.size()) return &menuButton[i];
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////
//GUIDROPLIST///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The DropList class

this class contains a list of options that can be opened or closed with a button,
the list will have a scrollbar and allow you to choose an option with the mouse
*/

//Constructor to make an empty droplist
Droplist::Droplist()
{
  this->x0 = 0;
  this->y0 = 0;
  this->markup1 = TS_White;
  this->markup2 = TS_White;
  
  init("", -1);
}

void Droplist::make(int x, int y, const std::string& text, int numVisibleOptions,
          const Markup& markup1, const Markup& markup2, const Markup& markup3,
          BackPanel topPanel, BackPanel listPanel,
          Texture* buttonTexture)
{
  this->topPanel = topPanel;
  this->listPanel = listPanel;
  this->markup1 = markup1;
  this->markup2 = markup2;
  this->markup3 = markup3;
  this->opened = 0;
  this->selected = 0;
  this->x0 = x;
  this->y0 = y;
  this->totallyEnable();
  
  int buttonWidth = buttonTexture->getU();
  int buttonHeight = buttonTexture->getV();
  
  sizeyc = buttonHeight;
  init(text, numVisibleOptions);
  sizexo += buttonWidth;
  sizexc = sizexo;
  
  this->setSizex(sizexc);
  this->setSizey(sizeyc);
  
  listButton.makeImage(x1 - buttonWidth, y0, buttonTexture, buttonTexture, buttonTexture, RGB_White, RGB_Grey, RGB_Grey, 0);


  bar.makeVertical(x1 - buttonWidth, y0 + sizeyc, sizeyo - sizeyc,
                   100, 0, 0, 1,
                   &builtInGuiSet, 1);

}

void Droplist::makeScrollbar(const GuiSet* set)
{
  bar.makeVertical(x1 - set->arrowN->getU(), y0 + sizeyc, sizeyo - sizeyc,
                   100, 0, 0, 1,
                   set, 1);
}  

void Droplist::init(const std::string& text, int numVisibleOptions)
{
  textButton.clear();
  
  unsigned long maxTextSize = 0; //the size of the largest text
  
  //calculate n, the number of elements
  unsigned long pos = 0;
  unsigned long n = 0;
  while(pos < text.length())
  {
    if(text[pos] == '|') n++;
    pos++;
  }
  n++;
  
  unsigned long i = 0;
  unsigned long t = 0;

  while(i < n)
  {
    std::string buttonText = "";

    while(text[t] != '|' && t < text.length())
    {
      buttonText += text[t];
      t++;
    }
    t++;
    
    Button b;
    b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * i, buttonText);
    textButton.push_back(b);
    
    if(buttonText.length() > maxTextSize) maxTextSize = buttonText.length();
    
    i++;
  }
  
  if(numVisibleOptions == -1) this->numVisibleOptions = numVisibleOptions = n;
  else this->numVisibleOptions = numVisibleOptions;
   
  sizexo = maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizex();
  sizeyo = sizeyc + numVisibleOptions * (markup1.getHeight() + 2) + markup1.getHeight() / 2;
  
  int buttonWidth = maxTextSize * markup1.getWidth();
  for(unsigned long i = 0; i < textButton.size(); i++)
  {
    textButton[i].setSizex(buttonWidth);
  }
}

void Droplist::addOption(const std::string& text)
{
  Button b;
  b.makeText(x0 + 2, y0 + sizeyc + 2 + (markup1.getHeight() + 2) * textButton.size(), text);
  textButton.push_back(b);
  
  unsigned long maxTextSize = b.getSizex() / markup1.getWidth();
  if(text.length() > maxTextSize) maxTextSize = text.length();
  
  int buttonWidth = maxTextSize * markup1.getWidth();
  for(unsigned long i = 0; i < textButton.size(); i++)
  {
    textButton[i].setSizex(buttonWidth);
  }
  
  setSizex(maxTextSize * markup1.getWidth() + markup1.getWidth() / 2 + bar.getSizex());
  sizexo = sizexc = getSizex();
  
  bar.moveTo(x0 + sizexo - bar.getSizex(), bar.getY0());
  listButton.moveTo(x0 + sizexc - listButton.getSizex(), listButton.getY0());
}

void Droplist::drawWidget() const
{
  topPanel.draw(x0, y0, sizexc, sizeyc);
  listButton.draw();
  print(textButton[selected].text, x0, y0 + sizeyc / 2 - markup3.getHeight() / 2, markup3);

  if(opened)
  {
    listPanel.draw(x0, y0 + sizeyc, sizexo, sizeyo - sizeyc);
    for(unsigned long i = 0; i < textButton.size(); i++) textButton[i].draw();
    bar.draw();
  }
}

int Droplist::check()
{
  return selected;
}

std::string Droplist::checkText()
{
  return textButton[selected].text;
}

void Droplist::handleWidget()
{
  if(listButton.clicked() || (clicked() && mouseGetRelPosY() < sizeyc && !listButton.mouseOver())) //behind the or is to open it also if you press the thing itself but not the button, the getMouseY() < sizeyc is necessary, without it, the code after this will not work anymore (starting from if(openen)
  {
    if(!opened)
    {
      open();
    }
    else
    {
      close();
    }
  }
  
  if(opened)
  {
    for(unsigned long i = 0; i <  textButton.size(); i++)
    {
      if(textButton[i].clicked())
      {
        close();
        selected = i;
      }
    }
    
    if(mouseScrollUp() && !bar.mouseOver()) bar.scroll(-2);
    if(mouseScrollDown() && !bar.mouseOver()) bar.scroll(2);
    bar.handle();
    scroll();
  }
}

void Droplist::scroll()
{
  int v = numVisibleOptions;
  int n = textButton.size();
  
  if(n <= v); //dan moet er ni gescrolld worden
  else
  {
    int s = int(((n - v) * bar.scrollPos) / bar.scrollSize);
    
    for(int i = 0; i < n; i++)
    {
      textButton[i].moveTo(textButton[i].getX0(), y0 + sizeyc + 2 + (markup1.getHeight() + 2) * (i - s));
      
      if(i >= s && i < s + v)
      {
        textButton[i].setVisible(1);
        textButton[i].setActive(1);
      }
      else
      {
        textButton[i].setVisible(0);
        textButton[i].setActive(0);
      }
    }
  }
}

void Droplist::open()
{
  opened = 1;
  setSizex(sizexo);
  setSizey(sizeyo);
}

void Droplist::close()
{
  opened = 0;
  setSizex(sizexc);
  setSizey(sizeyc);
}

Element* Droplist::getAutoSubElement(unsigned long i)
{
  if(i == 0) return &listButton;
  else if(i == 1) return &bar;
  else if(i < textButton.size() + 2U) return &textButton[i - 2];
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////
//GUIINPUTLINE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The InputLine Class Functions
InputLine allows the user to enter a single line of text.

How to use it:
  You can give text color styles with color1, color2, style
  When the draw() function is called, it is drawn and handled, i.e. user unicode key presses are detected, ...
  It is only handled if "active" is true, otherwise it's only drawn and it ignores user input
  Fetch the text with getText or by simply using inputLine.text
  Check if the user pressed enter, clicked on it with mouse, .... with the check() function
*/

//the default constructor
InputLine::InputLine()
{
  this->x0 = 0;
  this->y0 = 0;
  this->l = 0;
  this->markup = TS_White;
  this->type = 0;
  this->allowedChars = 0;
  this->title = "";
  this->selfActivate = 0;
  
  this->text = "";
  cursor = 0;
  entered = 0;
  
  this->keyCheckIndex = 0;
}

void InputLine::make(int x, int y, unsigned long l, const Markup& markup, int type, int allowedChars, const std::string& title, const Markup& titleMarkup, const ColorRGB& cursorColor)
{
  this->x0 = x;
  this->y0 = y;
  this->l = l;
  this->markup = markup;
  this->titleMarkup = titleMarkup;
  this->cursorColor = cursorColor;
  this->type = type;
  this->allowedChars = allowedChars;
  this->title = title;
  
  this->text = "";
  cursor = 0;
  entered = 0;
  
  if(type == 2) //if the type is "integer"
  {
    allowedChars |= 3; //set "disable text" and "disable special symbols" to true
  }
  
  this->setSizey( markup.getHeight()); //font height
  this->setSizex((title.length() + l) * markup.getWidth());
  
  this->active = 0;
  this->visible = 1;
  this->present = 1;
  
  this->selfActivate = 1; //with this true, it'll disable itself if you click outside of it, and turn on again if you click inside
}

//this function draws the line, and makes sure it gets handled too (calls handle())
void InputLine::drawWidget() const
{
  print(title, x0, y0, titleMarkup);

  int inputX = x0 + title.length() * markup.getWidth()/*(inputPos / h) % w*/;
  int inputY = y0/*inputPos % h*/;
  if(type == 0) print(text, inputX, inputY, markup);
  else if(type == 1) //password
  {
    unsigned long p = 0;
    while(p < text.length() && p < l)
    {
      drawLetter('*', inputX + p * 8, inputY, markup);
      p++;
    }
  }
  else if(type == 2) print(getInteger(), inputX, inputY, markup);
  
  //draw the cursor if active
  if(active && int((getTicks() / 1000.0) * 2) % 2 == 0)
  {
    int cursorXDraw = inputX + cursor * markup.getWidth();
    drawLine(cursorXDraw, inputY, cursorXDraw, inputY + markup.getHeight(), cursorColor);
  }
}

//return values of check():
/*
0: the text string is empty, and the rest of the things below aren't true either
&1: the text string is not empty
&2: you pressed ENTER (to enter the text string!)

normally you'll want to check if the return value is 3: then you entered a non-empty string
*/
int InputLine::check() //both check if you pressed enter, and also check letter keys pressed, backspace, etc...
{   
  int returnValue = 0;
  if(text.length() > 0) returnValue |= 1;
  if(entered) returnValue |= 2;
  
  entered = 0;
  
  return returnValue;
}

//returns true if the user entered text, i.e. if there's text in it, and the user presses enter, and it's active
int InputLine::enter()
{
  if((check() & 2) && (check() & 1) && active) return 1;
  else return 0;
}

void InputLine::handleWidget() //both check if you pressed enter, and also check letter keys pressed, backspace, etc...
{
  if(cursor >= text.length()) cursor = text.length();

  int ascii = unicodeKey(allowedChars, 500, 25, keyCheckIndex);
  if(ascii)
  {
    switch(ascii)
    {
      case 8: //backspace
        if(cursor > 0)
        {
          cursor--;
          text.erase(cursor, 1);
        }
        break;
      case 13:
        entered = 1;
        break;
      //a few certainly not meant to be printed ones
      case 0:
        break; 
      case 10: break;
      default:
        if(text.length() < l) text.insert(cursor, 1, ascii);
        if(text.length() < l || cursor == l - 1) cursor++;
        break;
    }
  }
  
  if(keyPressed(SDLK_DELETE)) text.erase(cursor, 1);
  if(keyPressed(SDLK_HOME)) cursor = 0;
  if(keyPressed(SDLK_END))
  {
    unsigned long pos = 0;
    while(text[pos] != 0 && pos < text.length()) pos++;
    cursor = pos;
  }
  if(keyPressed(SDLK_LEFT)) if(cursor > 0) cursor--;
  if(keyPressed(SDLK_RIGHT)) if(cursor < text.length()) cursor++;

  if(mouseDown())
  {
    int relMouse = globalMouseX - x0;
    relMouse -= title.length() * markup.getWidth();
    if(relMouse / markup.getWidth() < int(text.length())) cursor = relMouse / markup.getWidth();
    else cursor = text.length();
  }
}

void InputLine::clear()
{
  setText("");
  cursor = 0;
}

void InputLine::setText(const std::string& i_text)
{
  this->text = i_text;
}

const std::string& InputLine::getText() const
{
  return text;
}

void InputLine::setTitle(const std::string& i_title)
{
  int size_diff = i_title.size() - title.size();
  this->title = i_title;
  growX1(size_diff * titleMarkup.getWidth());
}

const std::string& InputLine::getTitle() const
{
  return title;
}

int InputLine::getInteger() const
{
  return std::atoi(text.c_str());
}

////////////////////////////////////////////////////////////////////////////////
//GUIINPUTBOX///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The InputBox Class Functions
InputBox  allows the user to enter a multi line text.
*/

InputBox::InputBox()
{
  this->visible = 0;
  this->active = 0;
}

void InputBox::make(int x, int y, int sizex, int sizey, int maxLines, int border, const Markup& markup, BackPanel panel, const ColorRGB& cursorColor)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->maxLines = maxLines;
  this->markup = markup;
  this->multiText.markup = markup;
  this->panel = panel;
  this->cursorColor = cursorColor;
  this->active = 0; 
  this->visible = 1;
  this->present = 1;
  this->cursor = 0;
  this->selfActivate = 0;
  this->border = border;
  this->selfActivate = 1;
  
  init();
}

void InputBox::makeScrollbar(const GuiSet* set)
{
  bar.makeVertical(x1 - set->arrowN->getU(), y0, getSizey(),
                   100, 0, 0, 1,
                   set, 1);
}

void InputBox::init()
{
  bar.makeVertical(x1 - builtInGuiSet.arrowN->getU(), y0, getSizey(), 100, 0, 0, 1);
  bar.scrollSize = 1;
  bar.scrollPos = 0;
}

int InputBox::getLeftText() const
{
  return border;
}

int InputBox::getRightText() const
{
  return getSizex() - border - bar.getSizex();
}

int InputBox::getTopText() const
{
  return border;
}

int InputBox::getBottomText() const
{
  return getSizey() - border;
}

int InputBox::getTextAreaHeight() const
{
  return getBottomText() - getTopText();
}

int InputBox::getTextAreaWidth() const
{
  return getRightText() - getLeftText();
}

int InputBox::getLinesVisible() const
{
  return getTextAreaHeight() / markup.getHeight();
}

void InputBox::drawWidget() const
{
  panel.draw(x0, y0, getSizex(), getSizey());
  bar.draw();
  
  //draw the cursor if active
  if(active && int((getTicks() / 1000.0) * 2) % 2 == 0)
  {
    unsigned long cursorLine;
    unsigned long cursorColumn;
    multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    int cursorXDraw = x0 + getLeftText() + cursorColumn * markup.getWidth();
    int cursorYDraw = y0 + getTopText() + (cursorLine - firstVisibleLine) * markup.getHeight();
    if(cursorYDraw >= y0 + getTopText() && cursorYDraw < y0 + getBottomText() - markup.getHeight()) drawLine(cursorXDraw, cursorYDraw, cursorXDraw, cursorYDraw + markup.getHeight(), cursorColor);
  }

  multiText.draw(x0 + getLeftText(), y0 + getTopText(), firstVisibleLine, firstVisibleLine + getLinesVisible());
}

void InputBox::handleWidget()
{
  bool scrollerMayJump = 0;
  
  if(keyPressed(SDLK_RIGHT)) if(cursor < text.length()) cursor++, scrollerMayJump = 1;
  if(keyPressed(SDLK_LEFT)) if(cursor > 0) cursor--, scrollerMayJump = 1;
  if(keyPressed(SDLK_UP))
  {
    unsigned long cursorLine;
    unsigned long cursorColumn;
    multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    if(cursorLine > 0) cursor = multiText.charAtCursorPos(cursorLine - 1, cursorColumn);
    
    scrollerMayJump = 1;
  }
  if(keyPressed(SDLK_DOWN))
  {
    unsigned long cursorLine;
    unsigned long cursorColumn;
    multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    cursor = multiText.charAtCursorPos(cursorLine + 1, cursorColumn);
    
    scrollerMayJump = 1;
  }
  if(keyPressed(SDLK_HOME))
  {
    if(keyDown(SDLK_LCTRL)) cursor = 0;
    else
    {
      unsigned long cursorLine;
      unsigned long cursorColumn;
      multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
      cursor = multiText.charAtCursorPos(cursorLine, 0);
    }
    
     scrollerMayJump = 1;
  }
  if(keyPressed(SDLK_END))
  {
    if(keyDown(SDLK_LCTRL)) cursor = text.length();
    else
    {
      unsigned long cursorLine;
      unsigned long cursorColumn;
      multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
      cursor = multiText.charAtCursorPos(cursorLine, int(getSizex() / markup.getWidth()));
    }
    
     scrollerMayJump = 1;
  }

  int ascii = unicodeKey(0, 500, 25);
  if(ascii)
  {
    switch(ascii)
    {
      case 8: //backspace
        if(cursor > 0)
        {
          cursor--;
          text.erase(cursor, 1);
        }
        break;
      //a few certainly not meant to be printed ones
      case 0:
        break; 
      case 10:
        break;
      case 13:
        text.insert(cursor, 1, 10);
        cursor++;
        break;
      default:
        text.insert(cursor, 1, ascii);
        cursor++;
        break;
    }

    multiText.text = text;
    multiText.splitWords(getTextAreaWidth());
    
    scrollerMayJump = 1;
    
  }
  
  if(mouseDownHere() && !bar.mouseOver())
  {
    unsigned long mouseColumn = (mouseGetRelPosX() - getLeftText()) / markup.getWidth();
    unsigned long mouseLine = (mouseGetRelPosY() - getTopText()) / markup.getHeight() + firstVisibleLine;
    
    cursor = multiText.charAtCursorPos(mouseLine, mouseColumn);
  }
  
  int scrollSize = multiText.getNumLines() - getLinesVisible();
  if(scrollSize < 1) scrollSize = 1;
  bar.scrollSize = scrollSize;

  if(mouseScrollUp() && !bar.mouseOver())
  {
    bar.scroll(-2);
  }
  if(mouseScrollDown() && !bar.mouseOver())
  {
    bar.scroll(2);
  }
  
  bar.handle();
  
  firstVisibleLine = int(bar.scrollPos);
  
  if(scrollerMayJump)
  {
    unsigned long cursorLine;
    unsigned long cursorColumn;
    multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    
    //int lineDifference = multiText.getNumLines() - getLinesVisible();
    //if(lineDifference < 1) lineDifference = 1;
    
    int cursorRelative = cursorLine - firstVisibleLine; //relative to the first visible line
    
    if(cursorRelative >= getLinesVisible()) bar.scrollPos = cursorLine - getLinesVisible() + 1;
    if(cursorRelative < 0) bar.scrollPos = cursorLine;
    //if(cursorRelative < 0) bar.scrollPos = cursorRelative + 1;

    //if((cursorRelative + 1) * markup.getHeight()

    //if(-getTopText() + (cursorRelative + 1) * markup.getHeight() >= 0) bar.scrollPos = cursorRelative + 1;
    //if(getBottomText() + cursorRelative * markup.getHeight() < (getBottomText() - getTopText())) bar.scrollPos = cursorLine;
  }
  
}

Element* InputBox::getAutoSubElement(unsigned long i)
{
  if(i == 0) return &bar;
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////
//          ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Invisible::make(int x0, int y0, int x1, int y1)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  this->active = true;
  this->visible = true;
}

void Invisible::drawWidget() const
{
  //intentionally do nothing, because this is invisible
}

////////////////////////////////////////////////////////////////////////////////
//GUIMATRIX/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Matrix::make(int x0, int y0, int x1, int y1, int numx, int numy)
{
  this->x0 = x0;
  this->y0 = y0;
  this->x1 = x1;
  this->y1 = y1;
  this->active = true;
  this->visible = true;
  this->numx = numx;
  this->numy = numy;
}

unsigned long Matrix::getTileX() const
{
  return mouseGetRelPosX() / (getSizex() / numx);
}

unsigned long Matrix::getTileY() const
{
  return mouseGetRelPosY() / (getSizey() / numy);
}

int Matrix::getScreenX(int tilex) const
{
  return (tilex * getSizex()) / numx + x0;
}

int Matrix::getScreenY(int tiley) const
{
  return (tiley * getSizey()) / numy + y0;
}

////////////////////////////////////////////////////////////////////////////////
//GUIGRID///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Grid::make(int x0, int y0, int x1, int y1, int tileSizeX, int tileSizeY)
{
  this->x0 = x0;
  this->y0 = y0; 
  this->x1 = x1;
  this->y1 = y1;
  this->active = true;
  this->visible = true;
  this->tileSizeX = tileSizeX;
  this->tileSizeY = tileSizeY;
}

void Grid::setNumTiles(int amount) //real amount can be larger, as the width of rows will stay the same, it'll add rows at the bottom or remove rows
{
  int newsize = 0;
  if(amount > 0) newsize = tileSizeY * (((amount - 1) / getNumx()) + 1);
  growSizeY1(newsize);
}
  
Grid::Grid()
{
  showGrid = false;
}

//returns x position on screen of center of given tile
int Grid::getTileCenterx(int index) const //index = index of the tile
{
  if(getNumx() == 0) return x0;
  int x = (index % getNumx());
  
  x = x * tileSizeX;
  x += tileSizeX / 2;
  
  return x0 + x;
}

//returns y position on screen of center of given tile
int Grid::getTileCentery(int index) const //index = index of the tile
{
  if(getNumy() == 0) return y0;
  int y = (index / getNumx());
  y = y * tileSizeY;
  y += tileSizeY / 2;
  
  return y0 + y;
}

unsigned long Grid::getNumx() const
{
  if(tileSizeX == 0) return 0;
  else return getSizex() / tileSizeX;
}

unsigned long Grid::getNumy() const
{
  if(tileSizeY == 0) return 0;
  else return getSizey() / tileSizeY;
}

unsigned long Grid::getNumElements() const
{
  return getNumx() * getNumy();
}

int Grid::getTileX() const
{
  return mouseGetRelPosX() / tileSizeX;
}

int Grid::getTileY() const
{
  return mouseGetRelPosY() / tileSizeY;
}

int Grid::getTile() const //returns index of the tile
{
  return getTileX() + getNumx() * getTileY();
}

int Grid::getScreenX(int tilex) const
{
  return tilex * tileSizeX + x0;
}

int Grid::getScreenY(int tiley) const
{
  return tiley * tileSizeY + y0;
}


void Grid::drawWidget() const
{
  if(showGrid)
  {
    for(int y = y0; y < y1; y += tileSizeY)
    for(int x = x0; x < x1; x += tileSizeX)
    {
      drawLine(x, y0, x, y1, gridColor);
      drawLine(x0, y, x1, y, gridColor);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
//GUICHECKBOX///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The Checkbox Class Functions
CheckBox is a simple box that can have a different texture if "on" or "off",
and toggles if you press or release the mouse button on it.

How to use:
  Give it two textures, texture1 one that looks like an unchecked checkbox,
  texture two like a checked checkbox.
  Optionally, give it a title that'll be drawn next to the textbox
  Choose if you want it to toggle on mousedown, or on mouseup with the
  toggleOnMouseUp boolean.
*/

Checkbox::Checkbox()
{
  this->enableImage2 = 0;
  //bad old code, must be fixed!
  this->active = 0;
  this->checked = 0;
  this->texture[0] = &emptyTexture;
  this->texture[1] = &emptyTexture;
  this->texture[2] = &emptyTexture;
  this->texture[3] = &emptyTexture;
  this->colorMod[0] = RGB_White;
  this->colorMod[1] = RGB_White;
  this->colorMod[2] = RGB_White;
  this->colorMod[3] = RGB_White;
  this->toggleOnMouseUp = 0;
  this->enableText = 0;
}

void Checkbox::addFrontImage
(
  const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, 
  const ColorRGB& imageColor1, const ColorRGB& imageColor2, const ColorRGB& imageColor3, const ColorRGB& imageColor4
)
{
  this->enableImage2 = 1;
  
  this->texture2[0] = texture1;
  this->texture2[1] = texture2;
  this->texture2[2] = texture3;
  this->texture2[3] = texture4;
  this->colorMod2[0] = imageColor1;
  this->colorMod2[1] = imageColor2;
  this->colorMod2[2] = imageColor3;
  this->colorMod2[3] = imageColor4;
}

void Checkbox::addFrontImage(const Texture* texture)
{
  addFrontImage(texture, texture, texture, texture, colorMod[0], colorMod[1], colorMod[2], colorMod[3]);
}
//constructor for checkbox with text title
void Checkbox::make(int x, int y, bool checked, const GuiSet* set, int toggleOnMouseUp)
{
  this->x0 = x;
  this->y0 = y;
  this->checked = checked;
  this->texture[0] = set->checkBox[0];
  this->texture[1] = set->checkBox[0];
  this->texture[2] = set->checkBox[1];
  this->texture[3] = set->checkBox[1];
  this->colorMod[0] = set->mainColor;
  this->colorMod[1] = set->mouseOverColor;
  this->colorMod[2] = set->mainColor;
  this->colorMod[3] = set->mouseOverColor;
  this->setSizex(set->checkBox[0]->getU());
  this->setSizey(set->checkBox[0]->getV());
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->totallyEnable();
  
  positionText();
}

void Checkbox::setTexturesAndColors(const Texture* texture1, const Texture* texture2, const Texture* texture3, const Texture* texture4, const ColorRGB& color1, const ColorRGB& color2, const ColorRGB& color3, const ColorRGB& color4)
{
  this->texture[0] = texture1;
  this->texture[1] = texture2;
  this->texture[2] = texture3;
  this->texture[3] = texture4;
  this->colorMod[0] = color1;
  this->colorMod[1] = color2;
  this->colorMod[2] = color3;
  this->colorMod[3] = color4;
  this->setSizex(texture[0]->getU());
  this->setSizey(texture[0]->getV());
  
  positionText();
}

void Checkbox::setTexturesAndColors(const Texture* texture1, const Texture* texture2, const ColorRGB& color1, const ColorRGB& color2)
{
  setTexturesAndColors(texture1, texture1, texture2, texture2, color1, color1, color2, color2);
}

void Checkbox::addText(const std::string& text, const Markup& markup)
{
  this->text = text;
  this->markup = markup;
  this->enableText = 1;
  positionText();
}

//place the text next to the checkbox
void Checkbox::positionText()
{
  textOffsetX = getSizex() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = getSizey() / 2 - markup.getHeight() / 2;
}

//toggle "checked" or "unchecked" state
void Checkbox::toggle()
{
  if(checked) uncheck();
  else check();
}

//see how you click with the mouse and toggle on click
void Checkbox::handleWidget()
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(toggleOnMouseUp == 0 && pressed()) toggle();
  if(toggleOnMouseUp == 1 && clicked()) toggle();
}

//draw the checkbox with a texture depending on it's state
void Checkbox::drawWidget() const
{
  /*if(checked) texture2->draw(x0, y0, colorMod2);
  else texture1->draw(x0, y0, colorMod1);*/
  int i = 0;
  if(mouseOver()) i++;
  if(checked) i += 2;
  
  texture[i]->draw(x0, y0, colorMod[i]);
  
  if(enableImage2)
    texture2[i]->draw(x0, y0, colorMod2[i]);
  
  if(enableText)
  print(text, x0 + textOffsetX, y0 + textOffsetY, markup);
}

////////////////////////////////////////////////////////////////////////////////
//GUINSTATESTATE////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This class is used by NState
*/

NStateState::NStateState()
{
  this->texture = &emptyTexture;
  this->colorMod = RGB_White;
  this->enableText = 0;
  this->text = "";
}

//constructor for checkbox with text title
void NStateState::make(Texture* texture, const ColorRGB& colorMod, const std::string& text, const Markup& markup)
{
  this->texture = texture;
  this->colorMod = colorMod;
  this->text = text;
  this->markup = markup;
  if(text != "") enableText = 1;
  
  positionText();
}



//place the text next to the checkbox
void NStateState::positionText()
{
  textOffsetX = texture->getU() + 2; //need some number of pixels that text is away from the texture, eg 2
  textOffsetY = texture->getV() / 2 - markup.getHeight() / 2;
}

////////////////////////////////////////////////////////////////////////////////
//GUINSTATE/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This is a sort of advanced checkbox, it can go through multiple states and you can add more states.
*/

NState::NState()
{
  //bad old code, must be fixed!
  this->active = 0;
  this->states.clear();
  this->toggleOnMouseUp = 0;
}

//constructor for checkbox with text title
void NState::make(int x, int y, int toggleOnMouseUp)
{
  states.clear();
  this->x0 = x;
  this->y0 = y;
  this->toggleOnMouseUp = toggleOnMouseUp;
  this->totallyEnable();
  this->setSizex(0); //no states yet, size 0
  this->setSizey(0);
}

void NState::addState(Texture* texture, const ColorRGB& colorMod, const std::string& text, const Markup& markup)
{
  this->setSizex(texture->getU()); //set the size of the NState to that of the last added texture
  this->setSizey(texture->getV());
  NStateState s;
  s.make(texture, colorMod, text, markup);
  states.push_back(s);
}


//see how you click with the mouse and toggle on click
void NState::handleWidget()
{
  //make sure never both pressed() and clicked() are checked, because one test screws up the other, hence the order of the tests in the if conditions
  if(states.size() == 0) return;
  
  if((toggleOnMouseUp == 0 && pressed()) || (toggleOnMouseUp == 1 && clicked()))
  {
    if(state >= states.size() - 1) state = 0;
    else state++;
  }
  if((toggleOnMouseUp == 0 && pressed(RMB)) || (toggleOnMouseUp == 1 && clicked(RMB)))
  {
    if(state == 0) state = states.size() - 1;
    else state--;
  }
  
}

//draw the checkbox with a texture depending on it's state
void NState::drawWidget() const
{
  if(states.size() == 0) return;
  
  NStateState s = states[state];
  s.texture->draw(x0, y0, s.colorMod);
  
  if(s.enableText) print(s.text, x0 + s.textOffsetX, y0 + s.textOffsetY, s.markup);
}

////////////////////////////////////////////////////////////////////////////////
//GUIBULLETLIST/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The BulletList

it's a list of checkboxes, where only one can be selected at the time

you first make() the prototype, and make() the bulletlist. Then all checkboxes have the
style you  want!
*/

Element* BulletList::getAutoSubElement(unsigned long i)
{
  if(i < bullet.size()) return &bullet[i];
  else return 0;
}

BulletList::BulletList()
{
  this->active = 0;
  this->visible = 0;
  this->lastChecked = -1;
  
  this->prototype.make(0, 0, 0, &builtInGuiSet, 1);
  this->prototype.setTexturesAndColors(builtInGuiSet.bullet[0], builtInGuiSet.bullet[1], builtInGuiSet.mainColor, builtInGuiSet.mainColor);
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, const GuiSet* set)
{
  this->prototype.make(0, 0, 0, set, 1);
  this->prototype.setTexturesAndColors(set->bullet[0], set->bullet[1], set->mainColor, set->mainColor);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->totallyEnable();

  bullet.clear();
  for(unsigned long i = 0; i < amount; i++)
  {
    bullet.push_back(prototype);
    bullet[i].moveTo(x + xDiff * i, y + yDiff * i);
  }
  
  //NOTE: DIT IS NIET CORRECT, DEZE FORMULES
  this->setSizex(amount * xDiff + prototype.getSizex());
  this->setSizey(amount * yDiff + prototype.getSizey());
}

void BulletList::make(int x, int y, unsigned long amount, int xDiff, int yDiff, unsigned long amountx, const GuiSet* set)
{
  if(amountx < 1) amountx = 1;
  this->prototype.make(0, 0, 0, set, 1);
  this->prototype.setTexturesAndColors(set->bullet[0], set->bullet[1], set->mainColor, set->mainColor);
  
  this->x0 = x;
  this->y0 = y;
  this->xDiff = xDiff;
  this->yDiff = yDiff;
  this->totallyEnable();

  bullet.clear();
  int xPos = x, yPos = y;
  for(unsigned long i = 0; i < amount; i++)
  {
    xPos = x + xDiff * (i % amountx);
    yPos = y + yDiff * (i / amountx);
    bullet.push_back(prototype);
    bullet[i].moveTo(xPos, yPos);
  }
  
  setCorrectSize();
}

void BulletList::setCorrectSize()
{
  if(bullet.size() == 0) return;
  
  int minx = bullet[0].getX0();
  int miny = bullet[0].getY0();
  int maxx = bullet[0].getX1();
  int maxy = bullet[0].getY1();
  
  for(size_t i = 1; i < bullet.size(); i++)
  {
    minx = std::min(minx, bullet[i].getX0());
    miny = std::min(miny, bullet[i].getY0());
    maxx = std::max(maxx, bullet[i].getX1());
    maxy = std::max(maxy, bullet[i].getY1());
  }
  
  this->x0 = minx;
  this->y0 = miny;
  this->setSizex(maxx - minx);
  this->setSizey(maxy - miny);
}

void BulletList::handleWidget()
{
  if(!mouseOver()) return; //some speed gain, don't do all those loops if the mouse isn't over this widget
  
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].handle();
  }

  int newChecked = -1;
  
  for(size_t i = 0; i < bullet.size(); i++)
  {
    if(bullet[i].checked && int(i) != lastChecked) newChecked = i;
    bullet[i].checked = false;
  }

  if(newChecked >= 0)
  {
    bullet[newChecked].checked = true;
    lastChecked = newChecked;
  }
  else if(lastChecked >= 0 && lastChecked < int(bullet.size()))
    bullet[lastChecked].checked = true;
}

void BulletList::drawWidget() const
{
  for(unsigned long i = 0; i < bullet.size(); i++)
  {
    bullet[i].draw();
  }
}

int BulletList::check()
{
  return lastChecked;
}

void BulletList::addText(const std::string& text, unsigned long i)
{
  if(i < bullet.size()) bullet[i].addText(text);
}

std::string BulletList::getText(unsigned long i) const
{
  std::string result;
  if(i < bullet.size()) result = bullet[i].getText();
  return result;
}

const std::string& BulletList::getCurrentText() const
{
  return bullet[lastChecked].getText();
}

void BulletList::set(unsigned long i)
{
  /*if(i >= 0 && i < bullet.size()) bullet[i].checked = true;
  if(!active) lastChecked = i;*/

  for(unsigned long j = 0; j < bullet.size(); j++)
  {
    if(i != j) bullet[j].checked = false;
    else bullet[j].checked = true;
  }
  
  lastChecked = i;
  
  handle();
}



////////////////////////////////////////////////////////////////////////////////
//STRINGSTACK///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The String Stack is a rotating stack of strings, that is very helpful for the
Console.

It's actually not a real stack: you don't have to pop() to get values out of it,
you only have to push values into it, it rotates, and you can access any value.

You can access values relative to the newest, or relative to the oldest value.

The StringStack has a finite number of positions, when it's full, it'll overwrite
the oldest value. It can however use indexes as if the newely added text has always
the highest index when the stack is full.

First it must allocate memory for the wanted number of strings, and it must
destroy it again when it's no longer used.

num = max number of strings
*/

StringStack::StringStack()
{
  this->num = 0;
  this->oldest = 0;
  this->next = 0;
  this->numFilled = 0;
}

void StringStack::make(int num)
{
  if(num < 1) num = 1;
  this->num = num;
  this->oldest = 0;
  this->next = 0;
  this->numFilled = 0;
  
  for(int i = 0; i < num; i++) text.push_back("");
}

void StringStack::push(const std::string& text)
{
  this->text[next] = text;
  if(next == oldest && numFilled > 0) oldest = increase(oldest);
  next = increase(next);
  
  if(numFilled < num) numFilled++;
}

//returns the text that is position positions away from the oldest text
const std::string& StringStack::getOldest(int offset)
{
  int pointer = increase(oldest, offset);
  return text[pointer];
}

const std::string& StringStack::getNewest(int offset)
{
  int pointer = decrease(next);
  pointer = decrease(pointer, offset);
  
  return text[pointer];
}

//returns how full the memory is
int StringStack::getNumText() const
{
  return numFilled;
}

//increase the position of a pointer (like oldest or next), if it reaches the edge it goes back to 0
int StringStack::increase(int pointer, int amount)
{
  pointer += amount;
  if(pointer >= num) pointer %= num;
  return pointer;
}

int StringStack::decrease(int pointer, int amount)
{
  pointer -= amount;
  while(pointer < 0) pointer += num;
  if(pointer >= num) pointer %= num;
  return pointer;
}

////////////////////////////////////////////////////////////////////////////////
//GUICONSOLE////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Console is a text window that contains different texts and can display
some of them like a console.

It's supposed to be used together with a scrollbar to be able to scroll it.
*/

Console::Console()
{
  this->num = 0;
  
  this->active = 0;
  this->visible = 0;

  this->scroll = 0;
}

void Console::make(int x, int y, int sizex, int sizey, int num, const Markup& markup)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->num = num;
  this->markup = markup;
  
  this->totallyEnable();

  this->scroll = 0;
  
  this->stack.make(num);

  calcNumLines();
}

void Console::push(const std::string& text/*, int length*/)
{
  stack.push(text);
  calcNumLines();  
}

void Console::calcNumLines()
{
  int line = 0;
  MultiLineText t;
  t.markup = markup;
  for(int i = 0; i < num; i++)
  {
    t.setText(stack.getOldest(i));
    t.splitWords(getSizex());
    line += t.getNumLines();
  }
  numLines = line;
}

int Console::getNumLines() const
{
  return numLines;
}

int Console::getNumMessages() const
{
  return stack.getNumText();
}

int Console::getVisibleLines(const Markup& markup)
{
  return getSizey() / markup.getHeight();
}

void Console::drawWidget() const
{
  int line = 0;
  int message = 0;
  int maxScreenLines = getSizey() / markup.getHeight();
  int messageLines = 0;
  int messageStartLine = 0;
  int messageEndLine = 0;
  int messageStartLineScreen = 0;
  int messageEndLineScreen = 0;
  int drawMessageStartLine = -1;
  int drawMessageEndLine = -1;
  int yDraw = 0;
   
  MultiLineText t;
  t.markup = markup;

  while(line - scroll < maxScreenLines && message < stack.getNumText())
  {
    messageStartLine = line; //first line of the message in all messages
    
    t.setText(stack.getOldest(message));
    t.splitWords(getSizex());
    messageLines = t.getNumLines();
    //messageLines = calcTextLines(text[message], sizex, markup); //number of lines of the message
    messageEndLine = messageStartLine + messageLines - 1; //last line of the message
    messageStartLineScreen = messageStartLine - scroll;
    messageEndLineScreen = messageEndLine - scroll;
    line += messageLines;
    if(line >= scroll)
    {
      yDraw = y0 + (messageStartLine - scroll) * markup.getHeight();
      
      if(messageStartLineScreen < 0)
      {
        drawMessageStartLine = -messageStartLineScreen;
        yDraw = y0;
      }
      else drawMessageStartLine = -1;
      
      if(messageEndLineScreen >= maxScreenLines) drawMessageEndLine = messageLines - (messageEndLineScreen - maxScreenLines + 1);
      else drawMessageEndLine = -1;
      
      t.draw(x0, yDraw, drawMessageStartLine, drawMessageEndLine);
    }
    
    message++;
  }
}

////////////////////////////////////////////////////////////////////////////////
//GUITEXTAREA///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
hmm I forgot what a guitextarea is for...
*/

TextArea::TextArea()
{
  this->active = 0;
  this->visible = 0;

  this->scroll = 0;
  
  this->scrollEnabled = 0;
}

void TextArea::make(int x, int y, int sizex, int sizey, const std::string& text, const Markup& markup)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->markup = markup;
  this->text.markup = markup;
  
  this->text.setText(text);
  this->text.splitWords(getSizex());

  
  this->totallyEnable();

  this->scroll = 0;
}

void TextArea::setText(const std::string& text, const Markup& markup)
{
  this->markup = markup;
  this->text.markup = markup;
  
  this->text.setText(text);
  int width = getSizex();
  if(scrollEnabled) width -= scrollbar.getSizex();
  this->text.splitWords(width);

  this->scroll = 0;
  
  if(scrollEnabled) setScrollbarSize();
}


int TextArea::getNumLines() const
{
  return text.getNumLines();
}

void TextArea::resizeWidget()
{
  this->text.splitWords(getSizex());
}

Element* TextArea::getAutoSubElement(unsigned long i)
{
  if(i == 0) return &scrollbar;
  else return 0;
}

void TextArea::handleWidget()
{
  if(scrollEnabled)
  {
    scrollbar.handle();
    scroll = int(scrollbar.scrollPos);
  }
}

int TextArea::getVisibleLines(const Markup& markup) const
{
  return getSizey() / markup.getHeight();
}

void TextArea::drawWidget() const
{
  int visible = getVisibleLines(markup);
  //scroll is the startline, scroll + visible is the end line
  int startLine = scroll;
  int endLine = scroll + visible;
  
  text.draw(x0, y0, startLine, endLine);
  
  if(scrollEnabled) scrollbar.draw();
}

void TextArea::setScrollbarSize()
{
  int size = text.getNumLines() - (getSizey() / markup.getHeight());
  if(size < 0) size = 0;
  
  scrollbar.scrollSize = size;
}

void TextArea::addScrollbar()
{
  scrollEnabled = true;
  
  scrollbar.makeVertical(x1 - 16, 0, getSizey());
  this->text.splitWords(getSizex() - scrollbar.getSizex());
  
  setScrollbarSize();
}

////////////////////////////////////////////////////////////////////////////////
//GUIIMAGE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Image is a simple texture but as child class of Element
*/

Image::Image()
{
  this->visible = 0;
  this->active = 0;
  this->image = &emptyTexture;
}

void Image::make(int x, int y, Texture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(image->getU());
  this->setSizey(image->getV());
  this->image = image;
  this->colorMod = colorMod;
  this->totallyEnable();
}

void Image::make(int x, int y, int sizex, int sizey, Texture* image, const ColorRGB& colorMod)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->image = image;
  this->colorMod = colorMod;
  this->totallyEnable();
}

void Image::drawWidget() const
{
  image->draw(x0, y0, colorMod, getSizex(), getSizey());
}

////////////////////////////////////////////////////////////////////////////////
//GUIRECTANGLE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Rectangle is a simple rectangle but as child class of Element
*/

Rectangle::Rectangle()
{
  this->visible = 0;
  this->active = 0;
  this->color = RGB_Black;
}

void Rectangle::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->totallyEnable();
}

void Rectangle::drawWidget() const
{
  drawRectangle(x0, y0, x1, y1, color);
}

////////////////////////////////////////////////////////////////////////////////
//GUIPAINTER////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
You give Painter drawing commands (lines, pictures, points, ...) that are queued, and then
all drawn when you call draw(). The queues are then cleared, to be filled again the next frame.

There's also by default a rectangle behind it, if you don't want it, set alpha channel of rectangle color to 0
*/

Painter::Painter()
{
  this->visible = 0;
  this->active = 0;
  this->stack.clear();
}

void Painter::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->stack.clear();
  this->totallyEnable();
}

void Painter::drawWidget() const
{
  //first draw the rectangle if the alpha channel of color is > 0
  if(color.a > 0) drawRectangle(x0, y0, x1, y1, color);
  
  //then draw the queued elements, in order
  for(unsigned long i = 0; i < stack.size(); i++)
  {
    switch(stack[i].type)
    {
      case 0: 
        drawPoint(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 1: 
        drawLine(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color);
        break;
      case 2: 
        stack[i].texture->draw(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 3: 
        stack[i].texture->drawCentered(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].color);
        break;
      case 4: 
        drawRectangle(stack[i].x0 + x0, stack[i].y0 + y0, stack[i].x1 + x0, stack[i].y1 + y0, stack[i].color);
        break;
      case 5:
        print(stack[i].text, stack[i].x0 + x0, stack[i].y0 + y0, stack[i].textMarkup);
        break;
      default: break;
    }
  }
  
  stack.clear();
}

void Painter::queuePoint(int x, int y, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = color;
  stack[i].type = 0;
}
void Painter::queueLine(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x0;
  stack[i].y0 = y0;
  stack[i].x1 = x1;
  stack[i].y1 = y1;
  stack[i].color = color;
  stack[i].type = 1;
}

void Painter::queueTexture(int x, int y, Texture* texture, const ColorRGB& colorMod)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = colorMod;
  stack[i].texture = texture;
  stack[i].type = 2;
}

void Painter::queueTextureCentered(int x, int y, Texture* texture, const ColorRGB& colorMod)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].color = colorMod;
  stack[i].texture = texture;
  stack[i].type = 3;
}

void Painter::queueRectangle(int x0, int y0, int x1, int y1, const ColorRGB& color)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x0;
  stack[i].y0 = y0;
  stack[i].x1 = x1;
  stack[i].y1 = y1;
  stack[i].color = color;
  stack[i].type = 4;
}

void Painter::queueText(int x, int y, const std::string& text, const Markup& markup)
{
  int i = stack.size();
  stack.resize(stack.size() + 1);
  
  stack[i].x0 = x;
  stack[i].y0 = y;
  stack[i].x1 = x;
  stack[i].y1 = y;
  stack[i].text = text;
  stack[i].textMarkup = markup;
  stack[i].type = 5;
}




////////////////////////////////////////////////////////////////////////////////
//GUILINE///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Line is a simple line but as child class of Element
*/

Line::Line()
{
  this->visible = 0;
  this->active = 0;
  this->color = RGB_Black;

  this->lx0 = 0;
  this->ly0 = 0;
  this->lx1 = 0;
  this->ly1 = 0;
}

void Line::make(int x, int y, int sizex, int sizey, const ColorRGB& color)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->color = color;
  this->totallyEnable();
  this->lx0 = x;
  this->ly0 = y;
  this->lx1 = x + sizex;
  this->ly1 = y + sizey;
}

void Line::setEndpoints(int x0, int y0, int x1, int y1)
{
  this->lx0 = x0;
  this->ly0 = y0;
  this->lx1 = x1;
  this->ly1 = y1;
}

void Line::drawWidget() const
{
  drawLine(lx0, ly0, lx1, ly1, color);
}

////////////////////////////////////////////////////////////////////////////////
//GUITEXT///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Text is just some text as child of Element
*/

Text::Text()
{
  this->visible = 0;
  this->active = 0;
  this->useNewLine = true;
}

void Text::make(int x, int y, const std::string& text, const Markup& markup)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(text.length() * markup.getWidth());
  this->setSizey(markup.getHeight());
  this->text = text;
  this->markup = markup;
  this->totallyEnable();
}

void Text::drawWidget() const
{
  print(text, x0, y0, markup, useNewLine);
}

void Text::setText(const std::string& text)
{
  this->text = text;
}

////////////////////////////////////////////////////////////////////////////////
//GUIFORMATTEDTEXT//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
FormattedText is just some formatted text as child of Element
*/

FormattedText::FormattedText()
{
  this->visible = 0;
  this->active = 0;
}

void FormattedText::make(int x, int y, const std::string& text, const Markup& markup)
{
  this->x0 = x;
  this->y0 = y;
  this->setSizex(text.length() * markup.getWidth());
  this->setSizey(markup.getHeight());
  this->text = text;
  this->markup = markup;
  this->totallyEnable();
}

void FormattedText::drawWidget() const
{
  printFormatted(text, x0, y0, markup);
}

void FormattedText::setText(const std::string& text)
{
  this->text = text;
}

////////////////////////////////////////////////////////////////////////////////
//YESNOWINDOW///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

YesNoWindow::YesNoWindow()
{
  this->active = 0;
  this->visible = 0;
}

void YesNoWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  Window::make(x, y , sizex, sizey);
  addTop();
  
  yes.makeTextPanel(0, 0, "Yes");
  no.makeTextPanel(0, 0, "No");
  //yes.autoTextSize();
  //no.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTop(&message, 16, 16);
  int centerx = getSizex() / 2;
  pushTop(&yes, centerx - yes.getSizex() - 16, getSizey() - yes.getSizey() - 8 - 16);
  pushTop(&no, centerx + 16, getSizey() - no.getSizey() - 8 - 16);
  
  this->active = 1;
  this->visible = 1;
}

////////////////////////////////////////////////////////////////////////////////
//OKWINDOW//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

OkWindow::OkWindow()
{
  this->active = 0;
  this->visible = 0;
}

void OkWindow::make(int x, int y, int sizex, int sizey, const std::string& text)
{
  Window::make(x, y , sizex, sizey);
  addTop();
  
  ok.makeTextPanel(0, 0, "Ok");
  //ok.autoTextSize();
  
  message.make(0, 0, text);
  
  pushTop(&message, 16, 16);
  int centerx = getSizex() / 2;
  pushTop(&ok, centerx - ok.getSizex() / 2, getSizey() - ok.getSizey() - 8 - 16);
  
  this->active = 1;
  this->visible = 1;
}
  
////////////////////////////////////////////////////////////////////////////////
//GUICANVAS/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
Canvas: allows you to draw on it with the mouse
*/

Canvas::Canvas()
{
  this->visible = 0;
  this->active = 0;
}

void Canvas::make(int x, int y, int sizex, int sizey, const ColorRGB& backColor, int border, const ColorRGB& leftColor, const ColorRGB& rightColor, double size, double hardness, double opacity)
{
  this->visible = true;
  this->active = true;
  this->x0 = x;
  this->y0 = y;
  this->setSizex(sizex);
  this->setSizey(sizey);
  this->leftColor = leftColor;
  this->rightColor = rightColor;
  this->backColor = backColor;
  this->border = border;
  
  this->size = size;
  this->opacity = opacity;
  this->hardness = hardness;
  
  this->oldMouseX = -1;
  this->oldMouseY = -1;
  
  init();
}

void Canvas::init()
{
  canvas.create(backColor, getSizex(), getSizey());
}

void Canvas::handleWidget()
{
  if(mouseGrabbed(LMB) || mouseGrabbed(RMB))
  {
    ColorRGB drawColor;
    
    if(globalLMB) drawColor = leftColor;
    else drawColor = rightColor;
    
    int drawx = mouseGetRelPosX();
    int drawy = mouseGetRelPosY();

    if(validOldMousePos == true)
    {
      drawLine(&canvas.getOpenGLBuffer()[0], canvas.getU2(), canvas.getV2(), oldMouseX, oldMouseY, drawx, drawy, drawColor, border, border, getSizex() - border, getSizey() - border);
      canvas.upload();
    }
    oldMouseX = drawx;
    oldMouseY = drawy;
    validOldMousePos = true;
  }
  else validOldMousePos = false;
}

void Canvas::drawWidget() const
{
  canvas.draw(x0, y0);
}

void Canvas::clear()
{
  canvas.create(backColor, getSizex(), getSizey());
}



////////////////////////////////////////////////////////////////////////////////
//OTHER FUNCTIONS///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//MultiLineText class
////////////////////////////////////////////////////////////////////////////////

MultiLineText::MultiLineText()
{
  this->text = "";
  resetLines();
  char10 = 1;
  useFormatting = 1;
  markup = TS_W;
}

void MultiLineText::resetLines()
{
  line.clear(); //reset lines
  line.push_back(0); //character 0 is always a new line since it's the beginning of the text
}

void MultiLineText::generateLineMarkup()
{
  lineMarkup.clear();
  lineMarkup.push_back(markup);
  
  Markup markup_ = markup;
  
  for(unsigned long i = 0; i < getNumLines(); i++)
  {
    std::string s = getLine(i);
    getFormattedMarkup(s, markup_, markup);
    lineMarkup.push_back(markup_);
  }
}

//from now on, this will also split words that are too long into multiple parts
void MultiLineText::splitWords(int width)
{
  resetLines();
  
  unsigned long charsPerLine = width / markup.getWidth();
  
  unsigned long pos = 0; //current position in the string
  unsigned long cline = 1; //current line
  unsigned long linepos = 0; //*printable* character position on current line, becomes 0 at every "newline"
  
  while(pos < text.length())
  {
    //get length of next word
    unsigned long wordpos = pos;
    unsigned long wordlength = 0;
    unsigned long steplength = 0; //word length + length of possible format symbols
    bool formattedEnd = false;
    while(!formattedEnd && !(text[wordpos] == ' ' || text[wordpos] == 0 || (char10 && (text[wordpos] == 10 || text[wordpos] == 13)) || wordlength >= charsPerLine))
    {
      if(!useFormatting || text[wordpos] != '#')
      {
        wordpos++;
        wordlength++;
        steplength++;
      }
      else
      {
        wordpos++;
        steplength++;
        if(wordpos < text.size())
        {
          char c = text[wordpos];
          switch(c)
          {
            case 'n':
              formattedEnd = true; //it breaks out of this loop, and the next part of the function will then detect the 'n' and treat it as newline
              break;
            default:
              wordpos += getFormattedTextAttrSize(c) + 1;
              wordlength += getFormattedTextSymbolPrintSize(c);
              steplength += getFormattedTextAttrSize(c) + 1;
              break;
          }
        }
      }
    }
    
    //for now, words longer than width aren't split in two parts

    //see if word can be drawn on current line
    if(linepos + wordlength > charsPerLine)
    {
      line.push_back(pos); //the word is split by inserting newline in front of the word
      cline++;
      linepos = 0; //you're at a new line, so you're at the first char of the new line again
    }
    
    //pos pointer now goes to behind the following word
    pos += steplength;
    linepos += wordlength; //since you go to behind the word drawn on the current line

    //now check what is there behind the word: is it a newline? a space? or a null-termination char?
    switch(text[pos])
    {
      case ' ': //space
        pos++;
        linepos++;
        break;
      case 10: //newline
        if(char10)
        {
          line.push_back(pos);
          cline++;
          linepos = 0;
          pos++;
        }
        break;
      case 13: //ms windows newline, ignore
        if(char10)
        {
          pos++;
          //linepos++;
        }
        break;
      case 'n': //it's the newline, there's a # in front of it for sure because that was already checked higher
        if(useFormatting)
        {
          line.push_back(pos - 1);
          cline++;
          linepos = 0;
          pos++;
        }
        break;
      /*case 0: //null termination char
        break;*/
      default: //not supposed to happen
        //dprint("WTF? Error in MultiLineText::splitWords");
        break;
    }
  }
  
  line.push_back(text.length());
  
  if(useFormatting) generateLineMarkup();
}

void MultiLineText::splitLetters(int width)
{
  resetLines(); 
  
  unsigned long charsPerLine = width / markup.getWidth();
  
  unsigned long pos = 0; //current position in the string
  unsigned long cline = 1; //current line
  unsigned long linepos = 0; //character position on current line, becomes 0 at every "newline"
  
  while(/*text[pos] != 0*/pos < text.length())
  {
    switch(text[pos])
    {
      /*case 0: //termination char
        break;*/
      case 10: //newline char
        if(char10)
        {
          line[cline] = pos;
          cline++;
          linepos = 0;
          pos++;
        }
        break;
      default:
        if(linepos >= charsPerLine)
        {
          line.push_back(pos);
          cline++;
          linepos = 0;
        }
        pos++;
        linepos++;
        break;
    }
  }
  
  line.push_back(text.length());
  
  if(useFormatting) generateLineMarkup();
}

void MultiLineText::splitChar10()
{
  resetLines();
  
  unsigned long pos = 0; //current position in the string
  unsigned long cline = 1; //current line
  
  while(/*text[pos] != 0*/pos < text.length())
  {
    switch(text[pos])
    {
      /*case 0: //termination char
        break;*/
      case 10: //newline char
        line.push_back(pos);
        cline++;
        pos++;
        break;
      default:
        pos++;
        break;
    }
  }
  
  line.push_back(text.length());
  
  if(useFormatting) generateLineMarkup();
}

void MultiLineText::setText(const std::string& text)
{
  this->text = text;
}

void MultiLineText::addText(const std::string& text)
{
  this->text += text;
}

void MultiLineText::draw(int x, int y, unsigned long startLine, unsigned long endLine) const
{
  Markup markup_ = markup;
  
  if(startLine == endLine) return;
  
  unsigned long cline = startLine;
  
  while(cline < endLine)
  {
    int printy = y + markup.getHeight() * (cline - startLine); //y coordinate to draw the text
    std::string sub = getLine(cline); //string containing the current line
    if(useFormatting)
    {
      if(cline < lineMarkup.size()) markup_ = lineMarkup[cline];
      printFormattedM(sub, x, printy, markup_, markup);
    }
    else print(sub, x, printy, markup);
    cline++;
  }
}

void MultiLineText::draw(int x, int y) const
{
  draw(x, y, 0, getNumLines());
}

unsigned long MultiLineText::getNumLines() const
{
  if(text.length() == 0) return 0; //no text!
  
  return line.size() - 1; //the size of the std::vector is also the number of lines, minus one because the last value denotes the position of the final character of the text, which is no new line
}

std::string MultiLineText::getLine(unsigned long i) const
{
  if(i >= getNumLines()/* || i < 0*/) return "";
  
  int length = line[i + 1] - line[i];
  
  int start = line[i]; //the first character of the substring
  if(char10 && text[line[i]] == 10) //do NOT draw newline chars if it's used as newline (i.e. when char10 is true)
  {
    start++;
    length--;
  }
  
  if(length < 0) length = 0;
  
  //std::string sub = text.substr(start, length);
  std::string sub;
  for(int j = start; j < start + length; j++)
  {
    if(!(char10 && text[j] == 13)) sub += text[j];
  }
  
  return sub;
}

//returns line and column of character at pos (handy for cursors)
void MultiLineText::cursorAtCharPos(unsigned long pos, unsigned long& cline, unsigned long& column) const
{
  cline = 0;
  column = 0;
  
  int extraPos = 0;
  
  if(text[pos] == 10) {pos--; extraPos++;}

  if(text.length() == 0)
  {
    cline = 0;
    column = 0;
  }
  else if(getNumLines() == 1)
  {
    cline = 0;
    column = pos;
  }
  else if(pos < line[1])
  {
    cline = 0;
    column = pos;
  }
  else
  {
    if(pos >= text.length())
    {
      extraPos = pos - text.length() + 1;
      pos = text.length() - 1;
    }
  
    while(line[cline] <= pos)
    {
      cline++;
    }
    cline--;
    column = pos - line[cline];
    
    if(text[line[cline]] == 10) {column--; } //lines start with a 10 char if you pressed enter but this char isn't drawn. If the line does not start with a 10 char, column must be done + 1
  }
  column += extraPos;
}

//returns position of letter at given line and column
int MultiLineText::charAtCursorPos(unsigned long cline, unsigned long column) const
{
  if(getNumLines() <= cline) return text.length();
  
  unsigned long lineLength;
  
  if(cline == getNumLines() - 1) lineLength = text.length() - line[cline];
  else if(cline == 0) 
  {
    lineLength = line[1];
    if(text[line[1]] != 10) lineLength--;
  }
  else lineLength = line[cline + 1] - line[cline] - 1;
  
  unsigned long pos = line[cline];
  
  if(column > lineLength) column = lineLength;
  
  if(text[line[cline]] == 10) pos += 1;
  
  pos += column;
  
  if(pos > text.length()) pos = text.length();
  
  /*if(cline == 0)
  {
    pos = column;
  }*/
  
  return pos;
}

#include "lpi_unittest.h"

#define LUT_MY_RESET \
{\
  debugSetLMB(0);\
  debugSetRMB(0);\
  debugSetMousePos(0, 0);\
}
  
void unitTest()
{
  screen(1024, 768, 0, "Unit Testing...");
  
  LUT_START_UNIT_TEST
  
  //I had a bug where when making a new BulletList and using "set" to give it a value, "check" wasn't returning the correct value until mouseOver
  LUT_CASE("set bulletlist")
    BulletList test1;
    test1.make(0, 0, 5, 0, 0);
    test1.set(3);
    LUT_SUB_ASSERT_FALSE(test1.check() != 3)
    test1.set(1);
    LUT_SUB_ASSERT_FALSE(test1.check() != 1)
  LUT_CASE_END
  
  LUT_CASE("mouseOver and mouseDown on Dummy")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver())
    debugSetMousePos(1, 1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseDown())
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseDown())
  LUT_CASE_END
  
  LUT_CASE("mouseJustDown")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown()) //the mouse moved over the element, the mouse button was already down since being outside the element, so it's now down over the element for the first time
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown()) //now it may not return true anymore!
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetMousePos(20, 20);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseJustDownHere on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetMousePos(mx, my);
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetLMB(0);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere())
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseJustDownHere())
    LUT_SUB_ASSERT_TRUE(!w.top.mouseJustDownHere()) //now it may not return true anymore!
  LUT_CASE_END
  
  LUT_CASE("mouseGrabbed")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse not down yet
    debugSetMousePos(10, 10);
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 10 && dummy.mouseGetGrabY() == 10)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(15, 15);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed
    /*//now repeat the test to see if states can properly be reused
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed())
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(100, 100);
    LUT_SUB_ASSERT_TRUE(dummy.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(dummy.mouseGetGrabX() == 15 && dummy.mouseGetGrabY() == 15)
    debugSetLMB(0);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse down somewhere else not on element ==> not grabbed
    debugSetMousePos(11, 11);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseGrabbed()) //mouse still down and moving over element, but it wasn't down HERE, so not grabbed*/
  LUT_CASE_END
  
  //the fact that the container and the window handles the elements, influences the result
  LUT_CASE("mouseGrabbed on top of window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
 
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed()) //mouse not down yet
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse down on element so it's grabbed
    debugSetMousePos(200, 200);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse moved away but still down ==> still grabbed
    LUT_SUB_ASSERT_TRUE(w.top.mouseGetGrabX() == mx && w.top.mouseGetGrabY() == my)
    debugSetLMB(0);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(!w.top.mouseGrabbed()) //mouse button up ==> not grabbed anymore
    debugSetLMB(1);
    c.handle();c.handle();
    LUT_SUB_ASSERT_TRUE(w.top.mouseGrabbed()) //mouse down somewhere else not on element ==> normally not grabbed, BUT, the window moves!! ==> under mouse and grabbed again
  LUT_CASE_END
  
  LUT_CASE("dragging a window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    w.addTop();
    c.handle();c.handle();
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    int wx1 = w.getX0();
    int wy1 = w.getY0();
    
    debugSetMousePos(mx, my); //mouse above top bar of the window
    debugSetLMB(1);
    c.handle();c.handle();
    
    int wx2 = w.getX0();
    int wy2 = w.getY0();
    
    LUT_SUB_ASSERT_TRUE(wx1 == wx2 && wy1 == wy2)
    
    debugSetMousePos(mx + 100, my + 50);
    c.handle();c.handle();
    int wx3 = w.getX0();
    int wy3 = w.getY0();
    
    LUT_SUB_ASSERT_FALSE(wx3 == wx1 && wy3 == wy1)
    LUT_SUB_ASSERT_TRUE(wx3 == wx1 + 100 && wy3 == wy1 + 50)
    LUT_APPEND_MSG << "wx1: " << wx1 << "wy1: " << wy1 << "wx2: " << wx2 << "wy2: " << wy2 << "wx3: " << wx3 << "wy3: " << wy3;
  LUT_CASE_END
  
  //functions like mouseJustDown and mouseJustDownHere may not influence each other's result, so both can return true once
  LUT_CASE("mouse independence")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(10, 10); //mouse above
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
    debugSetLMB(1); //press button
    //now both must return true
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(dummy.mouseJustDownHere())
    //but of course now both must return false
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDown())
    LUT_SUB_ASSERT_TRUE(!dummy.mouseJustDownHere())
  LUT_CASE_END
  
  //when boolean mouseActive is true, mouseOver should work, when it's false, mouseOver shout NOT work
  LUT_CASE("mouseOver when active and not active")
    Dummy dummy;
    dummy.resize(0, 0, 50, 50);
    debugSetMousePos(1, 1);
    LUT_SUB_ASSERT_TRUE(dummy.mouseOver())
    dummy.setElementOver(true);
    LUT_SUB_ASSERT_TRUE(!dummy.mouseOver())
    //note: containers like a gui::Window have to take control of the mouseActive boolean of the elements in it
  LUT_CASE_END
    
  
  //the selfActivate system (used by text input boxes) didn't work anymore, so I added this test to make sure it won't break anymore
  LUT_CASE("selfActivate of InputLine")
    InputLine line;
    line.make(0, 0, 10);
    
    //initially it's not active
    line.handle(); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //move mouse above it and click
    debugSetMousePos(1, 1);
    debugSetLMB(1);
    line.handle();
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    line.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    line.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    line.handle();
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  LUT_CASE("selfActivate of InputLine in Window")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    InputLine line;
    line.make(0, 0, 10);
    w.pushTop(&line, 0, 0);
    
    //initially it's not active
    c.handle(); //handle line all the time to let the selfActivate system work
    LUT_SUB_ASSERT_TRUE(!line.isActive())
    
    //now a little test to see if it's at the correct position for these tests below
    debugSetMousePos(line.getCenterx(), line.getCentery());
    line.setActive(true);
    line.setElementOver(false);
    LUT_SUB_ASSERT_TRUE(line.mouseOver())
    line.setElementOver(true);
    line.setActive(false);
    
    //move mouse above it and click
    debugSetMousePos(line.getCenterx(), line.getCentery());
    debugSetLMB(1);
    c.handle();
    //it must be active now!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //now release the mouse button
    debugSetLMB(0);
    c.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away
    debugSetMousePos(100, 100);
    c.handle();
    //it should still be active!
    LUT_SUB_ASSERT_TRUE(line.isActive())
    
    //move mouse away from it and click
    debugSetMousePos(100, 100);
    debugSetLMB(1);
    c.handle();
    //it may not be active anymore after clicking on a location outside (???????)
    LUT_SUB_ASSERT_TRUE(!line.isActive())
  LUT_CASE_END
  
  //two things on a container, both at the same location, first the one is totallyEnabled and the other totallyDisabled, then vica versa, then each time only the enabled one may return true to mouseOver
  LUT_CASE("enabled and disabled on each other")
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    c.pushTop(&w, 0, 0);
    
    Dummy dummy1;
    dummy1.resize(0, 0, 50, 50);
    w.pushTop(&dummy1, 0, 0);
    
    Dummy dummy2;
    dummy2.resize(0, 0, 50, 50);
    w.pushTop(&dummy2, 0, 0);
    
    debugSetMousePos(dummy1.getCenterx(), dummy1.getCentery()); //doesn't matter if you take center of dummy1 or dummy2, both are exactly at same position
    
    dummy1.totallyEnable();
    dummy2.totallyDisable();
    
    LUT_SUB_ASSERT_TRUE(dummy1.mouseOver())
    LUT_SUB_ASSERT_TRUE(!dummy2.mouseOver())
    
    dummy1.totallyDisable();
    dummy2.totallyEnable();
    
    LUT_SUB_ASSERT_TRUE(!dummy1.mouseOver())
    LUT_SUB_ASSERT_TRUE(dummy2.mouseOver())
  LUT_CASE_END
  
  //dragging a window that has a top
  LUT_CASE("dragging of a window")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop();
    c.pushTop(&w, 0, 0);
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle();
    
    //move
    debugSetMousePos(200, 200);
    
    c.handle();
    
    //now the window must still be below the mouse
    LUT_SUB_ASSERT_TRUE(w.mouseOver())  
  LUT_CASE_END
  
  //when there are two windows, A and B, and you're dragging one over the other, but you drag the mouse fast and the mousepointer goes to a location not over the dragged window (the dragged window will be under the mouse again soon), then the other window may not swap to top
  LUT_CASE("fast dragging of one window over another")
  
    Container c;
    Window A;
    A.make(0, 0, 100, 100);
    A.addTop();
    c.pushTopAt(&A, 0, 0);
    
    Window B;
    B.make(0, 0, 100, 100);
    B.addTop();
    c.pushTopAt(&B, 200, 0);
    
    int ax = A.top.getCenterx();
    int ay = A.top.getCentery();
    int bx = B.top.getCenterx();
    int by = B.top.getCentery();
    
    //grab the window A
    debugSetMousePos(ax, ay);
    debugSetLMB(1);
    
    c.handle();
    
    //move it over window B
    debugSetMousePos(bx, by);
    
    c.handle();
    
    //now window A must be over window B, so window A must have mouseOver, window B not
    
    LUT_SUB_ASSERT_TRUE(A.mouseOver())
    LUT_SUB_ASSERT_TRUE(!B.mouseOver())
  
  LUT_CASE_END
  
  //dragging a window that has a top that is shifted: when starting to drag, the window may not "jump" (but I once had)
  LUT_CASE("dragging of a window with shifted top")
  
    Container c;
    Window w;
    w.make(0, 0, 100, 100);
    w.addTop(&lpi::gui::builtInTexture[47], 2, 2, 2); //could cause jump of two pixels when the bug was there
    c.pushTop(&w, 0, 0);
    
    int mx = w.top.getCenterx();
    int my = w.top.getCentery();
    
    //grab the window
    debugSetMousePos(mx, my);
    debugSetLMB(1);
    
    c.handle();
    
    LUT_SUB_ASSERT_TRUE(w.getX0() == 0 && w.getY0() == 0); //NOT shifted over two pixels
  LUT_CASE_END

  LUT_END_UNIT_TEST
}

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

