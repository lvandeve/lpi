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

#pragma once

#include "lpi_gui.h"
#include "lpi_gui_text.h"
#include "lpi_color.h"

#include <vector>
#include <string>


namespace lpi
{
namespace gui
{

void drawCheckerBackground(IGUIDrawer& drawer, int x0, int y0, int x1, int y1, int w = 8, int h = 8, const ColorRGB& c0 = RGB_White, const ColorRGB& c1 = RGB_Black);
ColorRGB getIndicatorColor(const ColorRGB& color); //returns a color that is always well visible on the given background color

class ColorChangeable
{
  protected:
    bool changed; //true if INTERNALLY changed, not if you used "SetValue" or so externally
    void setChanged() { changed = true; }
  public:
    bool hasChanged() { bool temp = changed; changed = false; return temp; }
};

//base class for all color editing classes that edit a full color (not just a few channels of it), to make them work together
class ColorEditor : public ColorChangeable
{
  public:
    virtual ColorRGB getColor() const = 0;
    virtual void setColor(const ColorRGB& color) = 0;
};


class PartialEditor : public ColorChangeable
{
  public:
    enum OutOfRangeAction //what to do with sliders of other color model than RGB that produce values out of RGB range
    {
      DRAW, //just draw the out-of-range RGB color, no matter how weird it may look [#############]
      HIDE, //draw nothing at the places where the RGB color is out of range [   ##########  ]
      WARNING //draw the RGB color, but also draw some indication here to show the user that it's out of the RGB range [XXX#########XX]
    };
    
    
  public:
    PartialEditor();
    virtual ~PartialEditor(){};
    
    virtual void setAdaptiveColor(const ColorRGB& color) = 0;
};

class ChannelSlider : public PartialEditor, public Element
{
  protected:
    Direction dir;
    double value; //value on the slider, always in range 0.0-1.0, even if it's a color model with a channel going from -0.5 to +0.5 or whatever. Do this conversion in getDrawColor and elsewhere instead.
    
    void drawBackgroundH(IGUIDrawer& drawer) const;
    void drawBackgroundV(IGUIDrawer& drawer) const;
    virtual void getDrawColor(ColorRGB& o_color, double value) const = 0;
    
    ColorRGB color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
    bool drawalpha; //if true, draws alpha channel of color, if false, always draws it opaque (this depends on whether or not you want the adaptive color to show alpha channel too)
    OutOfRangeAction outofrangeaction;
  
  public:
  
    ChannelSlider();
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    double getValue() const { return value; }
    void setValue(double value) { this->value = value; }
    
    virtual void setAdaptiveColor(const ColorRGB& color) { this->color = color; }
    virtual void setDrawAlpha(bool drawalpha) { this->drawalpha = drawalpha; }
    virtual void setDrawOutOfRangeRGBColors(OutOfRangeAction action) { this->outofrangeaction = action; }
    
    void setDirection(Direction dir) { this->dir = dir; }
};

enum ColorChannelType //for dynamic sliders
{
  RGB_R,
  RGB_G,
  RGB_B,
  HSV_H,
  HSV_S,
  HSV_V,
  HSL_H,
  HSL_S,
  HSL_L,
  CMY_C,
  CMY_M,
  CMY_Y,
  CMYK_C,
  CMYK_M,
  CMYK_Y,
  CMYK_K,
  CIEXYZ_X,
  CIEXYZ_Y,
  CIEXYZ_Z,
  CIELab_L,
  CIELab_a,
  CIELab_b,
  YPbPr_Y,
  YPbPr_Pb,
  YPbPr_Pr,
  YCbCr_Y,
  YCbCr_Cb,
  YCbCr_Cr,
  A //alpha
};

class ChannelSliderType : public ChannelSlider //can dynamically take any color type
{
  private:
    ColorChannelType type;
    
  public:
  
    ChannelSliderType() : type(RGB_R) {}
    ChannelSliderType(ColorChannelType type) : type(type) {}
    
    void setType(ColorChannelType type) { this->type = type; }
    ColorChannelType getType() const { return type; }
    virtual void getDrawColor(ColorRGB& o_color, double value) const;
    
    void incrementType();
    void decrementType();
    
    virtual void handleImpl(const IInput& input); //used for debugging...
};

class ChannelSliderEx : public PartialEditor, public ElementComposite
{
  /*
  ColorSlicerEx extends ColorSlicer with multiple features:
  -a text input field to view and type the number
  -a label
  -custom ranges instead of the internal 0.0-1.0 representation
  
  If horizontal, label is on left (v-centered) and editbox on right (v-centered)
  If vertical, label is on top (h-centered) and editbox on bottom (h-centered)
  */
  protected:
  
    ChannelSlider* slider;
    InputLine input;
    Text label;
    double smallest; //the slider internally ranges from 0.0 to 1.0,
    double largest; //but the inputline shows numbers in your range (e.g. 0-255)
    
    void fixUpSizes();
    
  public:
    ChannelSliderEx(ChannelSlider* slider, const std::string& label = "", double smallest = 0, double largest = 255); //you must create slider with new, but ChannelSliderEx's dtor will delete it!!
    ~ChannelSliderEx() { delete slider; }
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    double getValue() const; //this is in range 0.0-1.0, like ChannelSlider!
    void setValue(double value);
    
    double getValueScaled() const; //this is in range smallest-largest
    void setValueScaled(double value);

    void setAdaptiveColor(const ColorRGB& color) { slider->setAdaptiveColor(color); }
    void setDrawAlpha(bool drawalpha) { slider->setDrawAlpha(drawalpha); }
    void setDrawOutOfRangeRGBColors(OutOfRangeAction action) { slider->setDrawOutOfRangeRGBColors(action); }
};

class ChannelSliderExType : public ChannelSliderEx
{
  protected:
    ChannelSliderType* the_slider;
  public:
    ChannelSliderExType(ColorChannelType type, const std::string& label = "", double smallest = 0, double largest = 255)
    : ChannelSliderEx(new ChannelSliderType(type), label, smallest, largest)
    {
      the_slider = dynamic_cast<ChannelSliderType*>(slider);
    }
};

class ColorSliders : public ColorEditor, public ElementComposite
{
  /*
  ColorSliders: multiple sliders, together forming one color, e.g. an R,G,B,A bar, or, a C,M,Y,K,A bar
  */
  
  protected:
    std::vector<ChannelSliderEx*> sliders;
    
    void fixUpSizes();
    
  public:
    ColorSliders();
    ~ColorSliders();
    
    void addSlider(ChannelSliderEx* slider); //this class takes ownership of the pointer
    void addAlpha(const std::string& label = "A:", double smallest = 0, double largest = 255); //adds a ColorSliderA, has its own function for convenience, since every color model can have the same alpha channel

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    virtual ColorRGB getColor() const = 0;
    virtual void setColor(const ColorRGB& color) = 0;
};

class ColorSlidersRGB : public ColorSliders
{
  public:
    ColorSlidersRGB(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersHSV : public ColorSliders
{
  public:
    ColorSlidersHSV(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersHSL : public ColorSliders
{
  public:
    ColorSlidersHSL(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersCMY : public ColorSliders
{
  public:
    ColorSlidersCMY(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersCMYK : public ColorSliders
{
  public:
    ColorSlidersCMYK(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersCIEXYZ : public ColorSliders
{
  public:
    ColorSlidersCIEXYZ(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersCIELab : public ColorSliders
{
  public:
    ColorSlidersCIELab(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersYPbPr : public ColorSliders
{
  public:
    ColorSlidersYPbPr(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class ColorSlidersYCbCr : public ColorSliders
{
  public:
    ColorSlidersYCbCr(bool with_alpha = true);
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class PartialEditorSquare : public PartialEditor, public Element
{
  protected:

    double value_x;
    double value_y;
    
    void drawBackground(IGUIDrawer& drawer) const;
    virtual void getDrawColor(ColorRGB& o_color, double value_x, double value_y) const = 0;
    
    ColorRGB color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
    bool drawalpha; //if true, draws alpha channel of color, if false, always draws it opaque (this depends on whether or not you want the adaptive color to show alpha channel too)
    OutOfRangeAction outofrangeaction;
  
  public:
  
    PartialEditorSquare();
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    double getValueX() const { return value_x; }
    void setValueX(double value) { this->value_x = value; }
    double getValueY() const { return value_y; }
    void setValueY(double value) { this->value_y = value; }
    
    virtual void setAdaptiveColor(const ColorRGB& color) { this->color = color; }
    virtual void setDrawAlpha(bool drawalpha) { this->drawalpha = drawalpha; }
    virtual void setDrawOutOfRangeRGBColors(OutOfRangeAction action) { this->outofrangeaction = action; }
};

class PartialEditorSquareType : public PartialEditorSquare
{
  protected:
    ColorChannelType typex;
    ColorChannelType typey;
    
    virtual void getDrawColor(ColorRGB& o_color, double value_x, double value_y) const;
  
  public:
    PartialEditorSquareType() {}
    PartialEditorSquareType(ColorChannelType typex, ColorChannelType typey) : typex(typex), typey(typey) {}
    void setChannels(ColorChannelType typex, ColorChannelType typey) { this->typex = typex; this->typey = typey; }
};

class PartialEditorHueCircle : public PartialEditor, public Element
{
  protected:

    double value_angle; //0.0-1.0
    double value_axial; //0.0-1.0
    
    void drawBackground(IGUIDrawer& drawer) const;
    virtual void getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const = 0;
    
    ColorRGB color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
    bool drawalpha; //if true, draws alpha channel of color, if false, always draws it opaque (this depends on whether or not you want the adaptive color to show alpha channel too)
    OutOfRangeAction outofrangeaction;
  
  public:
  
    PartialEditorHueCircle();
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    double getValueAngle() const { return value_angle; }
    void setValueAngle(double value) { this->value_angle = value; }
    double getValueAxial() const { return value_axial; }
    void setValueAxial(double value) { this->value_axial = value; }
    
    virtual void setAdaptiveColor(const ColorRGB& color) { this->color = color; }
    virtual void setDrawAlpha(bool drawalpha) { this->drawalpha = drawalpha; }
    virtual void setDrawOutOfRangeRGBColors(OutOfRangeAction action) { this->outofrangeaction = action; }
};

class PartialEditorHueCircle_HSV_HS : public PartialEditorHueCircle
{
  virtual void getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const;
};

class PartialEditorHueCircle_HSV_HV : public PartialEditorHueCircle
{
  virtual void getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const;
};

class PartialEditorHueCircle_HSL_HS : public PartialEditorHueCircle
{
  virtual void getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const;
};

class PartialEditorHueCircle_HSL_HL : public PartialEditorHueCircle
{
  virtual void getDrawColor(ColorRGB& o_color, double value_angle, double value_axial) const;
};

//HueCircleEditor: a full color editor, with hue circle, a vertical bar to the right, and a horizontal alpha channel bar below.
class HueCircleEditor : public ColorEditor, public ElementComposite
{
  protected:
    PartialEditorHueCircle* circle;
    ChannelSlider* sliderc; //the slider for third component of the color
    ChannelSlider* slidera; //the slider for alpha channel
};

class HueCircleEditor_HSV_HS : public HueCircleEditor
{
  private:
  
  public:
    
    HueCircleEditor_HSV_HS()
    {
      circle = new PartialEditorHueCircle_HSV_HS();
      addSubElement(circle, Sticky(0.0, 0, 0.0, 0, 0.8, 0, 0.8, 0));
      sliderc = new ChannelSliderType(HSV_V);
      sliderc->setDirection(V);
      addSubElement(sliderc, Sticky(0.85, 0, 0.0, 0, 0.95, 0, 0.8, 0));
      slidera = new ChannelSliderType(A);
      addSubElement(slidera, Sticky(0.0, 0, 0.85, 0, 0.8, 0, 0.95, 0));
    }
    
    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      circle->draw(drawer);
      sliderc->draw(drawer);
      slidera->draw(drawer);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      circle->handle(input);
      sliderc->handle(input);
      slidera->handle(input);
      if(circle->hasChanged()) setChanged();
      if(sliderc->hasChanged()) setChanged();
      if(slidera->hasChanged()) setChanged();
      circle->setAdaptiveColor(getColor());
      sliderc->setAdaptiveColor(getColor());
      slidera->setAdaptiveColor(getColor());
    }
    
    //these are from the ColorEditor interface, returns the currently selected color
    virtual ColorRGB getColor() const
    {
      ColorHSV hsv;
      hsv.h = (int)(circle->getValueAngle() * 255);
      hsv.s = (int)(circle->getValueAxial() * 255);
      hsv.v = (int)(sliderc->getValue() * 255);
      hsv.a = (int)(slidera->getValue() * 255);
      return HSVtoRGB(hsv);
    }
    
    virtual void setColor(const ColorRGB& color)
    {
      ColorHSV hsv = RGBtoHSV(color);
      circle->setValueAngle(hsv.h / 255.0);
      circle->setValueAxial(hsv.s / 255.0);
      sliderc->setValue(hsv.v / 255.0);
      slidera->setValue(hsv.a / 255.0);
    }
};

class PColorPlane : public Element
{
  private:
    ColorRGB* color;

  public:
    PColorPlane(ColorRGB* color);
    virtual void drawImpl(IGUIDrawer& drawer) const;
};

class ColorPlane : public PColorPlane
{
  public:
    ColorRGB color;
    ColorPlane();
};


class FGBGColor : public ColorEditor, public ElementComposite
{
  /*
  this class represents the following kind of thing (seen in many painting programs):
  
  There is a foreground and background color, and arrows to swap them.
   ______
  [      ] <-+
  |      |   v
  |      |____
  [______]    ]
       |      |
       |      |
       [______]
       
  */
  
  //TODO: if color is out of RGB range, draw some exclamation mark, warning sign or cross over the color in the square
  
  private:
   
    class Arrows : public Element
    {
      public:
        virtual void drawImpl(IGUIDrawer& drawer) const;
    };
    
    class ColorPlane : public Element
    {
      public:
        ColorRGB color;
        bool selected;
        
        ColorPlane();
        virtual void drawImpl(IGUIDrawer& drawer) const;
        virtual void handleImpl(const IInput& input);
    };
    
  private:
    ColorPlane fg;
    ColorPlane bg;
    Arrows arrows;
  
  public:
  
    FGBGColor();
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    void setFG(const ColorRGB& color);
    void setBG(const ColorRGB& color);
    
    ColorRGB getFG() const;
    ColorRGB getBG() const;
    
    bool selectedFG() const;
    bool selectedBG() const;
    
    //these are from the ColorEditor interface, returns the currently selected color
    virtual ColorRGB getColor() const;
    virtual void setColor(const ColorRGB& color);
};

class FGMGBGColor
{
  /*
  TODO
  because there can't be enough mouse buttons, it also supports a third color (MG or "middleground"), which
  can be used for the middle mouse button. Then it looks somewhat like this:
   ______
  [      ]_<-+
  |      | ]_v
  |      | | ]
  [______] | |
    [______] |
      [______]
  */
};

class ColorEditorSynchronizer
{
  private:
    std::vector<ColorEditor*> editors;
  
  public:
  
    void add(ColorEditor* editor);
    void remove(ColorEditor* editor);
    void clear();
    
    void handle();
    
    void setColor(const ColorRGB& color); //set the color of everything
};

} //namespace gui
} //namespace lpi
