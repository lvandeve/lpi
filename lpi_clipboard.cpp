/*
LodePaint

Copyright (c) 2009 Lode Vandevenne
All rights reserved.

This file is part of LodePaint.

LodePaint is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LodePaint is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LodePaint.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lpi_clipboard.h"

#include "lpi_os.h"

#if defined(LPI_WIN32)

#include "Windows.h"
#include <iostream>
#include <SDL/SDL.h> //for Uint32

namespace lpi
{



bool getClipboardString(std::string& text)
{
  if(IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(NULL))
  {
    text = (char*)GetClipboardData(CF_TEXT);
    CloseClipboard();
    return true;
  }
  return false;
}

bool setClipboardString(const std::string& text)
{
  if (OpenClipboard(NULL))
  {
  	HGLOBAL clipbuffer;
  	EmptyClipboard();
  	clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
  	char* buffer = (char*)GlobalLock(clipbuffer);
  	for(size_t i = 0; i < text.size(); i++) buffer[i] = text[i];
  	buffer[text.size()] = 0;
  	GlobalUnlock(clipbuffer);
  	SetClipboardData(CF_TEXT, clipbuffer);
  	CloseClipboard();
    return true;
  }
  return false;

}

} //namespace lpi

#elif defined(LPI_LINUX)

#define CLIPBOARDDEBUG true//gives debug messages in std::cout

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cstdio>
#include <climits>
#include <cstring>
#include <iostream>

#include "lpi_time.h"

namespace lpi
{

namespace
{

/*
This code is created thanks to the "ad-hoc" tutorials from http://mi.eng.cam.ac.uk/~er258/code/x11.html,
but written from scratch.
Also used for documentation:
http://tronche.com/gui/x/icccm/sec-2.html
xsel.c (search for a recent enough version of it that has INCR related code in it)
*/

//the return values of XGetWindowProperty
struct WindowProperty
{
  unsigned char* prop;
  int actual_format;
  int nitems;
  Atom actual_type;

  WindowProperty()
  : prop(0)
  {
  }

  ~WindowProperty()
  {
    if(prop) XFree(prop);
  }
};

std::string GetAtomName(Display* display, Atom atom)
{
  if(atom == None) return "None";
  char* name = XGetAtomName(display, atom);
  std::string result(name);
  XFree(name);
  return result;
}

void getWindowProperty(WindowProperty& property, Display* display, const Window& window, Atom atom, Bool del)
{
  Atom actual_type;
  int actual_format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char* prop = 0;

  int length = 1024;

  //read all bytes
  do
  {
    if(prop) XFree(prop);
    XGetWindowProperty(display, window, atom, del, length, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop);
    length *= 2;
  } while(bytes_after != 0);

  property.prop = prop;
  property.actual_format = actual_format;
  property.nitems = nitems;
  property.actual_type = actual_type;
}

Atom findAtomOfType(Display* display, const WindowProperty& property, const std::string& datatype)
{
  Atom* atoms = (Atom*)property.prop;

if(CLIPBOARDDEBUG) for(int i = 0; i < property.nitems; i++) std::cout << "available target type: " << GetAtomName(display, atoms[i]) << std::endl;

  for(int i = 0; i < property.nitems; i++)
  {
    if(GetAtomName(display, atoms[i]) == datatype) return atoms[i];
  }

  return None;
}

bool getNextEvent(Display *display, XEvent *event_return)
{
#if 1
//This one doesn't always get the event
  double t = lpi::getSeconds();
  while(!XPending(display))
  {
    if(lpi::getSeconds() - t > 5.0)
    {
if(CLIPBOARDDEBUG) std::cout << "Error: The XNextEvent never came... :(" << std::endl;
      return false; //he's probably never going to send us an event :(
    }
  }
  XNextEvent(display, event_return);
  return true;
#else
//This one blocks forever when seeing complex visuals fullscreen (so > 300K data), pressing ctrl + printscreen, and then pasting it in the program, causing INCR transfer
//ACTUALLY, good news, Gimp also hangs in this scenario. KDE 3.5 is the bug!
  XNextEvent(display, event_return);
  return true;
#endif
}

void getIncrData(std::vector<unsigned char>& data, const XSelectionEvent& selevent)
{
if(CLIPBOARDDEBUG) std::cout << "Incremental transfer starting due to INCR property" << std::endl;
  XEvent event;
  XSelectInput(selevent.display, selevent.requestor, PropertyChangeMask);
  XDeleteProperty(selevent.display, selevent.requestor, selevent.property); //this SHOULD start the INCR mechanism (but in KDE 3.5 I don't get any events after this???)

  for(;;)
  {
    if(!getNextEvent(selevent.display, &event)) break;
    if(event.type == PropertyNotify)
    {
      if (event.xproperty.state != PropertyNewValue) continue;
      WindowProperty property;
      getWindowProperty(property, selevent.display, selevent.requestor, selevent.property, False);
      size_t num_bytes = property.nitems * property.actual_format / 8;
if(CLIPBOARDDEBUG) std::cout<<"INCR data size: " << num_bytes << std::endl;
      for(size_t i = 0; i < num_bytes; i++) data.push_back(property.prop[i]);
      XDeleteProperty(selevent.display, selevent.requestor, selevent.property);
      if(num_bytes == 0) break;
    }
    else break;
  }
}

static Display* display = 0;

void clearClipboard()
{
  if(!display) display = XOpenDisplay(0);
  Atom ATOM_CLIPBOARD = XInternAtom(display, "CLIPBOARD", 0);
  
  XSetSelectionOwner(display, ATOM_CLIPBOARD, None, CurrentTime);
  XSync(display, False);
}

static std::string REMEMBER_TEXT;

}

bool getClipboardString(std::string& text)
{
  if(!display) display = XOpenDisplay(0);
  int screen = DefaultScreen(display);
  Window root = RootWindow(display, screen);

  //dummy window
  Window window = XCreateSimpleWindow(display, root, 0, 0, 100, 100, 0, BlackPixel(display, screen), BlackPixel(display, screen));

  Atom ATOM_TARGETS = XInternAtom(display, "TARGETS", False); //possible formats in which source program can output the data
  Atom ATOM_CLIPBOARD = XInternAtom(display, "CLIPBOARD", 0);

  XConvertSelection(display, ATOM_CLIPBOARD, ATOM_TARGETS, ATOM_CLIPBOARD, window, CurrentTime);
  XFlush(display);

  std::vector<unsigned char> data;
  
  XEvent event;
  bool sent_request = false;
  Atom text_atom = None;
  for(;;)
  {
    if(!getNextEvent(display, &event)) break;
    if(event.type == SelectionNotify)
    {
      Atom target = event.xselection.target;

if(CLIPBOARDDEBUG) std::cout << "target atom name: " << GetAtomName(display, target) << std::endl;

      if(event.xselection.property != None)
      {
        WindowProperty property;
        getWindowProperty(property, display, window, ATOM_CLIPBOARD, False);

if(CLIPBOARDDEBUG) std::cout << "property atom name: " << GetAtomName(display, property.actual_type) << std::endl;

        if(target == ATOM_TARGETS && !sent_request)
        {
          //property.prop now contains a list of Atoms, and each atom has a datatype associated with it
          sent_request = true;
          text_atom = findAtomOfType(display, property, "STRING");

          if(text_atom != None) XConvertSelection(display, ATOM_CLIPBOARD, text_atom, ATOM_CLIPBOARD, window, CurrentTime);
          else break;
        }
        else if(target == text_atom)
        {
          if(GetAtomName(display, property.actual_type) == "STRING")
          {
            //property.prop now contains actual data bytes
            size_t num_bytes = property.nitems * property.actual_format / 8;
if(CLIPBOARDDEBUG) std::cout<<"data size: " << num_bytes << std::endl;
            data.resize(num_bytes);
            for(size_t i = 0; i < data.size(); i++) data[i] = property.prop[i];
            break;
          }
          else if(GetAtomName(display, property.actual_type) == "INCR")
          {
            //XConvertSelection(display, ATOM_CLIPBOARD, ATOM_TARGETS, ATOM_CLIPBOARD, window, CurrentTime);
            //XFlush(display);
            getIncrData(data, event.xselection);
            break;
          }
        }
        else break;
      }
      else break;
    }
  }
  
  //XCloseDisplay(display);
  XDestroyWindow(display, window);
  
  if(data.empty())
  {
    if(REMEMBER_TEXT.empty())
    {
if(CLIPBOARDDEBUG) std::cout<<"no text on clipboard (or error)" << std::endl;
      return false;
    }
    else
    {
      text = REMEMBER_TEXT;
      return true;
    }
  }
  else
  {
    for(size_t i = 0; i < data.size(); i++) text.push_back(data[i]);
//if(CLIPBOARDDEBUG) std::cout<<"text: " << text << std::endl;
    return true;
  }
  
  
}


bool setClipboardString(const std::string& text)
{
  /*
  This is pretty unfriendly towards other applications, but for now, for simplicity, I don't actually
  properly put data on the X clipboard. Instead, I clear the clipboard, so the internal system won't see
  any outside clipboard data, and remember the datainternally here (in REMEMBER_TEXT).
  */
  
  clearClipboard();
  REMEMBER_TEXT = text;
  return true;
}

} //namespace lpi

#endif
