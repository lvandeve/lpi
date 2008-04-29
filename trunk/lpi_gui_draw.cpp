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


#include "lpi_gui_draw.h"

#include "lodepng.h"
#include "lpi_gl.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include <iostream>

namespace lpi
{
namespace gui
{

Texture builtInTexture[128];
BackPanel builtInPanel[4];
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

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

