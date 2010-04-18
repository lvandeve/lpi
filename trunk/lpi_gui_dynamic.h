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
#include "lpi_gui_file.h"
#include "lpi_parse.h"
#include <iostream>

#include <vector>
#include <map>
#include <string>

namespace lpi
{
namespace gui
{

//TODO: consider doing "controlToValue" always in the handle function of the controls that edit the value a pointer points to

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
    
    void ctor(T valmin, T valmax, const IGUIDrawer& geom)
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
  
    DynamicSlider(const IGUIDrawer& geom)
    {
      ctor(0.0, 1.0, geom);
    }
    
    DynamicSlider(T* value, T valmin, T valmax, const IGUIDrawer& geom)
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

template<typename T>
class DynamicSpinner : public TDymamicPageControl<T>
{
  private:
    SpinnerNumerical<T> spinner;
    
    void ctor()
    {
      this->resize(0, 0, 20, 20);
      spinner.move(0, (this->getSizeY() - spinner.getSizeY()) / 2);
      this->addSubElement(&spinner, Sticky(0.0,0, 0.0,0, 1.0,0, 1.0,0));
    }
  
  public:
  
    DynamicSpinner(T step=(T)1, bool hasmin=false, T minval=(T)0, bool hasmax=false, T maxval=(T)1)
    : spinner(step, hasmin, minval, hasmax, maxval)
    {
      ctor();
    }
    
    DynamicSpinner(T* value, T step=(T)1, bool hasmin=false, T minval=(T)0, bool hasmax=false, T maxval=(T)1)
    : spinner(step, hasmin, minval, hasmax, maxval)
    {
      TDymamicPageControl<T>::bind = value;
      ctor();
      spinner.setValue(*value);
    }
  
    virtual void getValue(T* value)
    {
      *value = spinner.getValue();
    }
    
    virtual void setValue(T* value)
    {
      spinner.setValue(*value);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      spinner.handle(input);
    }

    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      spinner.draw(drawer);
    }
};

/*
DynamicSliderSpinner: has slider, input box AND spinner, for extra deluxe convenience
*/
template<typename T>
class DynamicSliderSpinner : public TDymamicPageControl<T>
{
  private:
    SpinnerNumerical<T> spinner;
    Slider slider;
    T slidervalmin;
    T slidervalmax;
    T spinnervalmin;
    T spinnervalmax;
    
    void ctor(T slidervalmin, T slidervalmax, T spinnervalmin, T spinnervalmax, const IGUIDrawer& geom)
    {
      this->slidervalmin = slidervalmin;
      this->slidervalmax = slidervalmax;
      this->spinnervalmin = spinnervalmin;
      this->spinnervalmax = spinnervalmax;
      static const int TEXTSIZE = 64;
      this->resize(0, 0, TEXTSIZE * 2, 20);
      
      slider.makeSmallHorizontal(0, 0, this->getSizeX() - TEXTSIZE, 1.0, geom);
      slider.move(0, (this->getSizeY() - slider.getSizeY()) / 2);
      this->addSubElement(&slider, Sticky(0.0, 0, 0.5, -slider.getSizeY() / 2, 1.0, -TEXTSIZE, 0.5, slider.getSizeY() / 2));
      
      static const int H = 16;
      spinner.resize(slider.getX1(), (this->getSizeY() - H) / 2, slider.getX1() + TEXTSIZE, (this->getSizeY() - H) / 2 + H);
      this->addSubElement(&spinner, Sticky(1.0, -TEXTSIZE, 0.5, -spinner.getSizeY() / 2, 1.0, 0, 0.5, spinner.getSizeY() / 2));
    }

  protected:
  
    T getSliderValue() const
    {
      double val = slider.getRelValue();
      return (T)(slidervalmin + val * (slidervalmax - slidervalmin));
    }
    
    void setSliderValue(T val)
    {
      slider.setRelValue((double)(val - slidervalmin) / (double)(slidervalmax - slidervalmin));
    }
  
  public:
    
    DynamicSliderSpinner(T* value, T valmin, T valmax, T step, const IGUIDrawer& geom)
    : spinner(step, true, valmin, true, valmax)
    {
      TDymamicPageControl<T>::bind = value;
      ctor(valmin, valmax, valmin, valmax, geom);
      setValue(value);
    }
  
    DynamicSliderSpinner(T* value, T slidervalmin, T slidervalmax, T spinnervalmin, T spinnervalmax, T step, const IGUIDrawer& geom)
    : spinner(step, true, spinnervalmin, true, spinnervalmax)
    {
      TDymamicPageControl<T>::bind = value;
      ctor(slidervalmin, slidervalmax, spinnervalmin, spinnervalmax, geom);
      setValue(value);
    }
  
    virtual void getValue(T* value)
    {
      *value = spinner.getValue();
    }
    
    virtual void setValue(T* value)
    {
      spinner.setValue(*value);
      setSliderValue(*value);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      spinner.handle(input);
      slider.handle(input);
      if(spinner.hasChanged())
        setSliderValue(spinner.getValue());
      if(slider.mouseGrabbed(input))
        spinner.setValue(getSliderValue());
    }
    
    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      spinner.draw(drawer);
      slider.draw(drawer);
    }
};

template<typename T>
class DynamicEnum : public TDymamicPageControl<T>
{
  protected:
    DropDownList list;
    std::map<T, size_t> enumtoindex;
    std::map<size_t, T> indextoenum;

  public:
    DynamicEnum(T* value, const std::vector<std::string>& names, const std::vector<T> values, const IGUIDrawer& geom)
    : list(geom)
    {
      TDymamicPageControl<T>::bind = value;
      list.setItems(names);
      for(size_t i = 0; i < values.size(); i++)
      {
        enumtoindex[values[i]] = i;
        indextoenum[i] = values[i];
      }
      
      list.resize(0, 0, 1, 16);
      this->addSubElement(&list, Sticky(0.0, 0, 0.5,  -list.getSizeY() / 2, 1.0, 0, 0.5, list.getSizeY() / 2));
      setValue(value);
    }
    
    virtual void getValue(T* value)
    {
      *value = indextoenum[list.getSelectedItem()];
    }
    
    virtual void setValue(T* value)
    {
      list.setSelectedItem(enumtoindex[*value]);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      list.handle(input);
    }

    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      list.draw(drawer);
    }
};

//a simple way to make a boolean that has two names instead of being a simple checkbox, with a dropdown. So this is the same as DynamicEnum, but with only two states and a boolean as parameter type.
class DynamicDropDownBool : public TDymamicPageControl<bool>
{
  protected:
    DropDownList list;

  public:
    DynamicDropDownBool(bool* value, const std::string& nameFalse, const std::string& nameTrue, const IGUIDrawer& geom)
    : list(geom)
    {
      TDymamicPageControl<bool>::bind = value;
      std::vector<std::string> names; names.push_back(nameFalse); names.push_back(nameTrue);
      list.setItems(names);
      
      list.resize(0, 0, 1, 16);
      this->addSubElement(&list, Sticky(0.0, 0, 0.5,  -list.getSizeY() / 2, 1.0, 0, 0.5, list.getSizeY() / 2));
      setValue(value);
    }
    
    virtual void getValue(bool* value)
    {
      *value = list.getSelectedItem() == 1;
    }
    
    virtual void setValue(bool* value)
    {
      list.setSelectedItem(*value ? 1 : 0);
    }
    
    virtual void handleImpl(const IInput& input)
    {
      list.handle(input);
    }

    virtual void drawImpl(IGUIDrawer& drawer) const
    {
      list.draw(drawer);
    }
};

class DynamicColor : public TDymamicPageControl<ColorRGB>
{
  private:
    PColorPlane box;

    ColorDialog edit;
    
    void ctor();
    
  public:
  
    DynamicColor(ColorRGB* value, const IGUIDrawer& geom);
    virtual void getValue(ColorRGB* value);
    virtual void setValue(ColorRGB* value);
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
};

class DynamicColord : public TDymamicPageControl<ColorRGBd>
{
  private:
    PColorPlaned box;

    ColorDialog edit;

    void ctor();

  public:

    DynamicColord(ColorRGBd* value, const IGUIDrawer& geom);
    virtual void getValue(ColorRGBd* value);
    virtual void setValue(ColorRGBd* value);
    virtual void handleImpl(const IInput& input);
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
};

class DynamicFile : public TDymamicPageControl<std::string>
{
  private:
    InputLine line;
    Dummy pick;
    FileDialog edit;

    void ctor();

  public:

    DynamicFile(std::string* value, const IGUIDrawer& geom, IFileBrowse* browser, FileDialogPersist* persist);
    virtual void getValue(std::string* value);
    virtual void setValue(std::string* value);
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
    class IDynamicRow
    {
      public:
        virtual int getHeight() const = 0;
        virtual void draw(int x0, int y0, int x1, int y1, IGUIDrawer& drawer) const = 0;
        virtual void handle(const IInput& input) { (void)input; }
        virtual void controlToValue() {};
        virtual void valueToControl() {};
        virtual void manageHover(IHoverManager& hover) { (void)hover; }
        
        std::string tooltip;
    };
    
    class RowControl : public IDynamicRow
    {
      public:
        RowControl(IDynamicControl* control, const std::string& name) : control(control), name(name) {}
        ~RowControl();
        IDynamicControl* control;
        std::string name;

        virtual int getHeight() const { return CONTROLHEIGHT; }
        virtual void draw(int x0, int y0, int x1, int y1, IGUIDrawer& drawer) const;
        virtual void handle(const IInput& input) { control->handle(input); }
        virtual void controlToValue() {control->controlToValue();}
        virtual void valueToControl() {control->valueToControl();}
        virtual void manageHover(IHoverManager& hover) { control->manageHover(hover); }
    };
    
    class RowText : public IDynamicRow
    {
      public:
        RowText(const std::string& text) : text(text) {}
        std::string text;
        virtual int getHeight() const { return CONTROLHEIGHT; }
        virtual void draw(int x0, int y0, int x1, int y1, IGUIDrawer& drawer) const;
    };

    std::vector<IDynamicRow*> rows;
    
    bool enableTitle;
    std::string title; //title on top of this page

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
    size_t addControl(const std::string& name, IDynamicControl* control);
    size_t addTextRow(const std::string& text);
    void addToolTipToLastRow(const std::string& text);

    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void manageHoverImpl(IHoverManager& hover);
    virtual void handleImpl(const IInput& input);
    
    virtual void drawToolTip(IGUIDrawer& drawer) const;
};

} //namespace gui
} //namespace lpi
