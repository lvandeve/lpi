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
#include "lpi_gui_color.h"
#include "lpi_parse.h"
#include <iostream>

#include <vector>
#include <string>

namespace lpi
{
namespace gui
{


class IDynamicControl : public ElementComposite
{
  public:
    virtual void controlToValue() = 0;
    virtual void valueToControl() = 0;
};

template<typename T>
class TDymamicPageControl : public IDynamicControl
{
  protected:
  
    T* bind; //auto-bind copy of value
    
  protected:
    virtual void controlToValueCustom() {};
    virtual void valueToControlCustom() {};
    virtual void getValue(T* value) = 0;
    virtual void setValue(T* value) = 0;
  
  public:
    TDymamicPageControl()
    : bind(0)
    {
    }
    
    void bindValue(T* value)
    {
      bind = value;
    }
    
    //not to be overidden anymore
    virtual void controlToValue()
    {
      if(bind)
        getValue(bind);
      
      controlToValueCustom();
    }
    
    virtual void valueToControl()
    {
      valueToControlCustom();
      
      if(bind)
        setValue(bind);
    }
  
};

class DynamicCheckbox : public TDymamicPageControl<bool>
{
  private:
    Checkbox box;
    
    void ctor()
    {
      this->resize(0, 0, 20, 20);
      box.makeSmall(0, 0);
      box.move(0, (getSizeY() - box.getSizeY()) / 2);
      this->addSubElement(&box, Sticky(0.0, 0, 0.5, -box.getSizeY() / 2, 0.0, box.getSizeX(), 0.5, box.getSizeY() / 2));
    }
    
  public:
  
    DynamicCheckbox()
    {
      ctor();
    }
    
    DynamicCheckbox(bool* value)
    {
      TDymamicPageControl<bool>::bind = value;
      ctor();
      box.setChecked(*value);
    }
  
    virtual void getValue(bool* value)
    {
      *value = box.isChecked();
    }
    
    virtual void setValue(bool* value)
    {
      box.setChecked(*value);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      box.handle(input);
    }
    
    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      box.draw(drawer);
    }
};

template<typename T>
class DynamicSlider : public TDymamicPageControl<T>
{
  private:
    InputLine line;
    Slider slider;
    T valmax;
    T valmin;
    
    void ctor(T valmin, T valmax, const IGUIPartGeom& geom)
    {
      this->valmin = valmin;
      this->valmax = valmax;
      static const int TEXTSIZE = 64;
      this->resize(0, 0, TEXTSIZE * 2, 20);
      
      slider.makeSmallHorizontal(0, 0, this->getSizeX() - TEXTSIZE, 1.0, geom);
      slider.move(0, (this->getSizeY() - slider.getSizeY()) / 2);
      this->addSubElement(&slider, Sticky(0.0, 0, 0.5, -slider.getSizeY() / 2, 1.0, -TEXTSIZE, 0.5, slider.getSizeY() / 2));
      
      line.make(0, 0, 256);
      //line.move(0, (this->getSizeY() - line.getSizeY()) / 2);
      line.resize(slider.getX1(), (this->getSizeY() - line.getSizeY()) / 2, slider.getX1() + TEXTSIZE, (this->getSizeY() - line.getSizeY()) / 2 + line.getSizeY());
      this->addSubElement(&line, Sticky(1.0, -TEXTSIZE, 0.5, -line.getSizeY() / 2, 1.0, 0, 0.5, line.getSizeY() / 2));
    }

  protected:
  
    T getSliderValue() const
    {
      double val = slider.getRelValue();
      return valmin + val * (valmax - valmin);
    }
    
    void setSliderValue(T val)
    {
      slider.setRelValue((double)(val - valmin) / (double)(valmax - valmin));
    }
  
  public:
  
    DynamicSlider(const IGUIPartGeom& geom)
    {
      ctor(0.0, 1.0, geom);
    }
    
    DynamicSlider(T* value, T valmin, T valmax, const IGUIPartGeom& geom)
    {
      TDymamicPageControl<T>::bind = value;
      ctor(valmin, valmax, geom);
      setValue(value);
    }
  
    virtual void getValue(T* value)
    {
      *value = strtoval<T>(line.getText());
    }
    
    virtual void setValue(T* value)
    {
      line.setText(valtostr<T>(*value));
      setSliderValue(*value);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      line.handle(input);
      slider.handle(input);
      if(line.enteringDone())
        setSliderValue(strtoval<T>(line.getText()));
      if(slider.mouseGrabbed(input))
        line.setText(valtostr<T>(getSliderValue()));
    }
    
    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      line.draw(drawer);
      slider.draw(drawer);
    }
};

template<typename T>
class DynamicValue : public TDymamicPageControl<T>
{
  private:
    InputLine line;
    
    void ctor()
    {
      this->resize(0, 0, 20, 20);
      line.make(this->x0, this->y0, 256);
      line.move(0, (this->getSizeY() - line.getSizeY()) / 2);
      this->addSubElement(&line, Sticky(0.0, 0, 0.5,  -line.getSizeY() / 2, 1.0, 0, 0.5, line.getSizeY() / 2));

    }
  
  public:
  
    DynamicValue()
    {
      ctor();
    }
    
    DynamicValue(T* value)
    {
      TDymamicPageControl<T>::bind = value;
      ctor();
      line.setText(valtostr<T>(*value));
    }
  
    virtual void getValue(T* value)
    {
      *value = strtoval<T>(line.getText());
    }
    
    virtual void setValue(T* value)
    {
      line.setText(valtostr<T>(*value));
    }
    
    virtual void handleImpl(const IInput& input)
    {
      line.handle(input);
    }

    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      line.draw(drawer);
    }
};

class DynamicColor : public TDymamicPageControl<ColorRGB>
{
  private:
    PColorPlane box;

    ColorDialogSmall edit;
    
    void ctor();
    
  public:
  
    DynamicColor(ColorRGB* value, const IGUIPartGeom& geom);
    virtual void getValue(ColorRGB* value);
    virtual void setValue(ColorRGB* value);
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
};

class DynamicControlDummy : public IDynamicControl
{
    virtual void controlToValue() {}
    virtual void valueToControl() {}
    virtual void drawImpl(IGUIDrawer& /*drawer*/) const {};
};

class DynamicPage : public ElementComposite
{
  private:
    std::vector<IDynamicControl*> controls;
    std::vector<std::string> control_names;
    
    bool enableTitle;
    std::string title; //title on top of this page

    double title_width; //title of controls; number from 0.0 to 1.0

    const static int CONTROLHEIGHT = 16;
    const static int TITLEHEIGHT = 16;
  
  public:
  
  DynamicPage();
  ~DynamicPage();
  
  void controlToValue();
  void valueToControl();
  
  void setTitle(const std::string& title);
  
  //to be overidden if wanted: to handle controls without bound values
  virtual void controlToValueCustom() {};
  virtual void valueToControlCustom() {};
  
  //DynamicPage takes ownership of destructor of your added control!
  void addControl(const std::string& name, IDynamicControl* control);
  
  virtual void drawImpl(IGUIDrawer& drawer) const;
  virtual void manageHoverImpl(IHoverManager& hover);
  virtual void handleImpl(const IInput& input);
};

} //namespace gui
} //namespace lpi
