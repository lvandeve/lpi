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
    //main color: a single color, not associated to any mouse button (not even the first or left one)
    virtual bool isMainColorGettable() const { return true; }
    virtual ColorRGBd getColor() const = 0;
    virtual ColorRGB getColor255() { return RGBdtoRGB(getColor()); }
    virtual void setColor(const ColorRGBd& color) = 0;
    
    //multi color: a color per mouse button
    
    enum Plane //don't know best name for this enum, could also be "Depth" or "Dimension"...
    {
      FG, //foreground
      MG, //middleground
      BG  //background
    };
    
    virtual bool isMultiColorGettable(Plane plane) const { (void)plane; return false; }
    virtual ColorRGBd getMultiColor(Plane plane) const { return getColor(); }
    virtual void setMultiColor(Plane plane, const ColorRGBd& color) { (void)plane; (void)color; }
    
    //multi color linker: choose which of the multi-colors is linked to the main color
    virtual bool isMultiColorLinker() const { return false; }
    virtual Plane getMultiColorLink() const { return FG; }
    virtual void setMultiColorLink(Plane plane) { (void)plane; }
    
    /*
    The above functionality is all to cooperate with the ColorEditorSynchronizer
    The intention is that colors are synchronized between multiple controls that all edit the same color.
    What makes it more complex is that there are controls that can select multiple colors (FG, MG and BG) while
    others only edit one main color, and some controls (e.g. the FGBG) can make you choose which plane (FG,
    MG or BG) is currently being edited and link the controls with a main color to that plane.
    The controls that edit multiple colors often have one mouse button associated to each plane: left for FG,
    right for BG and middle for MG.
    */
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
    
    virtual void setAdaptiveColor(const ColorRGBd& color) = 0;
};

class ChannelSlider : public PartialEditor, public Element
{
  protected:
    Direction dir;
    double value; //value on the slider, always in range 0.0-1.0, even if it's a color model with a channel going from -0.5 to +0.5 or whatever. Do this conversion in getDrawColor and elsewhere instead.
    
    void drawBackgroundH(IGUIDrawer& drawer) const;
    void drawBackgroundV(IGUIDrawer& drawer) const;
    virtual void getDrawColor(ColorRGB& o_color, double value) const = 0;
    
    ColorRGBd color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
    bool drawalpha; //if true, draws alpha channel of color, if false, always draws it opaque (this depends on whether or not you want the adaptive color to show alpha channel too)
    OutOfRangeAction outofrangeaction;
  
  public:
  
    ChannelSlider();
  
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    double getValue() const { return value; }
    void setValue(double value) { this->value = value; }
    
    virtual void setAdaptiveColor(const ColorRGBd& color) { this->color = color; }
    virtual void setDrawAlpha(bool drawalpha) { this->drawalpha = drawalpha; }
    virtual void setDrawOutOfRangeRGBColors(OutOfRangeAction action) { this->outofrangeaction = action; }
    
    void setDirection(Direction dir) { this->dir = dir; }
};

enum ColorChannelType //for dynamic sliders
{
  CC_RGB_R,
  CC_RGB_G,
  CC_RGB_B,
  CC_HSV_H,
  CC_HSV_S,
  CC_HSV_V,
  CC_HSL_H,
  CC_HSL_S,
  CC_HSL_L,
  CC_CMY_C,
  CC_CMY_M,
  CC_CMY_Y,
  CC_CMYK_C,
  CC_CMYK_M,
  CC_CMYK_Y,
  CC_CMYK_K,
  CC_CIEXYZ_X,
  CC_CIEXYZ_Y,
  CC_CIEXYZ_Z,
  CC_CIELab_L,
  CC_CIELab_a,
  CC_CIELab_b,
  CC_YPbPr_Y,
  CC_YPbPr_Pb,
  CC_YPbPr_Pr,
  CC_YCbCr_Y,
  CC_YCbCr_Cb,
  CC_YCbCr_Cr,
  CC_A //alpha
};

class ChannelSliderType : public ChannelSlider //can dynamically take any color type
{
  private:
    ColorChannelType type;
    
  public:
  
    ChannelSliderType() : type(CC_RGB_R) {}
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

    void setAdaptiveColor(const ColorRGBd& color) { slider->setAdaptiveColor(color); }
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
    
    virtual ColorRGBd getColor() const = 0;
    virtual void setColor(const ColorRGBd& color) = 0;
};

class ColorSlidersRGB : public ColorSliders
{
  public:
    ColorSlidersRGB(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersHSV : public ColorSliders
{
  public:
    ColorSlidersHSV(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersHSL : public ColorSliders
{
  public:
    ColorSlidersHSL(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersCMY : public ColorSliders
{
  public:
    ColorSlidersCMY(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersCMYK : public ColorSliders
{
  public:
    ColorSlidersCMYK(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersCIEXYZ : public ColorSliders
{
  public:
    ColorSlidersCIEXYZ(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersCIELab : public ColorSliders
{
  public:
    ColorSlidersCIELab(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersYPbPr : public ColorSliders
{
  public:
    ColorSlidersYPbPr(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class ColorSlidersYCbCr : public ColorSliders
{
  public:
    ColorSlidersYCbCr(bool with_alpha = true);
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

class PartialEditorSquare : public PartialEditor, public Element
{
  protected:

    double value_x;
    double value_y;
    
    void drawBackground(IGUIDrawer& drawer) const;
    virtual void getDrawColor(ColorRGB& o_color, double value_x, double value_y) const = 0;
    
    ColorRGBd color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
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
    
    virtual void setAdaptiveColor(const ColorRGBd& color) { this->color = color; }
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
    
    ColorRGBd color; //this color is used as base color, e.g. when it's a lightness slider, the darkest is black, the brightest is this color. Can be used to show the effect of this slider on the current color.
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
    
    virtual void setAdaptiveColor(const ColorRGBd& color) { this->color = color; }
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
    
  public:
    HueCircleEditor();
    virtual ~HueCircleEditor();
    
    void init()
    {
      addSubElement(circle, Sticky(0.0, 0, 0.0, 0, 0.9, 0, 0.9, 0));
      sliderc->setDirection(V);
      addSubElement(sliderc, Sticky(0.9, 0, 0.0, 0, 1.0, 0, 0.9, 0));
      addSubElement(slidera, Sticky(0.0, 0, 0.9, 0, 0.9, 0, 1.0, 0));
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
};

class HueCircleEditor_HSV_HS : public HueCircleEditor
{
  public:
    HueCircleEditor_HSV_HS()
    {
      circle = new PartialEditorHueCircle_HSV_HS();
      sliderc = new ChannelSliderType(CC_HSV_V);
      slidera = new ChannelSliderType(CC_A);
      init();
    }
    
    virtual ColorRGBd getColor() const
    {
      return HSVtoRGB(ColorHSVd(circle->getValueAngle(), circle->getValueAxial(), sliderc->getValue(), slidera->getValue()));
    }
    
    virtual void setColor(const ColorRGBd& color)
    {
      ColorHSVd hsv = RGBtoHSV(color);
      circle->setValueAngle(hsv.h);
      circle->setValueAxial(hsv.s);
      sliderc->setValue(hsv.v);
      slidera->setValue(hsv.a);
    }
};

class HueCircleEditor_HSV_HV : public HueCircleEditor
{
  public:
    HueCircleEditor_HSV_HV()
    {
      circle = new PartialEditorHueCircle_HSV_HV();
      sliderc = new ChannelSliderType(CC_HSV_S);
      slidera = new ChannelSliderType(CC_A);
      init();
    }

    virtual ColorRGBd getColor() const
    {
      return HSVtoRGB(ColorHSVd(circle->getValueAngle(), sliderc->getValue(), circle->getValueAxial(), slidera->getValue()));
    }

    virtual void setColor(const ColorRGBd& color)
    {
      ColorHSVd hsv = RGBtoHSV(color);
      circle->setValueAngle(hsv.h);
      sliderc->setValue(hsv.s);
      circle->setValueAxial(hsv.v);
      slidera->setValue(hsv.a);
    }
};

class HueCircleEditor_HSL_HS : public HueCircleEditor
{
  public:
    HueCircleEditor_HSL_HS()
    {
      circle = new PartialEditorHueCircle_HSL_HS();
      sliderc = new ChannelSliderType(CC_HSL_L);
      slidera = new ChannelSliderType(CC_A);
      init();
    }

    virtual ColorRGBd getColor() const
    {
      return HSLtoRGB(ColorHSLd(circle->getValueAngle(), circle->getValueAxial(), sliderc->getValue(), slidera->getValue()));
    }

    virtual void setColor(const ColorRGBd& color)
    {
      ColorHSLd hsl = RGBtoHSL(color);
      circle->setValueAngle(hsl.h);
      circle->setValueAxial(hsl.s);
      sliderc->setValue(hsl.l);
      slidera->setValue(hsl.a);
    }
};

class HueCircleEditor_HSL_HL : public HueCircleEditor
{
  public:
    HueCircleEditor_HSL_HL()
    {
      circle = new PartialEditorHueCircle_HSL_HL();
      sliderc = new ChannelSliderType(CC_HSL_S);
      slidera = new ChannelSliderType(CC_A);
      init();
    }

    virtual ColorRGBd getColor() const
    {
      return HSLtoRGB(ColorHSLd(circle->getValueAngle(), sliderc->getValue(), circle->getValueAxial(), slidera->getValue()));
    }

    virtual void setColor(const ColorRGBd& color)
    {
      ColorHSLd hsl = RGBtoHSL(color);
      circle->setValueAngle(hsl.h);
      sliderc->setValue(hsl.s);
      circle->setValueAxial(hsl.l);
      slidera->setValue(hsl.a);
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

class InternalColorPlane : public Element //for internal use
{
  public:
    ColorRGBd color;
    bool selected;
    
    InternalColorPlane();
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
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
    
    
  private:
    InternalColorPlane fg;
    InternalColorPlane bg;
    Arrows arrows;
  
  public:
  
    FGBGColor();
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    
    void setFG(const ColorRGBd& color);
    void setBG(const ColorRGBd& color);
    
    ColorRGBd getFG() const;
    ColorRGBd getBG() const;

    ColorRGB getFG255() const { return RGBdtoRGB(getFG()); }
    ColorRGB getBG255() const { return RGBdtoRGB(getBG()); }

    bool selectedFG() const;
    bool selectedBG() const;
    
    //these are from the ColorEditor interface, returns the currently selected color
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
    
    bool isMultiColorGettable(Plane plane) const { return plane == FG || plane == BG; }
    virtual ColorRGBd getMultiColor(Plane plane) const
    {
      if(plane == FG) return fg.color;
      else if(plane == BG) return bg.color;
      else return RGBd_Black;
    }
    virtual void setMultiColor(Plane plane, const ColorRGBd& color)
    {
      if(plane == FG) fg.color = color;
      else if(plane == BG) bg.color = color;
    }

    //multi color linker: choose which of the multi-colors is linked to the main color
    bool isMultiColorLinker() const { return true; }
    virtual Plane getMultiColorLink() const { return selectedFG() ? FG : BG; }
    void setMultiColorLink(Plane plane)
    {
      if(plane == FG)
      {
        fg.selected = true;
        bg.selected = false;
      }
      else if(plane == BG)
      {
        fg.selected = false;
        bg.selected = true;
      }
    }
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


/*
Palette: contains N * M fixed color buttons
*/
class AColorPalette : public ColorEditor, public ElementComposite
{
  public:

    virtual ~AColorPalette();
    virtual void setPaletteSize(size_t n, size_t m) = 0;
    virtual void setColor(int i, const ColorRGBd& color) = 0;
    void generateDefault();
    void generateVibrant16x16();
};

/*
ColorPalette allows choosing one color and shows the selected one if applicable.
It only uses the left mouse button.
*/
class ColorPalette : public AColorPalette
{
  protected:
    std::vector<InternalColorPlane*> colors;
    int selected;

  public:

    ColorPalette();
    ~ColorPalette();
    void clear();
    virtual void setPaletteSize(size_t n, size_t m);
    virtual void setColor(int i, const ColorRGBd& color);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IInput& input);
    //these are from the ColorEditor interface, returns the currently selected color
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
};

/*
MultiColorPalette allows selecting one color per mouse button (FG, MG, BG)
*/
class MultiColorPalette : public AColorPalette
{
  private:

    ITexture** texture; //for speed, palette is presented on a texture (double pointer due to a const-correctness situation)
    std::vector<ColorRGBd> colors;
    size_t m;
    size_t n;
    mutable bool textureuptodate;

    int selectedfg;
    int selectedmg;
    int selectedbg;
    bool validfg; //to make the ColorEditorSynchronizer system work properly. A color is only valid if it was chosen HERE.
    bool validmg;
    bool validbg;

  public:
  
    MultiColorPalette();
    ~MultiColorPalette();
    void clear();
    virtual void drawImpl(IGUIDrawer& drawer) const;
    void generateTexture() const;
    virtual void setPaletteSize(size_t n, size_t m);
    virtual void setColor(int i, const ColorRGBd& color);
    virtual void handleImpl(const IInput& input);
    //these are from the ColorEditor interface, returns the currently selected color
    virtual ColorRGBd getColor() const;
    virtual void setColor(const ColorRGBd& color);
    virtual bool isMainColorGettable() const;
    bool isMultiColorGettable(Plane plane) const;
    virtual ColorRGBd getMultiColor(Plane plane) const;
    virtual void setMultiColor(Plane plane, const ColorRGBd& color);
};

class ColorEditorSynchronizer
{
  private:
    std::vector<ColorEditor*> editors;
    ColorEditor::Plane mainColorLink;
  
  public:

    ColorEditorSynchronizer();
  
    void add(ColorEditor* editor);
    void remove(ColorEditor* editor);
    void clear();
    
    void handle();
    
    void setColor(const ColorRGBd& color); //set the color of everything
};

} //namespace gui
} //namespace lpi
