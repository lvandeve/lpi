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

#include "lpi_gui_color.h"

#include "lpi_base64.h"
#include "lpi_parse.h"
#include "lodepng.h"

#include <cstdlib> //abs(int)
#include <iostream>
#include <algorithm>
#include <string>

namespace lpi
{
namespace gui
{

void drawCheckerBackground(IGUIDrawer& drawer, int x0, int y0, int x1, int y1, int w, int h, const ColorRGB& c0, const ColorRGB& c1)
{
  static ITexture* texture0 = 0;
  static ITexture* texture1 = 0;
  if(texture0 == 0)
  {
    texture0 = drawer.createTexture();
    texture0->setSize(2, 2);
    setPixel(texture0, 0, 0, RGB_White);
    setPixel(texture0, 0, 1, RGB_Invisible);
    setPixel(texture0, 1, 0, RGB_Invisible);
    setPixel(texture0, 1, 1, RGB_White);
    texture0->update();
  }
  if(texture1 == 0)
  {
    texture1 = drawer.createTexture();
    texture1->setSize(2, 2);
    setPixel(texture1, 0, 0, RGB_Invisible);
    setPixel(texture1, 0, 1, RGB_White);
    setPixel(texture1, 1, 0, RGB_White);
    setPixel(texture1, 1, 1, RGB_Invisible);
    texture1->update();
  }
  drawer.convertTextureIfNeeded(texture0);
  drawer.convertTextureIfNeeded(texture1);
  
  drawer.drawTextureSizedRepeated(texture0, x0, y0, x1, y1, w * 2, h * 2, c0);
  drawer.drawTextureSizedRepeated(texture1, x0, y0, x1, y1, w * 2, h * 2, c1);
  
  
  //Rectangle method below is too slow.
  
  /*bool bx = true, by = true;
  
  for(int x = x0; x < x1; x += w)
  {
    bx = !bx;
    by = true;
    
    for(int y = y0; y < y1; y += h)
    {
      by = !by;
      bool b = (bx != by);
      
      int xb = x + w;
      int yb = y + h;
      if(xb > x1) xb = x1;
      if(yb > y1) yb = y1;
      
      drawer.drawRectangle(x, y, xb, yb, b ? c0 : c1, true);
    }
  }*/
}

PartialEditor::PartialEditor()
{
}

ColorRGB getIndicatorColor(const ColorRGB& color) //returns a color that is always well visible on the given background color
{
  //algorithm one. Best visible, but the least continuous.
  /*ColorRGB result;
  result.r = (color.r + 128) % 256;
  result.g = (color.g + 128) % 256;
  result.b = (color.b + 128) % 256;
  result.a = 255;
  return result;*/
  
  //algorithm two. Very well visible. Not continuous, but better than algorithm one at this.
  ColorRGB result;
  if(color.r < 128) result.r = 255; else result.r = 0;
  if(color.g < 128) result.g = 255; else result.g = 0;
  if(color.b < 128) result.b = 255; else result.b = 0;
  result.a = 255;
  return result;
}

////////////////////////////////////////////////////////////////////////////////

ChannelSlider::ChannelSlider()
: dir(H)
, value(0.5)
, drawalpha(true)
, outofrangeaction(WARNING)
{
}


void ChannelSlider::drawImpl(IGUIDrawer& drawer) const
{
  (void)drawer;
  
  ColorRGB indicator;
  getDrawColor(indicator, value);
  indicator = getIndicatorColor(indicator);
  
  if(dir == H)
  {
    drawBackgroundH(drawer);
    int pos = x0 + (int)(value * getSizeX());
    if(pos <= x0) pos = x0 + 1; if(pos >= x1 - 1) pos = x1 - 2; //don't make it hidden by border...
    drawer.drawLine(pos, y0, pos, y1, indicator);
  }
  else
  {
    drawBackgroundV(drawer);
    int pos = y1 - (int)(value * getSizeY()) - 1;
    if(pos <= y0) pos = y0 + 1; if(pos >= y1 - 1) pos = y1 - 2; //don't make it hidden by border...
    drawer.drawLine(x0, pos, x1, pos, indicator);
  }

  drawer.drawRectangle(x0, y0, x1, y1, RGB_Black, false);
}

void ChannelSlider::drawBackgroundH(IGUIDrawer& drawer) const
{
  static const size_t NUM = 32;
  ColorRGB c;
  
  if(drawalpha) drawCheckerBackground(drawer, x0, y0, x1, y1, 4, 4, RGB_White, RGB_Black);
  
  for(size_t i = 0; i < NUM; i++)
  {
    int xa = 1 + x0 + ((i * getSizeX()) / NUM);
    int xb = 1 + x0 + (((i + 1) * getSizeX()) / NUM);
    if(i == NUM - 1) xb = x1 - 1;
    double v = (double)((xa + xb) / 2 - x0) / (getSizeX() - 2);
    getDrawColor(c, v);
    if(!drawalpha) c.a = 255;
    bool outofrange = !(c.r >= 0 && c.r <= 255 && c.g >= 0 && c.g <= 255 && c.b >= 0 && c.b <= 255);
    c.clamp();
    if(!outofrange || outofrangeaction == DRAW)
      drawer.drawRectangle(xa, y0, xb, y1, c, true);
    else if(outofrange && outofrangeaction == WARNING)
    {
      int yi = y0 + (3 * (y1 - y0)) / 4;
      drawer.drawRectangle(xa, y0, xb, yi, c, true);
      ColorRGB w = RGB_Red;//c; w.clamp(); w.negateRGB();
      drawer.drawRectangle(xa, yi, xb, y1, w, true);
      drawer.drawLine(xa, yi, xb, yi, RGB_Yellow);
    }
  }
}

void ChannelSlider::drawBackgroundV(IGUIDrawer& drawer) const
{
  static const size_t NUM = 32;
  ColorRGB c;
  
  if(drawalpha) drawCheckerBackground(drawer, x0, y0, x1, y1, 4, 4, RGB_White, RGB_Black);
  
  for(size_t i = 0; i < NUM; i++)
  {
    int ya = 1 + y0 + ((i * getSizeY()) / NUM);
    int yb = 1 + y0 + (((i + 1) * getSizeY()) / NUM);
    if(i == NUM - 1) yb = y1 - 1;
    double v = 1.0 - (double)((ya + yb) / 2 - y0) / (getSizeY() - 2);
    getDrawColor(c, v);
    if(!drawalpha) c.a = 255;
    bool outofrange = !(c.r >= 0 && c.r <= 255 && c.g >= 0 && c.g <= 255 && c.b >= 0 && c.b <= 255);
    c.clamp();
    if(!outofrange || outofrangeaction == DRAW)
      drawer.drawRectangle(x0, ya, x1, yb, c, true);
    else if(outofrange && outofrangeaction == WARNING)
    {
      int xi = x0 + (3 * (x1 - x0)) / 4;
      drawer.drawRectangle(x0, ya, xi, yb, c, true);
      ColorRGB w = RGB_Red;//c; w.clamp(); w.negateRGB();
      drawer.drawRectangle(xi, ya, x1, yb, w, true);
      drawer.drawLine(xi, ya, xi, yb, RGB_Yellow);
    }
  }
}

void ChannelSlider::handleImpl(const IInput& input)
{
  if(mouseGrabbed(input))
  {
    if(dir == H)
      value = mouseGetRelPosX(input) / (double)(getSizeX());
    else
      value = (getSizeY() - mouseGetRelPosY(input) - 1) / (double)(getSizeY());
    
    if(value < 0.0) value = 0.0;
    if(value > 1.0) value = 1.0;
    
    setChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

void getColorDynamic(ColorRGB& o_color, const ColorRGB& color, double value, ColorChannelType type)
{
  switch(type)
  {
    case CC_RGB_R:
    case CC_RGB_G:
    case CC_RGB_B:
      {
        o_color = color;
        if(type == CC_RGB_R) o_color.r = (int)(value * 255.0);
        else if(type == CC_RGB_G) o_color.g = (int)(value * 255.0);
        else if(type == CC_RGB_B) o_color.b = (int)(value * 255.0);
      }
      break;
    case CC_HSV_H:
    case CC_HSV_S:
    case CC_HSV_V:
      {
        ColorHSV convert = RGBtoHSV(color);
        if(type == CC_HSV_H) convert.h = (int)(value * 255.0);
        else if(type == CC_HSV_S) convert.s = (int)(value * 255.0);
        else if(type == CC_HSV_V) convert.v = (int)(value * 255.0);
        o_color = HSVtoRGB(convert);
      }
      break;
    case CC_HSL_H:
    case CC_HSL_S:
    case CC_HSL_L:
      {
        ColorHSL convert = RGBtoHSL(color);
        if(type == CC_HSL_H) convert.h = (int)(value * 255.0);
        else if(type == CC_HSL_S) convert.s = (int)(value * 255.0);
        else if(type == CC_HSL_L) convert.l = (int)(value * 255.0);
        o_color = HSLtoRGB(convert);
      }
      break;
    case CC_CMY_C:
    case CC_CMY_M:
    case CC_CMY_Y:
      {
        ColorCMY convert = RGBtoCMY(color);
        if(type == CC_CMY_C) convert.c = (int)(value * 255.0);
        else if(type == CC_CMY_M) convert.m = (int)(value * 255.0);
        else if(type == CC_CMY_Y) convert.y = (int)(value * 255.0);
        o_color = CMYtoRGB(convert);
      }
      break;
    case CC_CMYK_C:
    case CC_CMYK_M:
    case CC_CMYK_Y:
    case CC_CMYK_K:
      {
        ColorCMYK convert = RGBtoCMYK(color);
        if(type == CC_CMYK_C) convert.c = (int)(value * 255.0);
        else if(type == CC_CMYK_M) convert.m = (int)(value * 255.0);
        else if(type == CC_CMYK_Y) convert.y = (int)(value * 255.0);
        else if(type == CC_CMYK_K) convert.k = (int)(value * 255.0);
        o_color = CMYKtoRGB(convert);
      }
      break;
    case CC_CIEXYZ_X:
    case CC_CIEXYZ_Y:
    case CC_CIEXYZ_Z:
      {
        ColorCIEXYZ convert = RGBtoCIEXYZ(color);
        if(type == CC_CIEXYZ_X) convert.x = (int)(value * 255.0);
        else if(type == CC_CIEXYZ_Y) convert.y = (int)(value * 255.0);
        else if(type == CC_CIEXYZ_Z) convert.z = (int)(value * 255.0);
        o_color = CIEXYZtoRGB(convert);
      }
      break;
    case CC_CIELab_L:
    case CC_CIELab_a:
    case CC_CIELab_b:
      {
        ColorCIELab convert = RGBtoCIELab(color);
        if(type == CC_CIELab_L) convert.l = (int)(value * 255.0);
        else if(type == CC_CIELab_a) convert.a = (int)((2.0 * value - 1.0) * 255.0);
        else if(type == CC_CIELab_b) convert.b = (int)((2.0 * value - 1.0) * 255.0);
        o_color = CIELabtoRGB(convert);
      }
      break;
    case CC_YPbPr_Y:
    case CC_YPbPr_Pb:
    case CC_YPbPr_Pr:
      {
        ColorYPbPr convert = RGBtoYPbPr(color);
        if(type == CC_YPbPr_Y) convert.y = (int)(value * 255.0);
        else if(type == CC_YPbPr_Pb) convert.pb = (int)((value - 0.5) * 255.0);
        else if(type == CC_YPbPr_Pr) convert.pr = (int)((value - 0.5) * 255.0);
        o_color = YPbPrtoRGB(convert);
      }
      break;
    case CC_YCbCr_Y:
    case CC_YCbCr_Cb:
    case CC_YCbCr_Cr:
      {
        ColorYCbCr convert = RGBtoYCbCr(color);
        if(type == CC_YCbCr_Y) convert.y = (int)(value * 255.0);
        else if(type == CC_YCbCr_Cb) convert.cb = (int)(value * 255.0);
        else if(type == CC_YCbCr_Cr) convert.cr = (int)(value * 255.0);
        o_color = YCbCrtoRGB(convert);
      }
      break;
    case CC_A:
      {
        o_color = color;
        o_color.a = (int)(value * 255.0);
      }
      break;
    default:
      {
      }
      break;
  }
}

void getColorDynamic(ColorRGBd& o_color, const ColorRGBd& color, double value, ColorChannelType type)
{
  switch(type)
  {
    case CC_RGB_R:
    case CC_RGB_G:
    case CC_RGB_B:
      {
        o_color = color;
        if(type == CC_RGB_R) o_color.r = value;
        else if(type == CC_RGB_G) o_color.g = value;
        else if(type == CC_RGB_B) o_color.b = value;
      }
      break;
    case CC_HSV_H:
    case CC_HSV_S:
    case CC_HSV_V:
      {
        ColorHSVd convert = RGBtoHSV(color);
        if(type == CC_HSV_H) convert.h = value;
        else if(type == CC_HSV_S) convert.s = value;
        else if(type == CC_HSV_V) convert.v = value;
        o_color = HSVtoRGB(convert);
      }
      break;
    case CC_HSL_H:
    case CC_HSL_S:
    case CC_HSL_L:
      {
        ColorHSLd convert = RGBtoHSL(color);
        if(type == CC_HSL_H) convert.h = value;
        else if(type == CC_HSL_S) convert.s = value;
        else if(type == CC_HSL_L) convert.l = value;
        o_color = HSLtoRGB(convert);
      }
      break;
    case CC_CMY_C:
    case CC_CMY_M:
    case CC_CMY_Y:
      {
        ColorCMYd convert = RGBtoCMY(color);
        if(type == CC_CMY_C) convert.c = value;
        else if(type == CC_CMY_M) convert.m = value;
        else if(type == CC_CMY_Y) convert.y = value;
        o_color = CMYtoRGB(convert);
      }
      break;
    case CC_CMYK_C:
    case CC_CMYK_M:
    case CC_CMYK_Y:
    case CC_CMYK_K:
      {
        ColorCMYKd convert = RGBtoCMYK(color);
        if(type == CC_CMYK_C) convert.c = value;
        else if(type == CC_CMYK_M) convert.m = value;
        else if(type == CC_CMYK_Y) convert.y = value;
        else if(type == CC_CMYK_K) convert.k = value;
        o_color = CMYKtoRGB(convert);
      }
      break;
    case CC_CIEXYZ_X:
    case CC_CIEXYZ_Y:
    case CC_CIEXYZ_Z:
      {
        ColorCIEXYZd convert = RGBtoCIEXYZ(color);
        if(type == CC_CIEXYZ_X) convert.x = value;
        else if(type == CC_CIEXYZ_Y) convert.y = value;
        else if(type == CC_CIEXYZ_Z) convert.z = value;
        o_color = CIEXYZtoRGB(convert);
      }
      break;
    case CC_CIELab_L:
    case CC_CIELab_a:
    case CC_CIELab_b:
      {
        ColorCIELabd convert = RGBtoCIELab(color);
        if(type == CC_CIELab_L) convert.l = value;
        else if(type == CC_CIELab_a) convert.a = (2.0 * value - 1.0);
        else if(type == CC_CIELab_b) convert.b = (2.0 * value - 1.0);
        o_color = CIELabtoRGB(convert);
      }
      break;
    case CC_YPbPr_Y:
    case CC_YPbPr_Pb:
    case CC_YPbPr_Pr:
      {
        ColorYPbPrd convert = RGBtoYPbPr(color);
        if(type == CC_YPbPr_Y) convert.y = value;
        else if(type == CC_YPbPr_Pb) convert.pb = (value - 0.5);
        else if(type == CC_YPbPr_Pr) convert.pr = (value - 0.5);
        o_color = YPbPrtoRGB(convert);
      }
      break;
    case CC_YCbCr_Y:
    case CC_YCbCr_Cb:
    case CC_YCbCr_Cr:
      {
        ColorYCbCrd convert = RGBtoYCbCr(color);
        if(type == CC_YCbCr_Y) convert.y = value;
        else if(type == CC_YCbCr_Cb) convert.cb = value;
        else if(type == CC_YCbCr_Cr) convert.cr = value;
        o_color = YCbCrtoRGB(convert);
      }
      break;
    case CC_A:
      {
        o_color = color;
        o_color.a = value;
      }
      break;
    default:
      {
      }
      break;
  }
}

void getColorDynamic(ColorRGB& o_color, const ColorRGBd& color, double value, ColorChannelType type)
{
  getColorDynamic(o_color, ColorRGB((int)(color.r*255),(int)(color.g*255),(int)(color.b*255),(int)(color.a*255)), value, type);
}

void getColorDynamic(ColorRGBd& o_color, const ColorRGB& color, double value, ColorChannelType type)
{
  getColorDynamic(o_color, ColorRGBd(color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0), value, type);
}

void ChannelSliderType::getDrawColor(ColorRGB& o_color, double value) const
{
  getColorDynamic(o_color, color, value, type);
}

void ChannelSliderType::incrementType()
{
  if(type == CC_A)
    type = CC_RGB_R;
  else
    type = (ColorChannelType)((int)(type) + 1);
}

void ChannelSliderType::decrementType()
{
  if(type == CC_RGB_R)
    type = CC_A;
  else
    type = (ColorChannelType)((int)(type) - 1);
}

void ChannelSliderType::handleImpl(const IInput& input)
{
  ChannelSlider::handleImpl(input);
  if(mouseDoubleClicked(input, LMB)) incrementType();
  if(mouseDoubleClicked(input, RMB)) decrementType();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ChannelSliderEx::ChannelSliderEx(ChannelSlider* slider, const std::string& label, double smallest, double largest)
: slider(slider)
, smallest(smallest)
, largest(largest)
{
  static const int EDITBOXSIZEX = 52, EDITBOXSIZEY = 12;
  this->label.make(0, 3, label, FONT_Shadow);
  int LABELSIZEX = this->label.getSizeX();
  resize(0, 0, LABELSIZEX + EDITBOXSIZEX, EDITBOXSIZEY);
  input.make(0, 0, 5, FONT_Black);
  input.resize(LABELSIZEX + 6, 2, LABELSIZEX + EDITBOXSIZEX - 1, EDITBOXSIZEY - 1);
  input.cursorColor = RGB_Black;
  slider->resize(LABELSIZEX, 0, LABELSIZEX, EDITBOXSIZEY);
  addSubElement(&this->label, Sticky(0.0, 0, 0.5, -this->label.getSizeY() / 2, 0.0, this->label.getSizeX(), 0.5, this->label.getSizeY() / 2));
  addSubElement(&input, Sticky(1.0, -input.getSizeX(), 0.5, -input.getSizeY() / 2, 1.0, 0, 0.5, input.getSizeY() / 2));
  addSubElement(slider);
  ic.setStickyFull(slider, this);
}

void ChannelSliderEx::fixUpSizes()
{
}

void ChannelSliderEx::drawImpl(IGUIDrawer& drawer) const
{
  slider->draw(drawer);
  drawer.drawRectangle(input.getX0()-1, input.getY0()-1, input.getX1(), input.getY1(), RGB_White, true);
  drawer.drawRectangle(input.getX0()-2, input.getY0()-2, input.getX1()+1, input.getY1()+1, RGB_Black, false);
  input.draw(drawer);
  label.draw(drawer);
}

void ChannelSliderEx::handleImpl(const IInput& input)
{
  slider->handle(input);
  this->input.handle(input);
  
  if(slider->hasChanged())
  {
    this->input.setText(valtostr((int)(0.49 + smallest + slider->getValue() * (largest - smallest)))); //the 0.49 is for rounding
    setChanged();
  }
  else if(this->input.enteringDone())
  {
    double d = strtoval<double>(this->input.getText());
    slider->setValue((d  - smallest) / (largest - smallest));
    setChanged();
  }
}

double ChannelSliderEx::getValue() const
{
  return slider->getValue();
}

void ChannelSliderEx::setValue(double value)
{
  input.setText(valtostr((int)(0.49 + smallest + value * (largest - smallest)))); //the 0.49 is for rounding
  slider->setValue(value);
}

double ChannelSliderEx::getValueScaled() const
{
  return smallest + getValue() * (largest - smallest);
}

void ChannelSliderEx::setValueScaled(double value)
{
  setValue(value / (largest - smallest) - smallest);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void ColorSliders::fixUpSizes()
{
  int minsizey = sliders.size() * 4;
  int origsizey = getSizeY();
  /*if(getSizeY() < minsizey)
  {
    resize(x0, y0, x1, y0 + minsizey);
  }*/
  resize(x0, y0, x1, y0 + minsizey);
  for(size_t i = 0; i < sliders.size(); i++)
  {
    sliders[i]->resize(x0, y0 + 4 * i, x1, y0 + 4 * i + 3);
    //ic.updateRelativeSize(sliders[i], this);
    ic.setStickyRelative(sliders[i], this);
  }
  resize(x0, y0, x1, origsizey);
  
}

ColorSliders::ColorSliders()
: with_alpha(false)
{
  resize(0, 0, 256, 64); //sort of sloppy solution: set x-size large enough initially, because otherwise the color slider's relative sizes become corrupted
}

ColorSliders::~ColorSliders()
{
  for(size_t i = 0; i < sliders.size(); i++)
    delete sliders[i];
}

void ColorSliders::addSlider(ChannelSliderEx* slider)
{
  sliders.push_back(slider);
  addSubElement(slider);
  fixUpSizes();
}

void ColorSliders::addAlpha(const std::string& label, double smallest, double largest)
{
  addSlider(new ChannelSliderExType(CC_A, label, smallest, largest));
  with_alpha = true;
}

void ColorSliders::drawImpl(IGUIDrawer& drawer) const
{
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->draw(drawer);
}

void ColorSliders::handleImpl(const IInput& input)
{
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->handle(input);
    
  //for "adaptive color" in the sliders
  ColorRGBd adaptive;
  getColor(adaptive);
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->setAdaptiveColor(adaptive);
  
  for(size_t i = 0; i < sliders.size(); i++)
  {
    if(sliders[i]->hasChanged())
      setChanged();
  }
}

ColorSlidersRGB::ColorSlidersRGB(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_RGB_R, "R:"));
  addSlider(new ChannelSliderExType(CC_RGB_G, "G:"));
  addSlider(new ChannelSliderExType(CC_RGB_B, "B:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersRGB::getColor(ColorRGBd& color) const
{
  color.r = sliders[0]->getValue();
  color.g = sliders[1]->getValue();
  color.b = sliders[2]->getValue();
  if(with_alpha) color.a = sliders[3]->getValue();
}

void ColorSlidersRGB::setColor(const ColorRGBd& color)
{
  sliders[0]->setValue(color.r);
  sliders[1]->setValue(color.g);
  sliders[2]->setValue(color.b);
  sliders[3]->setValue(color.a);
}

ColorSlidersHSV::ColorSlidersHSV(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_HSV_H, "H:"));
  addSlider(new ChannelSliderExType(CC_HSV_S, "S:"));
  addSlider(new ChannelSliderExType(CC_HSV_V, "V:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersHSV::getColor(ColorRGBd& color) const
{
  lpi::ColorHSVd convert;
  convert.h = sliders[0]->getValue();
  convert.s = sliders[1]->getValue();
  convert.v = sliders[2]->getValue();
  convert.a = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::HSVtoRGB(convert);
}

void ColorSlidersHSV::setColor(const ColorRGBd& color)
{
  lpi::ColorHSVd convert = lpi::RGBtoHSV(color);
  sliders[0]->setValue(convert.h);
  sliders[1]->setValue(convert.s);
  sliders[2]->setValue(convert.v);
  sliders[3]->setValue(convert.a);
}

ColorSlidersHSL::ColorSlidersHSL(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_HSL_H, "H:"));
  addSlider(new ChannelSliderExType(CC_HSL_S, "S:"));
  addSlider(new ChannelSliderExType(CC_HSL_L, "L:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersHSL::getColor(ColorRGBd& color) const
{
  lpi::ColorHSLd convert;
  convert.h = sliders[0]->getValue();
  convert.s = sliders[1]->getValue();
  convert.l = sliders[2]->getValue();
  convert.a = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::HSLtoRGB(convert);
}

void ColorSlidersHSL::setColor(const ColorRGBd& color)
{
  lpi::ColorHSLd convert = lpi::RGBtoHSL(color);
  sliders[0]->setValue(convert.h);
  sliders[1]->setValue(convert.s);
  sliders[2]->setValue(convert.l);
  sliders[3]->setValue(convert.a);
}

ColorSlidersCMY::ColorSlidersCMY(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_CMY_C, "C:"));
  addSlider(new ChannelSliderExType(CC_CMY_M, "M:"));
  addSlider(new ChannelSliderExType(CC_CMY_Y, "Y:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersCMY::getColor(ColorRGBd& color) const
{
  lpi::ColorCMYd convert;
  convert.c = sliders[0]->getValue();
  convert.m = sliders[1]->getValue();
  convert.y = sliders[2]->getValue();
  convert.a = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::CMYtoRGB(convert);
}

void ColorSlidersCMY::setColor(const ColorRGBd& color)
{
  lpi::ColorCMYd convert = lpi::RGBtoCMY(color);
  sliders[0]->setValue(convert.c);
  sliders[1]->setValue(convert.m);
  sliders[2]->setValue(convert.y);
  sliders[3]->setValue(convert.a);
}

ColorSlidersCMYK::ColorSlidersCMYK(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_CMYK_C, "C:"));
  addSlider(new ChannelSliderExType(CC_CMYK_M, "M:"));
  addSlider(new ChannelSliderExType(CC_CMYK_Y, "Y:"));
  addSlider(new ChannelSliderExType(CC_CMYK_K, "K:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersCMYK::getColor(ColorRGBd& color) const
{
  lpi::ColorCMYKd convert;
  convert.c = sliders[0]->getValue();
  convert.m = sliders[1]->getValue();
  convert.y = sliders[2]->getValue();
  convert.k = sliders[3]->getValue();
  convert.a = sliders[4]->getValue();
  color = lpi::CMYKtoRGB(convert);
}

void ColorSlidersCMYK::setColor(const ColorRGBd& color)
{
  lpi::ColorCMYKd convert = lpi::RGBtoCMYK(color);
  sliders[0]->setValue(convert.c);
  sliders[1]->setValue(convert.m);
  sliders[2]->setValue(convert.y);
  sliders[3]->setValue(convert.k);
  sliders[4]->setValue(convert.a);
}

ColorSlidersCIEXYZ::ColorSlidersCIEXYZ(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_CIEXYZ_X, "X:"));
  addSlider(new ChannelSliderExType(CC_CIEXYZ_Y, "Y:"));
  addSlider(new ChannelSliderExType(CC_CIEXYZ_Z, "Z:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersCIEXYZ::getColor(ColorRGBd& color) const
{
  lpi::ColorCIEXYZd convert;
  convert.x = sliders[0]->getValue();
  convert.y = sliders[1]->getValue();
  convert.z = sliders[2]->getValue();
  convert.alpha = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::CIEXYZtoRGB(convert);
}

void ColorSlidersCIEXYZ::setColor(const ColorRGBd& color)
{
  lpi::ColorCIEXYZd convert = lpi::RGBtoCIEXYZ(color);
  sliders[0]->setValue(convert.x);
  sliders[1]->setValue(convert.y);
  sliders[2]->setValue(convert.z);
  sliders[3]->setValue(convert.alpha);
}

ColorSlidersCIELab::ColorSlidersCIELab(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_CIELab_L, "L:", 0, 255));
  addSlider(new ChannelSliderExType(CC_CIELab_a, "a:", -128, 127));
  addSlider(new ChannelSliderExType(CC_CIELab_b, "b:", -128, 127));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

void ColorSlidersCIELab::getColor(ColorRGBd& color) const
{
  lpi::ColorCIELabd convert;
  convert.l = sliders[0]->getValue();
  convert.a = (sliders[1]->getValue() * 2.0 - 1.0);
  convert.b = (sliders[2]->getValue() * 2.0 - 1.0);
  convert.alpha = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::CIELabtoRGB(convert);
}

void ColorSlidersCIELab::setColor(const ColorRGBd& color)
{
  lpi::ColorCIELabd convert = lpi::RGBtoCIELab(color);
  sliders[0]->setValue(convert.l);
  sliders[1]->setValue((convert.a) / 2.0 + 0.5);
  sliders[2]->setValue((convert.b) / 2.0 + 0.5);
  sliders[3]->setValue(convert.alpha);
}

////

ColorSlidersYPbPr::ColorSlidersYPbPr(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_YPbPr_Y, " Y:", 0, 255));
  addSlider(new ChannelSliderExType(CC_YPbPr_Pb, "Pb:", -128, 127));
  addSlider(new ChannelSliderExType(CC_YPbPr_Pr, "Pr:", -128, 127));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha(" A:");
}

void ColorSlidersYPbPr::getColor(ColorRGBd& color) const
{
  lpi::ColorYPbPrd convert;
  convert.y = sliders[0]->getValue();
  convert.pb = (sliders[1]->getValue()-0.5);
  convert.pr = (sliders[2]->getValue()-0.5);
  convert.alpha = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::YPbPrtoRGB(convert);
}

void ColorSlidersYPbPr::setColor(const ColorRGBd& color)
{
  lpi::ColorYPbPrd convert = lpi::RGBtoYPbPr(color);
  sliders[0]->setValue(convert.y);
  sliders[1]->setValue(convert.pb + 0.5);
  sliders[2]->setValue(convert.pr + 0.5);
  sliders[3]->setValue(convert.alpha);
}

ColorSlidersYCbCr::ColorSlidersYCbCr(bool with_alpha)
{
  addSlider(new ChannelSliderExType(CC_YCbCr_Y, " Y:"));
  addSlider(new ChannelSliderExType(CC_YCbCr_Cb, "Cb:"));
  addSlider(new ChannelSliderExType(CC_YCbCr_Cr, "Cr:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha(" A:");
}

void ColorSlidersYCbCr::getColor(ColorRGBd& color) const
{
  lpi::ColorYCbCrd convert;
  convert.y = sliders[0]->getValue();
  convert.cb = sliders[1]->getValue();
  convert.cr = sliders[2]->getValue();
  convert.alpha = with_alpha ? sliders[3]->getValue() : color.a;
  color = lpi::YCbCrtoRGB(convert);
}

void ColorSlidersYCbCr::setColor(const ColorRGBd& color)
{
  lpi::ColorYCbCrd convert = lpi::RGBtoYCbCr(color);
  sliders[0]->setValue(convert.y);
  sliders[1]->setValue(convert.cb);
  sliders[2]->setValue(convert.cr);
  sliders[3]->setValue(convert.alpha);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  
PartialEditorSquare::PartialEditorSquare()
: drawalpha(true)
, outofrangeaction(WARNING)
{
}

void PartialEditorSquare::drawBackground(IGUIDrawer& drawer) const
{
  static const size_t NUMX = 16;
  static const size_t NUMY = 16;
  
  for(size_t y = 0; y < NUMY; y++)
  for(size_t x = 0; x < NUMX; x++)
  {
    int drawx0 = x0 + (getSizeX() * x) / NUMX;
    int drawy0 = y0 + (getSizeY() * y) / NUMY;
    int drawx1 = x0 + (getSizeX() * (x + 1)) / NUMX;
    int drawy1 = y0 + (getSizeY() * (y + 1)) / NUMY;
    
    ColorRGB color = RGBdtoRGB(this->color);
    getDrawColor(color, (double)x / (double)NUMX, (double)y / (double)NUMY);
    bool outofrange = !(color.r >= 0 && color.r <= 255 && color.g >= 0 && color.g <= 255 && color.b >= 0 && color.b <= 255);
    color.clamp();
    
    if(!outofrange || outofrangeaction == DRAW || outofrangeaction == WARNING)
      drawer.drawRectangle(drawx0, drawy0, drawx1, drawy1, color, true);
    if(outofrange && outofrangeaction == WARNING)
    {
      drawer.drawLine(drawx0, drawy0, drawx1, drawy1, RGB_Red);
      drawer.drawLine(drawx0, drawy1, drawx1, drawy0, RGB_Yellow);
    }
  }
}

void PartialEditorSquare::drawImpl(IGUIDrawer& drawer) const
{
  drawBackground(drawer);
}

void PartialEditorSquare::handleImpl(const IInput& input)
{
  (void)input;
}

void PartialEditorSquareType::getDrawColor(ColorRGB& o_color, double value_x, double value_y) const
{
  ColorRGB temp;
  getColorDynamic(temp, color, value_x, typex);
  getColorDynamic(o_color, temp, value_y, typey);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  
PartialEditorHueDisk::PartialEditorHueDisk()
{
}

void PartialEditorHueDisk::drawImpl(IGUIDrawer& drawer) const
{
  static const size_t NUMANGLE = 24;
  static const size_t NUMAXIAL = 8;
  
  int cx = getCenterX();
  int cy = getCenterY();
  double radius = getSizeX() < getSizeY() ? getSizeX() / 2 : getSizeY() / 2;
  
  const double twopi = 2.0 * 3.14159;
  
  for(size_t a = 0; a < NUMANGLE; a++)
  {
    double angle = ((double)a / (double)NUMANGLE) * twopi;
    double angle2 = ((double)(a == NUMANGLE ? 0 : a + 1) / (double)NUMANGLE) * twopi;
    for(size_t r = 0; r < NUMAXIAL; r++)
    {
      double avoidWeirdAnnoyingSpiralEffect = 0.0;//1.0 - (double)a / (double)NUMANGLE;
      double axial = ((double)r / (double)NUMAXIAL) * (radius - avoidWeirdAnnoyingSpiralEffect);
      double axial2 = ((double)(r + 1) / (double)NUMAXIAL) * (radius - avoidWeirdAnnoyingSpiralEffect);
      int x0 = cx + (int)(std::cos(angle) * axial);
      int y0 = cy + (int)(std::sin(angle) * axial);
      int x1 = cx + (int)(std::cos(angle2) * axial);
      int y1 = cy + (int)(std::sin(angle2) * axial);
      int x2 = cx + (int)(std::cos(angle2) * axial2);
      int y2 = cy + (int)(std::sin(angle2) * axial2);
      int x3 = cx + (int)(std::cos(angle) * axial2);
      int y3 = cy + (int)(std::sin(angle) * axial2);
      
      ColorRGB temp = RGBdtoRGB(this->color);
      ColorRGB color0 = temp, color1 = temp, color2 = temp, color3 = temp;
      getDrawColor(color0, angle / twopi, axial / radius);
      getDrawColor(color1, angle2 / twopi, axial / radius);
      getDrawColor(color2, angle2 / twopi, axial2 / radius);
      getDrawColor(color3, angle / twopi, axial2 / radius);
      
      drawer.drawGradientQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, color1, color2, color3);
    }
  }
  
  int x = cx + (int)(std::cos(twopi * value_angle) * value_axial * radius);
  int y = cy + (int)(std::sin(twopi * value_angle) * value_axial * radius);
  ColorRGB indicator;
  getDrawColor(indicator, value_angle, value_axial);
  indicator = getIndicatorColor(indicator);
  drawer.drawCircle(x, y, 4, indicator, false);
}

void PartialEditorHueDisk::handleImpl(const IInput& input)
{
  if(mouseGrabbed(input))
  {
    double radius = getSizeX() < getSizeY() ? getSizeX() / 2 : getSizeY() / 2;
    
    double x = (double)(mouseGetRelPosX(input) - getSizeX() / 2) / radius;
    double y = (double)(mouseGetRelPosY(input) - getSizeY() / 2) / radius;
    
    value_axial = std::sqrt(x*x+y*y);
    if(value_axial < 0.0) value_axial = 0.0;
    if(value_axial > 1.0) value_axial = 1.0;

    const static double pi = 3.14159;
    double theta = atan2(y, x);
    value_angle = theta / (2.0 * pi);
    if(value_angle < 0.0) value_angle = 1.0 + value_angle;
    if(value_angle < 0.0) value_angle = 0.0;
    if(value_angle > 1.0) value_angle = 1.0;
    
    setChanged();
  }
}

void PartialEditorHueDisk_HSV_HS::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSV convert = RGBtoHSV(RGBdtoRGB(color));
  convert.h = (int)(value_angle * 255.0);
  convert.s = (int)(value_axial * 255.0);
  o_color = HSVtoRGB(convert);
}

void PartialEditorHueDisk_HSV_HV::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSV convert = RGBtoHSV(RGBdtoRGB(color));
  convert.h = (int)(value_angle * 255.0);
  convert.v = (int)(value_axial * 255.0);
  o_color = HSVtoRGB(convert);
}

void PartialEditorHueDisk_HSL_HS::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSL convert = RGBtoHSL(RGBdtoRGB(color));
  convert.h = (int)(value_angle * 255.0);
  convert.s = (int)(value_axial * 255.0);
  o_color = HSLtoRGB(convert);
}

void PartialEditorHueDisk_HSL_HL::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSL convert = RGBtoHSL(RGBdtoRGB(color));
  convert.h = (int)(value_angle * 255.0);
  convert.l = (int)(value_axial * 255.0);
  o_color = HSLtoRGB(convert);
}

////////////////////////////////////////////////////////////////////////////////

HueDiskEditor::HueDiskEditor()
: circle(0)
, sliderc(0)
, slidera(0)
{
}

HueDiskEditor::~HueDiskEditor()
{
  delete circle;
  delete sliderc;
  delete slidera;
}

////////////////////////////////////////////////////////////////////////////////

PColorPlane::PColorPlane(ColorRGB* color)
: color(color)
{
}

void PColorPlane::drawImpl(IGUIDrawer& drawer) const
{
  int checkersize = 8;
  if(getSizeX() < 16 && getSizeY() < 16) checkersize = getSizeX() / 2;
  bool largeenough = getSizeX() > 4 && getSizeY() > 4;
  if(largeenough) drawCheckerBackground(drawer, x0, y0, x1, y1, checkersize, checkersize);
  drawer.drawRectangle(x0, y0, x1, y1, *color, true);
  if(largeenough) drawer.drawGUIPart(GP_PANEL_BORDER, x0, y0, x1, y1);
}

////////////////////////////////////////////////////////////////////////////////

ColorPlane::ColorPlane()
: PColorPlane(&color)
, color(RGB_White)
{
}

////////////////////////////////////////////////////////////////////////////////

PColorPlaned::PColorPlaned(ColorRGBd* color)
: color(color)
{
}

void PColorPlaned::drawImpl(IGUIDrawer& drawer) const
{
  int checkersize = 8;
  if(getSizeX() < 16 && getSizeY() < 16) checkersize = getSizeX() / 2;
  bool largeenough = getSizeX() > 4 && getSizeY() > 4;
  if(largeenough) drawCheckerBackground(drawer, x0, y0, x1, y1, checkersize, checkersize);
  drawer.drawRectangle(x0, y0, x1, y1, RGBdtoRGB(*color), true);
  if(largeenough) drawer.drawGUIPart(GP_PANEL_BORDER, x0, y0, x1, y1);
}

void PColorPlaned::getColor(ColorRGBd& color) const
{
  color = *(this->color);
}

void PColorPlaned::setColor(const ColorRGBd& color)
{
  *(this->color) = color;
}

////////////////////////////////////////////////////////////////////////////////

ColorPlaned::ColorPlaned()
: PColorPlaned(&color)
, color(RGBd_White)
{
}

////////////////////////////////////////////////////////////////////////////////

InternalColorPlane::InternalColorPlane()
: selected(false)
{
}

void InternalColorPlane::drawImpl(IGUIDrawer& drawer) const
{
  int checkersize = 8;
  if(getSizeX() < 16 && getSizeY() < 16) checkersize = getSizeX() / 2;
  bool largeenough = getSizeX() > 4 && getSizeY() > 4;
  if(largeenough) drawCheckerBackground(drawer, x0, y0, x1, y1, checkersize, checkersize);
  drawer.drawRectangle(x0, y0, x1, y1, RGBdtoRGB(color), true);
  if(largeenough) drawer.drawGUIPart(selected ? GP_INVISIBLE_BUTTON_PANEL_DOWN : GP_INVISIBLE_BUTTON_PANEL_UP, x0, y0, x1, y1);
}

void InternalColorPlane::handleImpl(const IInput& input)
{
  (void)input;
}

////////////////////////////////////////////////////////////////////////////////

void FGBGColor::Arrows::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawGUIPart(GP_FG_BG_COLOR_ARROWS, x0, y0, x1, y1);
}



FGBGColor::FGBGColor()
: selectable(true)
{
  addSubElement(&fg, Sticky(0.0, 0, 0.0, 0, 1.0, -16, 1.0, -16));
  addSubElement(&bg, Sticky(0.0, 16, 0.0, 16, 1.0, 0, 1.0, 0));
  addSubElement(&arrows, Sticky(0.66, 0, 0.0, 0, 1.0, 0, 0.33, 0));
  fg.selected = true;
}

void FGBGColor::drawImpl(IGUIDrawer& drawer) const
{
  bg.draw(drawer);
  fg.draw(drawer);
  arrows.draw(drawer);
}

void FGBGColor::handleImpl(const IInput& input)
{
  if(dialog && dialog->isEnabled())
  {
    if(dialog->pressedOk(input))
    {
      dialog->setEnabled(false);
      if(fg.selected)  dialog->getColor(fg.color);
      else  dialog->getColor(bg.color);
      setChanged();
    }
  }
  else
  {
    if(selectable)
    {
      if(fg.mouseDownHere(input))
      {
        fg.selected = true;
        bg.selected = false;
        setChanged();
      }
      else if(bg.mouseDownHere(input))
      {
        fg.selected = false;
        bg.selected = true;
        setChanged();
      }
    }
    
    if(dialog)
    {
      if(selectable ? fg.mouseDoubleClicked(input, LMB) : fg.clicked(input, LMB))
      {
        fg.selected = true;
        bg.selected = false;
        dialog->setColor(fg.color);
        dialog->setEnabled(true);
      }
      else if(selectable ? bg.mouseDoubleClicked(input, LMB) : bg.clicked(input, LMB))
      {
        fg.selected = false;
        bg.selected = true;
        dialog->setColor(bg.color);
        dialog->setEnabled(true);
      }
    }

    if(arrows.clicked(input))
    {
      ColorRGBd temp = fg.color;
      fg.color = bg.color;
      bg.color = temp;
      setChanged();
    }
  }
}

void FGBGColor::setFG(const ColorRGBd& color)
{
  fg.color = color;
}

void FGBGColor::setBG(const ColorRGBd& color)
{
  bg.color = color;
}

ColorRGBd FGBGColor::getFG() const
{
  return fg.color;
}

ColorRGBd FGBGColor::getBG() const
{
  return bg.color;
}

bool FGBGColor::selectedFG() const
{
  return fg.selected;
}

bool FGBGColor::selectedBG() const
{
  return bg.selected;
}

void FGBGColor::getColor(ColorRGBd& color) const
{
  color = fg.selected? fg.color : bg.color;
}

void FGBGColor::setColor(const ColorRGBd& color)
{
  if(fg.selected) fg.color = color; else bg.color = color;
}

void FGBGColor::setColorChoosingDialog(AColorDialog* dialog)
{
  this->dialog = dialog;
  if(dialog) dialog->setEnabled(false);
}

void FGBGColor::manageHoverImpl(IHoverManager& hover)
{
  if(dialog && dialog->isEnabled())
    hover.addHoverElement(dialog);
}

////////////////////////////////////////////////////////////////////////////////

ColorEditorSynchronizer::ColorEditorSynchronizer()
: mainColorLink(ColorEditor::FG)
{
}

void ColorEditorSynchronizer::add(ColorEditor* editor)
{
  editors.push_back(editor);
  if(editor->isMultiColorLinker()) mainColorLink = editor->getMultiColorLink();
}

void ColorEditorSynchronizer::remove(ColorEditor* editor)
{
  editors.erase(std::remove(editors.begin(), editors.end(), editor), editors.end());
}

void ColorEditorSynchronizer::clear()
{
  editors.clear();
}

void ColorEditorSynchronizer::handle()
{
  for(size_t i = 0; i < editors.size(); i++)
  {
    ColorEditor* editor = editors[i];
    if(editors[i]->hasChanged())
    {
      if(editor->isMultiColorLinker())
      {
        mainColorLink = editor->getMultiColorLink();
        for(size_t j = 0; j < editors.size(); j++)
        {
          if(j != i && editors[j]->isMultiColorLinker()) editors[j]->setMultiColorLink(mainColorLink);
        }
      }
      
      
      if(editor->isMainColorGettable())
      {
        ColorRGBd color;
        editor->getColor(color);
        for(size_t j = 0; j < editors.size(); j++)
        {
          if(j != i)
          {
            editors[j]->setColor(color);
            editors[j]->setMultiColor(mainColorLink, color);
          }
        }
      }
      if(editor->isMultiColorGettable(ColorEditor::FG))
      {
        ColorRGBd fg;
        editor->getMultiColor(fg, ColorEditor::FG);
        for(size_t j = 0; j < editors.size(); j++)
        {
          if(j != i)
          {
            editors[j]->setMultiColor(ColorEditor::FG, fg);
            if(mainColorLink == ColorEditor::FG) editors[j]->setColor(fg);
          }
        }
      }
      if(editor->isMultiColorGettable(ColorEditor::MG))
      {
        ColorRGBd mg;
        editor->getMultiColor(mg, ColorEditor::MG);
        for(size_t j = 0; j < editors.size(); j++)
        {
          if(j != i)
          {
            editors[j]->setMultiColor(ColorEditor::MG, mg);
            if(mainColorLink == ColorEditor::MG) editors[j]->setColor(mg);
          }
        }
      }
      if(editor->isMultiColorGettable(ColorEditor::BG))
      {
        ColorRGBd bg;
        editor->getMultiColor(bg, ColorEditor::BG);
        for(size_t j = 0; j < editors.size(); j++)
        {
          if(j != i)
          {
            editors[j]->setMultiColor(ColorEditor::BG, bg);
            if(mainColorLink == ColorEditor::BG) editors[j]->setColor(bg);
          }
        }
      }
      break;
    }
  }
}

void ColorEditorSynchronizer::setColor(const ColorRGBd& color)
{
  for(size_t i = 0; i < editors.size(); i++)
  {
    editors[i]->setColor(color);
  }
}

////////////////////////////////////////////////////////////////////////////////

AColorPalette::AColorPalette()
: dialog(0)
{
}

AColorPalette::~AColorPalette()
{
}

void AColorPalette::generateDefault()
{
  setPaletteSize(4, 2);
  setColor(0, RGBd_Black);
  setColor(1, RGBd_White);
  setColor(2, RGBd_Gray);
  setColor(3, RGBd_Grey);
  setColor(4, RGBd_Red);
  setColor(5, RGBd_Green);
  setColor(6, RGBd_Blue);
  setColor(7, RGBd_Yellow);
}

void AColorPalette::generateVibrant16x16()
{
  setPaletteSize(16, 16);
  static const std::string palette = "\
iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAABU0lEQVR42pWQMU4DMRBFv8VKHCVX\n\
SUGZMlS5RAp6zkANxd6BFDkCBcUiUbjwSi6MNIhBTJSRjD3ORkEKEsz+/Xpj747HA9Toy4O+7436\n\
Bbbb7aJGLs8i52yU7XUpJSJi5uZH+C11GXgD+Jzo3KLLwwDvEUL1U+iXoADyRSMjNgk6zFDVYgfo\n\
AcfwEBNiRKTqKR7+cU95yQgEn+BpBEWwba9XQ/h5tLl3OLlE94dLuCFnDwTAY+dxaVCFcbSGJqXW\n\
X3TLTQ4ML/C1pPVkGm5WPlgfYWrIwAH/G2yX8zU+GUJgBhPEnOXuKrTBJJsTI5lzh5d7TJc4hTnG\n\
CCQgTmrscnytVYtUDiACbWeWA+ldVERZlQuous16XnNLSpSNAu2LAmTrlUu1krJ0j7czIWnSCYQb\n\
XwhloX31veiHype6gdkzB+YhpWeRkkLV1wGYauXSoR71Df2Cwy3C/XOmAAAAAElFTkSuQmCC\n\
";

  std::vector<unsigned char> png;
  decodeBase64(png, palette);
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> data;
  pngdec.decode(data, png);

  for(size_t i = 0; i < 256; i++)
  {
    setColor(i, RGBtoRGBd(ColorRGB(data[i * 4 + 0],data[i * 4 + 1],data[i * 4 + 2],data[i * 4 + 3])));
  }
}

void AColorPalette::generateVibrant8x8()
{
  setPaletteSize(8, 8);
  static const std::string palette = "\
iVBORw0KGgoAAAANSUhEUgAAAAgAAAAICAYAAADED76LAAAAtklEQVQYlT2MMWrDUBAFR0RFbuAc\n\
Z/ukTOkjGHKFPYQLHyFHSPkLlS4UUKFCxQo28A0KqBGuzEvjuHgwMMPDXQIJXGBySUjCXZgJlwsh\n\
HGFI98QdmSF0v/gEGfx7OS7D1EjSPM9ExGOllAc3qpPIAbLnMo0My0rUlXFZGZaV9nvn5K5jegmy\n\
hafXD7IE2V2JItrn7cTxfOZrHCGC09uBGz0bPRsDjbQXvwk1YUmo7xAdPxn0Ff4AWj+M3ac/TB8A\n\
AAAASUVORK5CYII=\n\
";

  std::vector<unsigned char> png;
  decodeBase64(png, palette);
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> data;
  pngdec.decode(data, png);

  for(size_t i = 0; i < 64; i++)
  {
    setColor(i, RGBtoRGBd(ColorRGB(data[i * 4 + 0],data[i * 4 + 1],data[i * 4 + 2],data[i * 4 + 3])));
  }
}

void AColorPalette::generateVibrant6x6()
{
  setPaletteSize(6, 6);
  static const std::string palette = "\
iVBORw0KGgoAAAANSUhEUgAAAAYAAAAGCAYAAADgzO9IAAAAZklEQVQImTWLoRHDMADE5EmSUTxE\n\
B/pxAjtEYUFBQUCBgXuX3hl4A5UkQEQ6kSgoxKioJBIjIkHPlCCeywaXN8QCWGvlYl1XlmWhOKoc\n\
HUaH4wbtwbc3is+79Be/z857TNox2cfkD1/gTTo56eXqAAAAAElFTkSuQmCC\n\
";

  std::vector<unsigned char> png;
  decodeBase64(png, palette);
  LodePNG::Decoder pngdec;
  std::vector<unsigned char> data;
  pngdec.decode(data, png);

  for(size_t i = 0; i < 36; i++)
  {
    setColor(i, RGBtoRGBd(ColorRGB(data[i * 4 + 0],data[i * 4 + 1],data[i * 4 + 2],data[i * 4 + 3])));
  }
}

void AColorPalette::setColorChoosingDialog(AColorDialog* dialog)
{
  this->dialog = dialog;
  if(dialog) dialog->setEnabled(false);
}

void AColorPalette::manageHoverImpl(IHoverManager& hover)
{
  if(dialog && dialog->isEnabled())
    hover.addHoverElement(dialog);
}

////////////////////////////////////////////////////////////////////////////////

ColorPalette::ColorPalette()
{
  selected = -1;
}

ColorPalette::~ColorPalette()
{
  clear();
}

void ColorPalette::clear()
{
  clearSubElements();
  for(size_t i = 0; i < colors.size(); i++)
  {
    delete colors[i];
  }
  colors.clear();
  selected = -1;
}


void ColorPalette::setPaletteSize(size_t n, size_t m)
{
  clear();
  for(size_t y = 0; y < m; y++)
  for(size_t x = 0; x < n; x++)
  {
    colors.push_back(new InternalColorPlane);
    double x0 = (double)(x + 0) / (double)n;
    double y0 = (double)(y + 0) / (double)m;
    double x1 = (double)(x + 1) / (double)n;
    double y1 = (double)(y + 1) / (double)m;
    addSubElement(colors.back(), Sticky(x0, 0, y0, 0, x1, 0, y1, 0));
  }
}

void ColorPalette::setColor(int i, const ColorRGBd& color)
{
  colors[i]->color = color;
}


void ColorPalette::drawImpl(IGUIDrawer& drawer) const
{
  for(size_t i = 0; i < colors.size(); i++) colors[i]->draw(drawer);
}

void ColorPalette::handleImpl(const IInput& input)
{
  if(dialog && dialog->isEnabled())
  {
    if(dialog->pressedOk(input))
    {
      dialog->setEnabled(false);
      if(selected >= 0)
      {
        ColorRGBd temp;
        dialog->getColor(temp);
        setColor(selected, temp);
      }
      setChanged();
    }
  }
  else
  {
    for(size_t i = 0; i < colors.size(); i++)
    {
      if(colors[i]->pressed(input))
      {
        if(selected >= 0) colors[selected]->selected = false;
        colors[i]->selected = true;
        selected = i;
        setChanged();
        break;
      }
      if(dialog && colors[i]->mouseDoubleClicked(input, LMB))
      {
        selected = i;
        dialog->setColor(colors[i]->color);
        dialog->setEnabled(true);
      }
    }
  }
}

//these are from the ColorEditor interface, returns the currently selected color
void ColorPalette::getColor(ColorRGBd& color) const
{
  if(selected >= 0) color = colors[selected]->color;
}

void ColorPalette::setColor(const ColorRGBd& color)
{
  (void)color; //ignore setColor! Palette has only a limited amount of colors.
  if(selected >= 0) colors[selected]->selected = false;
}

////////////////////////////////////////////////////////////////////////////////

MultiColorPalette::MultiColorPalette()
: texture(new ITexture*)
, m(0)
, n(0)
, textureuptodate(false)
, selectedfg(-1)
, selectedmg(-1)
, selectedbg(-1)
, validfg(false)
, validmg(false)
, validbg(false)
, selectedEditing(-1)
{
  *texture = 0;
}

MultiColorPalette::~MultiColorPalette()
{
  delete *texture;
  delete texture;
}

void MultiColorPalette::clear()
{
  selectedfg = -1;
  selectedmg = -1;
  selectedbg = -1;
  selectedEditing = -1;
  n = m = 0;
  colors.clear();
  textureuptodate = false;
}

void MultiColorPalette::drawImpl(IGUIDrawer& drawer) const
{
  if(*texture == 0) (*texture) = drawer.createTexture();
  if(!textureuptodate) generateTexture();
  drawer.convertTextureIfNeeded((*texture));
  drawer.drawTextureSized(*texture, x0, y0, x1 - x0, y1 - y0);
  if(n > 0 && m > 0 && getSizeX() / n > 4 && getSizeY() / m > 4)
  {
    for(size_t x = 0; x <= n; x++) drawer.drawLine(x0 + (getSizeX() * x) / n, y0, x0 + (getSizeX() * x) / n, y1, RGB_Grey);
    for(size_t y = 0; y <= m; y++) drawer.drawLine(x0, y0 + (getSizeY() * y) / m, x1, y0 + (getSizeY() * y) / m, RGB_Grey);
  }
}

void MultiColorPalette::generateTexture() const
{
  if(!(*texture)) return;

  (*texture)->setSize(n, m);
  for(size_t y = 0; y < m; y++)
  for(size_t x = 0; x < n; x++)
  {
    size_t i = y * n + x;
    ColorRGB rgb = RGBdtoRGB(colors[i]);
    setPixel(*texture, x, y, rgb);
  }
  (*texture)->update();
  textureuptodate = true;
}

void MultiColorPalette::setPaletteSize(size_t n, size_t m)
{
  this->n = n;
  this->m = m;
  colors.resize(n * m);
  textureuptodate = false;
}

void MultiColorPalette::setColor(int i, const ColorRGBd& color)
{
  colors[i] = color;
  textureuptodate = false;
}

void MultiColorPalette::handleImpl(const IInput& input)
{
  if(n > 0 && m > 0)
  {
    if(dialog && dialog->isEnabled())
    {
      if(dialog->pressedOk(input))
      {
        dialog->setEnabled(false);
        if(selectedEditing >= 0)
        {
          ColorRGBd temp;
          dialog->getColor(temp);
          setColor(selectedEditing, temp);
        }
        validfg = true;
        setChanged();
      }
    }
    else
    {
      size_t x = ((input.mouseX() - x0) * n) / getSizeX();
      size_t y = ((input.mouseY() - y0) * m) / getSizeY();
      size_t i = y * n + x;

      if(pressed(input, LMB)) { selectedfg = i; validfg = true; setChanged(); }
      if(pressed(input, MMB)) { selectedmg = i; validmg = true; setChanged(); }
      if(pressed(input, RMB)) { selectedbg = i; validbg = true; setChanged(); }
      
      if(dialog && mouseDoubleClicked(input, LMB))
      {
        selectedEditing = i;
        dialog->setColor(colors[i]);
        dialog->setEnabled(true);
      }
    }
  }
}

//these are from the ColorEditor interface, returns the currently selected color
void MultiColorPalette::getColor(ColorRGBd& color) const
{
  if(selectedfg >= 0) color = colors[selectedfg];
  if(selectedmg >= 0) color = colors[selectedmg];
  if(selectedbg >= 0) color = colors[selectedbg];
}

void MultiColorPalette::setColor(const ColorRGBd& color)
{
  (void)color;
  validfg = validmg = validbg = false;
}

bool MultiColorPalette::isMainColorGettable() const { return false; }

bool MultiColorPalette::isMultiColorGettable(Plane plane) const
{
  if(plane == FG) return validfg;
  if(plane == MG) return validmg;
  if(plane == BG) return validbg;
  return false;
}
void MultiColorPalette::getMultiColor(ColorRGBd& color, Plane plane) const
{
  if(plane == FG && selectedfg >= 0) color = colors[selectedfg];
  if(plane == MG && selectedmg >= 0) color = colors[selectedmg];
  if(plane == BG && selectedbg >= 0) color = colors[selectedbg];
}

void MultiColorPalette::setMultiColor(Plane plane, const ColorRGBd& color)
{
  //The palette is not changeable, don't do anything
  (void)color;
  if(plane == FG) validfg = false;
  if(plane == MG) validmg = false;
  if(plane == BG) validbg = false;
  /*if(plane == FG && selectedfg >= 0) colors[selectedfg]->color = color;
  if(plane == MG && selectedmg >= 0) colors[selectedmg]->color = color;
  if(plane == BG && selectedbg >= 0) colors[selectedbg]->color = color;*/
}

////////////////////////////////////////////////////////////////////////////////

ColorDialogSmall::ColorDialogSmall(const IGUIPartGeom& geom)
{
  addTop(geom);
  addTitle("Color");
  //addResizer(geom);
  pushTop(&rgb, Sticky(0.05,0, 0.05,0, 0.95,0, 1.0,-24));
  ok.makeTextPanel(0, 0, "Ok", 64, 24);
  pushTop(&ok, Sticky(0.5, -32, 1.0,-20, 0.5,32, 1.0,-4));
  resize(0, 20, 256, 128);
}

void ColorDialogSmall::handleImpl(const IInput& input)
{
  rgb.handle(input);
  ok.handle(input);
  Window::handleImpl(input);
}

void ColorDialogSmall::drawImpl(IGUIDrawer& drawer) const
{
  Window::drawImpl(drawer);
  rgb.draw(drawer);
  ok.draw(drawer);
}

bool ColorDialogSmall::pressedOk(const IInput& input)
{
  return ok.clicked(input);
}

void ColorDialogSmall::getColor(ColorRGBd& color) const
{
  rgb.getColor(color);
}

void ColorDialogSmall::setColor(const ColorRGBd& color)
{
  rgb.setColor(color);
}

////////////////////////////////////////////////////////////////////////////////

ColorDialog::ColorDialog(const IGUIPartGeom& geom)
{
  addTop(geom);
  addTitle("Color");
  addResizer(geom);
  
  palette.generateVibrant6x6();
  
  ok.makeTextPanel(0, 0, "Ok", 64, 24);
  ok2.makeTextPanel(0, 0, "Ok", 64, 24);
  
  resize(0, 20, 500, 300);
  
  pushTop(&tabs, Sticky(0.0,0, 0.0,0, 1.0,0, 1.0,0));
  tabs.addTab("Basic");
  tabs.addTab("Advanced");
  
  tabs.getTabContent(0).pushTop(&ok, Sticky(1.0, -100, 1.0,-20, 1.0,-16, 1.0,-4));
  tabs.getTabContent(0).pushTop(&html, Sticky(1.0, -100, 1.0,-40, 1.0,-16, 1.0,-24));
  tabs.getTabContent(0).pushTop(&rgb, Sticky(0.05,0, 0.75,0, 0.49,0, 0.99,0));
  tabs.getTabContent(0).pushTop(&plane, Sticky(0.5,4, 0.75,4, 0.75,-4, 1.0,-4));
  tabs.getTabContent(0).pushTop(&hsl, Sticky(0.05,0, 0.05,0, 0.49,0, 0.74,0));
  tabs.getTabContent(0).pushTop(&palette, Sticky(0.5,4, 0.0,4, 1.0,-4, 0.75,-4));
  
  synchronizer.add(&rgb);
  synchronizer.add(&hsl);
  synchronizer.add(&palette);
  synchronizer.add(&plane);
  synchronizer.add(&html);
  
  tabs.getTabContent(1).pushTop(&ok2, Sticky(1.0, -80, 1.0,-20, 1.0,-16, 1.0,-4));
  tabs.getTabContent(1).pushTop(&plane2, Sticky(0.66,4, 0.66,4, 1.0,-84, 1.0,-4));
  tabs.getTabContent(1).pushTop(&rgb2, Sticky(0.0,4, 0.0,4, 0.33,-4, 0.33,-4));
  tabs.getTabContent(1).pushTop(&hsl2, Sticky(0.0,4, 0.33,4, 0.33,-4, 0.66,-4));
  tabs.getTabContent(1).pushTop(&hsv2, Sticky(0.0,4, 0.66,4, 0.33,-4, 1.0,-4));
  tabs.getTabContent(1).pushTop(&cmyk2, Sticky(0.33,4, 0.0,4, 0.66,-4, 0.33,-4));
  tabs.getTabContent(1).pushTop(&ypbpr2, Sticky(0.33,4, 0.33,4, 0.66,-4, 0.66,-4));
  tabs.getTabContent(1).pushTop(&ycbcr2, Sticky(0.33,4, 0.66,4, 0.66,-4, 1.0,-4));
  tabs.getTabContent(1).pushTop(&cielab2, Sticky(0.66,4, 0.0,4, 1.0,-4, 0.33,-4));
  tabs.getTabContent(1).pushTop(&ciexyz2, Sticky(0.66,4, 0.33,4, 1.0,-4, 0.66,-4));

  synchronizer.add(&plane2);
  synchronizer.add(&rgb2);
  synchronizer.add(&hsl2);
  synchronizer.add(&hsv2);
  synchronizer.add(&cmyk2);
  synchronizer.add(&ypbpr2);
  synchronizer.add(&ycbcr2);
  synchronizer.add(&cielab2);
  synchronizer.add(&ciexyz2);

}

void ColorDialog::handleImpl(const IInput& input)
{
  tabs.handle(input);

  synchronizer.handle();
  Window::handleImpl(input);
}

void ColorDialog::drawImpl(IGUIDrawer& drawer) const
{
  Window::drawImpl(drawer);
  
  tabs.draw(drawer);
}

bool ColorDialog::pressedOk(const IInput& input)
{
  return ok.clicked(input);
}

void ColorDialog::getColor(ColorRGBd& color) const
{
  rgb.getColor(color);
}

void ColorDialog::setColor(const ColorRGBd& color)
{
  synchronizer.setColor(color);
}

} //namespace gui
} //namespace lpi
