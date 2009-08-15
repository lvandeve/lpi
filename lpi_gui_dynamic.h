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
#include <iostream>

#include <vector>
#include <string>

namespace lpi
{
namespace gui
{


class IDynamicPageControl : public Element
{
  public:
    virtual void controlToValue() = 0;
    virtual void valueToControl() = 0;
};

template<typename T>
class TDymamicPageControl : public IDynamicPageControl
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
      this->addSubElement(&box, STICKYCENTERLEFT);
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
    
    virtual void handleWidget(const IGUIInput& input)
    {
      box.handle(input);
    }
    
    virtual void drawWidget(IGUIDrawer& drawer) const
    {
      box.draw(drawer);
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
      this->addSubElement(&line, STICKYHORIZONTALCENTER);

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
    
    virtual void handleWidget(const IGUIInput& input)
    {
      line.handle(input);
    }
    
    virtual void drawWidget(IGUIDrawer& drawer) const
    {
      line.draw(drawer);
    }
};

class DynamicPageControlDummy : public IDynamicPageControl
{
    virtual void controlToValue() {}
    virtual void valueToControl() {}
    virtual void drawWidget(IGUIDrawer& /*drawer*/) const {};
};

class DynamicPage : public Element
{
  private:
    std::vector<IDynamicPageControl*> controls;
    std::vector<std::string> control_names;
    
    double title_width; //number from 0.0 to 1.0
    
    const static int CONTROLHEIGHT = 16;
  
  public:
  
  DynamicPage();
  ~DynamicPage();
  
  void controlToValue();
  void valueToControl();
  
  //to be overidden if wanted: to handle controls without bound values
  virtual void controlToValueCustom() {};
  virtual void valueToControlCustom() {};
  
  //DynamicPage takes ownership of destructor of your added control!
  void addControl(const std::string& name, IDynamicPageControl* control);
  
  virtual void drawWidget(IGUIDrawer& drawer) const;
  virtual void handleWidget(const IGUIInput& input);
};

} //namespace gui
} //namespace lpi
