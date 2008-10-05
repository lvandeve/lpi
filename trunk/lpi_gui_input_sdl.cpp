#include "lpi_gui_input_sdl.h"

#include "lpi_time.h"

namespace lpi
{
namespace gui
{

double GUIInputSDL::getSeconds() const
{
  return lpi::getSeconds();
}

int GUIInputSDL::mouseX() const
{
  return globalMouseX;
}

int GUIInputSDL::mouseY() const
{
  return globalMouseY;
}

bool GUIInputSDL::mouseButtonDown(GUIMouseButton button) const
{
  if(button == GUI_LMB)
    return globalLMB;
  else if(button == GUI_RMB)
    return globalRMB;
  else if(button == GUI_MMB)
    return globalMMB;
  else return false;
}

bool GUIInputSDL::mouseWheelUp() const
{
  return globalMouseWheelUp;
}

bool GUIInputSDL::mouseWheelDown() const
{
  return globalMouseWheelDown;
}

bool GUIInputSDL::keyDown(int key) const
{
  return lpi::keyDown(key);
}

bool GUIInputSDL::keyPressed(int key) const
{
  return lpi::keyPressed(key, &keystate);
}

int GUIInputSDL::unicodeKey(int allowedChars, double time, double warmupTime, double repTime) const
{
  return lpi::unicodeKey(allowedChars, time, warmupTime, repTime);
}

} //namespace gui
} //namespace lpi

namespace lpi
{

gui::GUIInputSDL gGUIInput;

} //namespace lpi