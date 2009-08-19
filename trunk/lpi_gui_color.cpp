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

#include "lpi_parse.h"

#include <cstdlib> //abs(int)
#include <iostream>

namespace lpi
{
namespace gui
{

void drawCheckerBackground(IGUIDrawer& drawer, int x0, int y0, int x1, int y1, int w, int h, const ColorRGB& c0, const ColorRGB& c1)
{
  bool bx = true, by = true;
  
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
  }
}

ColorChannelEditor::ColorChannelEditor()
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

ColorSlider::ColorSlider()
: dir(H)
, value(0.5)
, drawalpha(true)
, outofrangeaction(WARNING)
{
}


void ColorSlider::drawWidget(IGUIDrawer& drawer) const
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
    int pos = y0 + (int)(value * getSizeY());
    if(pos <= y0) pos = y0 + 1; if(pos >= y1 - 1) pos = y1 - 2; //don't make it hidden by border...
    drawer.drawLine(x0, pos, x1, pos, indicator);
  }

  drawer.drawRectangle(x0, y0, x1, y1, RGB_Black, false);
}

void ColorSlider::drawBackgroundH(IGUIDrawer& drawer) const
{
  static const size_t NUM = 128;
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

void ColorSlider::drawBackgroundV(IGUIDrawer& drawer) const
{
  static const size_t NUM = 128;
  ColorRGB c;
  
  if(drawalpha) drawCheckerBackground(drawer, x0, y0, x1, y1, 4, 4, RGB_White, RGB_Black);
  
  for(size_t i = 0; i < NUM; i++)
  {
    int ya = 1 + y0 + ((i * getSizeY()) / NUM);
    int yb = 1 + y0 + (((i + 1) * getSizeY()) / NUM);
    if(i == NUM - 1) yb = y1 - 1;
    double v = (double)((ya + yb) / 2 - y0) / (getSizeY() - 2);
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

void ColorSlider::handleWidget(const IGUIInput& input)
{
  if(mouseGrabbed(input))
  {
    if(dir == H)
      value = mouseGetRelPosX(input) / (double)(getSizeX());
    else
      value = mouseGetRelPosY(input) / (double)(getSizeY());
    
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
    case RGB_R:
    case RGB_G:
    case RGB_B:
      {
        o_color = color;
        if(type == RGB_R) o_color.r = (int)(value * 255.0);
        else if(type == RGB_G) o_color.g = (int)(value * 255.0);
        else if(type == RGB_B) o_color.b = (int)(value * 255.0);
      }
      break;
    case HSV_H:
    case HSV_S:
    case HSV_V:
      {
        ColorHSV convert = RGBtoHSV(color);
        if(type == HSV_H) convert.h = (int)(value * 255.0);
        else if(type == HSV_S) convert.s = (int)(value * 255.0);
        else if(type == HSV_V) convert.v = (int)(value * 255.0);
        o_color = HSVtoRGB(convert);
      }
      break;
    case HSL_H:
    case HSL_S:
    case HSL_L:
      {
        ColorHSL convert = RGBtoHSL(color);
        if(type == HSL_H) convert.h = (int)(value * 255.0);
        else if(type == HSL_S) convert.s = (int)(value * 255.0);
        else if(type == HSL_L) convert.l = (int)(value * 255.0);
        o_color = HSLtoRGB(convert);
      }
      break;
    case CMY_C:
    case CMY_M:
    case CMY_Y:
      {
        ColorCMY convert = RGBtoCMY(color);
        if(type == CMY_C) convert.c = (int)(value * 255.0);
        else if(type == CMY_M) convert.m = (int)(value * 255.0);
        else if(type == CMY_Y) convert.y = (int)(value * 255.0);
        o_color = CMYtoRGB(convert);
      }
      break;
    case CMYK_C:
    case CMYK_M:
    case CMYK_Y:
    case CMYK_K:
      {
        ColorCMYK convert = RGBtoCMYK(color);
        if(type == CMYK_C) convert.c = (int)(value * 255.0);
        else if(type == CMYK_M) convert.m = (int)(value * 255.0);
        else if(type == CMYK_Y) convert.y = (int)(value * 255.0);
        else if(type == CMYK_K) convert.k = (int)(value * 255.0);
        o_color = CMYKtoRGB(convert);
      }
      break;
    case CIEXYZ_X:
    case CIEXYZ_Y:
    case CIEXYZ_Z:
      {
        ColorCIEXYZ convert = RGBtoCIEXYZ(color);
        if(type == CIEXYZ_X) convert.x = (int)(value * 255.0);
        else if(type == CIEXYZ_Y) convert.y = (int)(value * 255.0);
        else if(type == CIEXYZ_Z) convert.z = (int)(value * 255.0);
        o_color = CIEXYZtoRGB(convert);
      }
      break;
    case CIELab_L:
    case CIELab_a:
    case CIELab_b:
      {
        ColorCIELab convert = RGBtoCIELab(color);
        if(type == CIELab_L) convert.l = (int)(value * 255.0);
        else if(type == CIELab_a) convert.a = (int)((2.0 * value - 1.0) * 255.0);
        else if(type == CIELab_b) convert.b = (int)((2.0 * value - 1.0) * 255.0);
        o_color = CIELabtoRGB(convert);
      }
      break;
    case YPbPr_Y:
    case YPbPr_Pb:
    case YPbPr_Pr:
      {
        ColorYPbPr convert = RGBtoYPbPr(color);
        if(type == YPbPr_Y) convert.y = (int)(value * 255.0);
        else if(type == YPbPr_Pb) convert.pb = (int)((value - 0.5) * 255.0);
        else if(type == YPbPr_Pr) convert.pr = (int)((value - 0.5) * 255.0);
        o_color = YPbPrtoRGB(convert);
      }
      break;
    case YCbCr_Y:
    case YCbCr_Cb:
    case YCbCr_Cr:
      {
        ColorYCbCr convert = RGBtoYCbCr(color);
        if(type == YCbCr_Y) convert.y = (int)(value * 255.0);
        else if(type == YCbCr_Cb) convert.cb = (int)(value * 255.0);
        else if(type == YCbCr_Cr) convert.cr = (int)(value * 255.0);
        o_color = YCbCrtoRGB(convert);
      }
      break;
    case A:
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

void ColorSliderType::getDrawColor(ColorRGB& o_color, double value) const
{
  getColorDynamic(o_color, color, value, type);
}

void ColorSliderType::incrementType()
{
  if(type == A)
    type = RGB_R;
  else
    type = (ColorChannelType)((int)(type) + 1);
}

void ColorSliderType::decrementType()
{
  if(type == RGB_R)
    type = A;
  else
    type = (ColorChannelType)((int)(type) - 1);
}

void ColorSliderType::handleWidget(const IGUIInput& input)
{
  ColorSlider::handleWidget(input);
  if(mouseDoubleClicked(input, GUI_LMB)) incrementType();
  if(mouseDoubleClicked(input, GUI_RMB)) decrementType();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ColorSliderEx::ColorSliderEx(ColorSlider* slider, const std::string& label, double smallest, double largest)
: slider(slider)
, smallest(smallest)
, largest(largest)
{
  static const int EDITBOXSIZEX = 52, EDITBOXSIZEY = 12;
  this->label.make(0, 3, label, TS_ShadowInv);
  int LABELSIZEX = this->label.getSizeX();
  resize(0, 0, LABELSIZEX + EDITBOXSIZEX, EDITBOXSIZEY);
  input.make(0, 0, 5, TS_B);
  input.resize(LABELSIZEX + 6, 2, LABELSIZEX + EDITBOXSIZEX - 1, EDITBOXSIZEY - 1);
  input.cursorColor = RGB_Black;
  slider->resize(LABELSIZEX, 0, LABELSIZEX, EDITBOXSIZEY);
  addSubElement(&this->label, Sticky(0.0, 0, 0.5, -this->label.getSizeY() / 2, 0.0, this->label.getSizeX(), 0.5, this->label.getSizeY() / 2));
  addSubElement(&input, Sticky(1.0, -input.getSizeX(), 0.5, -input.getSizeY() / 2, 1.0, 0, 0.5, input.getSizeY() / 2));
  addSubElement(slider);
  ic.setStickyFull(slider, this);
}

void ColorSliderEx::fixUpSizes()
{
}

void ColorSliderEx::drawWidget(IGUIDrawer& drawer) const
{
  slider->draw(drawer);
  drawer.drawRectangle(input.getX0()-1, input.getY0()-1, input.getX1(), input.getY1(), RGB_White, true);
  drawer.drawRectangle(input.getX0()-2, input.getY0()-2, input.getX1()+1, input.getY1()+1, RGB_Black, false);
  input.draw(drawer);
  label.draw(drawer);
}

void ColorSliderEx::handleWidget(const IGUIInput& input)
{
  slider->handle(input);
  this->input.handle(input);
  
  if(slider->hasChanged())
  {
    this->input.setText(valtostr((int)(smallest + slider->getValue() * (largest - smallest))));
    setChanged();
  }
  else if(this->input.enteringDone())
  {
    int d = strtoval<int>(this->input.getText());
    slider->setValue((d  - smallest) / (largest - smallest));
    setChanged();
  }
}

double ColorSliderEx::getValue() const
{
  return slider->getValue();
}

void ColorSliderEx::setValue(double value)
{
  slider->setValue(value);
  input.setText(valtostr((int)(smallest + value * (largest - smallest))));
}

double ColorSliderEx::getValueScaled() const
{
  return smallest + getValue() * (largest - smallest);
}

void ColorSliderEx::setValueScaled(double value)
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
{
  resize(0, 0, 256, 64); //sort of sloppy solution: set x-size large enough initially, because otherwise the color slider's relative sizes become corrupted
}

ColorSliders::~ColorSliders()
{
  for(size_t i = 0; i < sliders.size(); i++)
    delete sliders[i];
}

void ColorSliders::addSlider(ColorSliderEx* slider)
{
  sliders.push_back(slider);
  addSubElement(slider);
  fixUpSizes();
}

void ColorSliders::addAlpha(const std::string& label, double smallest, double largest)
{
  addSlider(new ColorSliderExType(A, label, smallest, largest));
}

void ColorSliders::drawWidget(IGUIDrawer& drawer) const
{
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->draw(drawer);
}

void ColorSliders::handleWidget(const IGUIInput& input)
{
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->handle(input);
    
  //for "adaptive color" in the sliders
  ColorRGB color = getColor();
  for(size_t i = 0; i < sliders.size(); i++)
    sliders[i]->setAdaptiveColor(color);
  
  for(size_t i = 0; i < sliders.size(); i++)
  {
    if(sliders[i]->hasChanged())
      setChanged();
  }
}

ColorSlidersRGB::ColorSlidersRGB(bool with_alpha)
{
  addSlider(new ColorSliderExType(RGB_R, "R:"));
  addSlider(new ColorSliderExType(RGB_G, "G:"));
  addSlider(new ColorSliderExType(RGB_B, "B:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersRGB::getColor() const
{
  ColorRGB result;
  result.r = (int)(sliders[0]->getValue() * 255.0);
  result.g = (int)(sliders[1]->getValue() * 255.0);
  result.b = (int)(sliders[2]->getValue() * 255.0);
  result.a = (int)(sliders[3]->getValue() * 255.0);
  return result;
}

void ColorSlidersRGB::setColor(const ColorRGB& color)
{
  sliders[0]->setValue(color.r / 255.0);
  sliders[1]->setValue(color.g / 255.0);
  sliders[2]->setValue(color.b / 255.0);
  sliders[3]->setValue(color.a / 255.0);
}

ColorSlidersHSV::ColorSlidersHSV(bool with_alpha)
{
  addSlider(new ColorSliderExType(HSV_H, "H:"));
  addSlider(new ColorSliderExType(HSV_S, "S:"));
  addSlider(new ColorSliderExType(HSV_V, "V:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersHSV::getColor() const
{
  lpi::ColorHSV convert;
  convert.h = (int)(255.0 * sliders[0]->getValue());
  convert.s = (int)(255.0 * sliders[1]->getValue());
  convert.v = (int)(255.0 * sliders[2]->getValue());
  convert.a = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::HSVtoRGB(convert);
  return result;
}

void ColorSlidersHSV::setColor(const ColorRGB& color)
{
  lpi::ColorHSV convert = lpi::RGBtoHSV(color);
  sliders[0]->setValue(convert.h / 255.0);
  sliders[1]->setValue(convert.s / 255.0);
  sliders[2]->setValue(convert.v / 255.0);
  sliders[3]->setValue(convert.a / 255.0);
}

ColorSlidersHSL::ColorSlidersHSL(bool with_alpha)
{
  addSlider(new ColorSliderExType(HSL_H, "H:"));
  addSlider(new ColorSliderExType(HSL_S, "S:"));
  addSlider(new ColorSliderExType(HSL_L, "L:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersHSL::getColor() const
{
  lpi::ColorHSL convert;
  convert.h = (int)(255.0 * sliders[0]->getValue());
  convert.s = (int)(255.0 * sliders[1]->getValue());
  convert.l = (int)(255.0 * sliders[2]->getValue());
  convert.a = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::HSLtoRGB(convert);
  return result;
}

void ColorSlidersHSL::setColor(const ColorRGB& color)
{
  lpi::ColorHSL convert = lpi::RGBtoHSL(color);
  sliders[0]->setValue(convert.h / 255.0);
  sliders[1]->setValue(convert.s / 255.0);
  sliders[2]->setValue(convert.l / 255.0);
  sliders[3]->setValue(convert.a / 255.0);
}

ColorSlidersCMY::ColorSlidersCMY(bool with_alpha)
{
  addSlider(new ColorSliderExType(CMY_C, "C:"));
  addSlider(new ColorSliderExType(CMY_M, "M:"));
  addSlider(new ColorSliderExType(CMY_Y, "Y:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersCMY::getColor() const
{
  lpi::ColorCMY convert;
  convert.c = (int)(255.0 * sliders[0]->getValue());
  convert.m = (int)(255.0 * sliders[1]->getValue());
  convert.y = (int)(255.0 * sliders[2]->getValue());
  convert.a = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::CMYtoRGB(convert);
  return result;
}

void ColorSlidersCMY::setColor(const ColorRGB& color)
{
  lpi::ColorCMY convert = lpi::RGBtoCMY(color);
  sliders[0]->setValue(convert.c / 255.0);
  sliders[1]->setValue(convert.m / 255.0);
  sliders[2]->setValue(convert.y / 255.0);
  sliders[3]->setValue(convert.a / 255.0);
}

ColorSlidersCMYK::ColorSlidersCMYK(bool with_alpha)
{
  addSlider(new ColorSliderExType(CMYK_C, "C:"));
  addSlider(new ColorSliderExType(CMYK_M, "M:"));
  addSlider(new ColorSliderExType(CMYK_Y, "Y:"));
  addSlider(new ColorSliderExType(CMYK_K, "K:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersCMYK::getColor() const
{
  lpi::ColorCMYK convert;
  convert.c = (int)(255.0 * sliders[0]->getValue());
  convert.m = (int)(255.0 * sliders[1]->getValue());
  convert.y = (int)(255.0 * sliders[2]->getValue());
  convert.k = (int)(255.0 * sliders[3]->getValue());
  convert.a = (int)(255.0 * sliders[4]->getValue());
  lpi::ColorRGB result = lpi::CMYKtoRGB(convert);
  return result;
}

void ColorSlidersCMYK::setColor(const ColorRGB& color)
{
  lpi::ColorCMYK convert = lpi::RGBtoCMYK(color);
  sliders[0]->setValue(convert.c / 255.0);
  sliders[1]->setValue(convert.m / 255.0);
  sliders[2]->setValue(convert.y / 255.0);
  sliders[3]->setValue(convert.k / 255.0);
  sliders[4]->setValue(convert.a / 255.0);
}

ColorSlidersCIEXYZ::ColorSlidersCIEXYZ(bool with_alpha)
{
  addSlider(new ColorSliderExType(CIEXYZ_X, "X:"));
  addSlider(new ColorSliderExType(CIEXYZ_Y, "Y:"));
  addSlider(new ColorSliderExType(CIEXYZ_Z, "Z:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersCIEXYZ::getColor() const
{
  lpi::ColorCIEXYZ convert;
  convert.x = (int)(255.0 * sliders[0]->getValue());
  convert.y = (int)(255.0 * sliders[1]->getValue());
  convert.z = (int)(255.0 * sliders[2]->getValue());
  convert.alpha = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::CIEXYZtoRGB(convert);
  return result;
}

void ColorSlidersCIEXYZ::setColor(const ColorRGB& color)
{
  lpi::ColorCIEXYZ convert = lpi::RGBtoCIEXYZ(color);
  sliders[0]->setValue(convert.x / 255.0);
  sliders[1]->setValue(convert.y / 255.0);
  sliders[2]->setValue(convert.z / 255.0);
  sliders[3]->setValue(convert.alpha / 255.0);
}

ColorSlidersCIELab::ColorSlidersCIELab(bool with_alpha)
{
  addSlider(new ColorSliderExType(CIELab_L, "L:", 0, 255));
  addSlider(new ColorSliderExType(CIELab_a, "a:", -128, 127));
  addSlider(new ColorSliderExType(CIELab_b, "b:", -128, 127));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha("A:");
}

ColorRGB ColorSlidersCIELab::getColor() const
{
  lpi::ColorCIELab convert;
  convert.l = (int)(255.0 * sliders[0]->getValue());
  convert.a = (int)(255.0 * (sliders[1]->getValue() * 2.0 - 1.0));
  convert.b = (int)(255.0 * (sliders[2]->getValue() * 2.0 - 1.0));
  convert.alpha = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::CIELabtoRGB(convert);
  return result;
}

void ColorSlidersCIELab::setColor(const ColorRGB& color)
{
  lpi::ColorCIELab convert = lpi::RGBtoCIELab(color);
  sliders[0]->setValue(convert.l / 255.0);
  sliders[1]->setValue((convert.a / 255.0) / 2.0 + 0.5);
  sliders[2]->setValue((convert.b / 255.0) / 2.0 + 0.5);
  sliders[3]->setValue(convert.alpha / 255.0);
}

////

ColorSlidersYPbPr::ColorSlidersYPbPr(bool with_alpha)
{
  addSlider(new ColorSliderExType(YPbPr_Y, " Y:", 0, 255));
  addSlider(new ColorSliderExType(YPbPr_Pb, "Pb:", -128, 127));
  addSlider(new ColorSliderExType(YPbPr_Pr, "Pr:", -128, 127));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha(" A:");
}

ColorRGB ColorSlidersYPbPr::getColor() const
{
  lpi::ColorYPbPr convert;
  convert.y = (int)(255.0 * sliders[0]->getValue());
  convert.pb = (int)(255.0 * (sliders[1]->getValue()-0.5));
  convert.pr = (int)(255.0 * (sliders[2]->getValue()-0.5));
  convert.alpha = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::YPbPrtoRGB(convert);
  return result;
}

void ColorSlidersYPbPr::setColor(const ColorRGB& color)
{
  lpi::ColorYPbPr convert = lpi::RGBtoYPbPr(color);
  sliders[0]->setValue(convert.y / 255.0);
  sliders[1]->setValue(convert.pb / 255.0 + 0.5);
  sliders[2]->setValue(convert.pr / 255.0 + 0.5);
  sliders[3]->setValue(convert.alpha / 255.0);
}

ColorSlidersYCbCr::ColorSlidersYCbCr(bool with_alpha)
{
  addSlider(new ColorSliderExType(YCbCr_Y, " Y:"));
  addSlider(new ColorSliderExType(YCbCr_Cb, "Cb:"));
  addSlider(new ColorSliderExType(YCbCr_Cr, "Cr:"));
  for(size_t i = 0; i < sliders.size(); i++) sliders[i]->setDrawAlpha(false);
  if(with_alpha) addAlpha(" A:");
}

ColorRGB ColorSlidersYCbCr::getColor() const
{
  lpi::ColorYCbCr convert;
  convert.y = (int)(255.0 * sliders[0]->getValue());
  convert.cb = (int)(255.0 * sliders[1]->getValue());
  convert.cr = (int)(255.0 * sliders[2]->getValue());
  convert.alpha = (int)(255.0 * sliders[3]->getValue());
  lpi::ColorRGB result = lpi::YCbCrtoRGB(convert);
  return result;
}

void ColorSlidersYCbCr::setColor(const ColorRGB& color)
{
  lpi::ColorYCbCr convert = lpi::RGBtoYCbCr(color);
  sliders[0]->setValue(convert.y / 255.0);
  sliders[1]->setValue(convert.cb / 255.0);
  sliders[2]->setValue(convert.cr / 255.0);
  sliders[3]->setValue(convert.alpha / 255.0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  
ColorEditor2D::ColorEditor2D()
{
}

void ColorEditor2D::drawBackground(IGUIDrawer& drawer) const
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
    
    ColorRGB color = this->color;
    getDrawColor(color, (double)x / (double)NUMX, (double)y / (double)NUMY);
    
    drawer.drawRectangle(drawx0, drawy0, drawx1, drawy1, color, true);
  }
}

void ColorEditor2D::drawWidget(IGUIDrawer& drawer) const
{
  drawBackground(drawer);
}

void ColorEditor2D::handleWidget(const IGUIInput& input)
{
  (void)input;
}

void ColorEditor2DType::getDrawColor(ColorRGB& o_color, double value_x, double value_y) const
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

  
HueCircle::HueCircle()
{
}

void HueCircle::drawWidget(IGUIDrawer& drawer) const
{
  static const size_t NUMANGLE = 24;
  static const size_t NUMAXIAL = 8;
  
  int cx = getCenterX();
  int cy = getCenterY();
  double radius = getSizeX() < getSizeY() ? getSizeX() / 2 : getSizeY() / 2;
  
  for(size_t a = 0; a < NUMANGLE; a++)
  {
    const double twopi = 2.0 * 3.14159;
    double angle = ((double)a / (double)NUMANGLE) * twopi;
    double angle2 = ((double)(a == NUMANGLE ? 0 : a + 1) / (double)NUMANGLE) * twopi;
    for(size_t r = 0; r < NUMAXIAL; r++)
    {
      double axial = ((double)r / (double)NUMAXIAL) * radius;
      double axial2 = ((double)(r + 1) / (double)NUMAXIAL) * radius;
      int x0 = cx + (int)(std::cos(angle) * axial);
      int y0 = cy + (int)(std::sin(angle) * axial);
      int x1 = cx + (int)(std::cos(angle) * axial2);
      int y1 = cy + (int)(std::sin(angle) * axial2);
      int x2 = cx + (int)(std::cos(angle2) * axial2);
      int y2 = cy + (int)(std::sin(angle2) * axial2);
      int x3 = cx + (int)(std::cos(angle2) * axial);
      int y3 = cy + (int)(std::sin(angle2) * axial);
      
      ColorRGB color0 = this->color, color1 = this->color, color2 = this->color, color3 = this->color;
      getDrawColor(color0, angle / twopi, axial / radius);
      getDrawColor(color1, angle / twopi, axial2 / radius);
      getDrawColor(color2, angle2 / twopi, axial2 / radius);
      getDrawColor(color3, angle2 / twopi, axial / radius);
      
      drawer.drawGradientQuad(x0, y0, x1, y1, x2, y2, x3, y3, color0, color1, color2, color3);
    }
  }
}

void HueCircle::handleWidget(const IGUIInput& input)
{
  (void)input;
}

void HueCircle_HSV_HS::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSV convert = RGBtoHSV(color);
  convert.h = (int)(value_angle * 255.0);
  convert.s = (int)(value_axial * 255.0);
  o_color = HSVtoRGB(convert);
}

void HueCircle_HSV_HV::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSV convert = RGBtoHSV(color);
  convert.h = (int)(value_angle * 255.0);
  convert.v = (int)(value_axial * 255.0);
  o_color = HSVtoRGB(convert);
}

void HueCircle_HSL_HS::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSL convert = RGBtoHSL(color);
  convert.h = (int)(value_angle * 255.0);
  convert.s = (int)(value_axial * 255.0);
  o_color = HSLtoRGB(convert);
}

void HueCircle_HSL_HL::getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const
{
  ColorHSL convert = RGBtoHSL(color);
  convert.h = (int)(value_angle * 255.0);
  convert.l = (int)(value_axial * 255.0);
  o_color = HSLtoRGB(convert);
}

////////////////////////////////////////////////////////////////////////////////

ColorPlane::ColorPlane()
: color(RGB_White)
{
}

void ColorPlane::drawWidget(IGUIDrawer& drawer) const
{
  drawCheckerBackground(drawer, x0, y0, x1, y1);
  drawer.drawRectangle(x0, y0, x1, y1, color, true);
  drawer.drawGUIPart(GP_PANEL_BORDER, x0, y0, x1, y1, false, RGB_White);
}

} //namespace gui
} //namespace lpi

