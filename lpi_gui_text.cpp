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

#include "lpi_gui_text.h"

#include "lodepng.h"
#include "lpi_clipboard.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"
#include <SDL/SDL.h> //this is for the key codes like SDLK_ENTER

#include <iostream>
#include <cstdlib>

namespace lpi
{
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
//GUIINPUTLINE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The InputLine Class Functions
InputLine allows the user to enter a single line of text.

How to use it:
  You can give text color styles with color1, color2, style
  When the draw() function is called, it is drawn and handled, i.e. user unicode key presses are detected, ...
  It is only handled if "active" is true, otherwise it's only drawn and it ignores user input
  Fetch the text with getText or by simply using inputLine.text
  Check if the user pressed enter, clicked on it with mouse, .... with the check() function
*/

//the default constructor
InputLine::InputLine()
: last_draw_time(0)
, sel0(0)
, sel1(0)
{
  this->x0 = 0;
  this->y0 = 0;
  this->l = 0;
  this->type = 0;
  this->title = "";
  this->control_active = 0;
  this->entering_done = false;
  
  this->text = "";
  cursor = 0;
  entered = 0;
}

void InputLine::make(int x, int y, unsigned long l, int type)
{
  this->x0 = x;
  this->y0 = y;
  this->l = l;
  this->type = type;
  this->title = title;
  
  this->text = "";
  cursor = 0;
  entered = 0;
  
  this->setSizeY( /*markup.getHeight()*/8); //TODO use IDrawer2D to determine this text size
  this->setSizeX((title.length() + l) * /*markup.getWidth()*/8);
  
  this->enabled = 1;
}

bool InputLine::enteringDone() const
{
  if(control_active)
  {
    entering_done = true;
    return false;
  }
  else if(entering_done)
  {
    entering_done = false;
    return true;
  }
  else return false;
}

//this function draws the line, and makes sure it gets handled too
void InputLine::drawImpl(IGUIDrawer& drawer) const
{
  static const int FONTSIZE = 8; //TODO: use drawer to find out cursor position in text and such

  if(type == 0) drawer.drawGUIPartText(GPT_TEXTINPUTLINE_TITLE, title, x0, y0, x1, y1);
  
  int titlew, titleh;
  //drawer.calcTextRectSize(titlew, titleh, title, titleFont);
  titlew = FONTSIZE * title.size();
  titleh = FONTSIZE;

  int inputX = x0 + titlew;
  int inputY = (y0 + y1) / 2;
  if(type == 0) drawer.drawGUIPartText(GPT_TEXTINPUTLINE, text, inputX, y0, x1, y1);
  else if(type == 1) //password
  {
    std::string s;
    unsigned long p = 0;
    while(p < text.length() && p < l)
    {
      s += "*";
      p++;
    }
    drawer.drawGUIPartText(GPT_TEXTINPUTLINE, s, inputX, y0, x1, y1);
  }
  else if(type == 2)
  {
    std::string s = valtostr(getInteger());
    drawer.drawGUIPartText(GPT_TEXTINPUTLINE, s, inputX, y0, x1, y1);
  }
  
  //draw the cursor if active
  if(control_active && (int((draw_time - last_draw_time) * 2.0) % 2 == 0 || mouseDown(drawer.getInput())))
  {
    int cursorXDraw = inputX + cursor * FONTSIZE - 1;
    drawer.drawLine(cursorXDraw, inputY - FONTSIZE / 2, cursorXDraw, inputY + FONTSIZE / 2, /*font.color*/RGB_Black);
  }
  
  //draw selection, if any
  if(sel0 != sel1)
  {
    int start = inputX + sel0 * FONTSIZE;
    int end = inputX + sel1 * FONTSIZE;
    drawer.drawRectangle(start, inputY - FONTSIZE / 2, end, inputY + FONTSIZE / 2, ColorRGB(128, 128, 255, 96), true);
  }
}

bool InputLine::enter()
{
  bool result = false;
  if(entered && text.length() > 0 && enabled) result = true;
  entered = 0;
  return result;
}

void InputLine::deleteSelectedText()
{
  int s0 = sel0, s1 = sel1;
  if(s1 < s0) std::swap(s0, s1);
  
  if(s0 < 0) s0 = 0;
  if(s1 < 0) s1 = 0;
  if(s0 > (int)text.length()) s0 = text.length();
  if(s1 > (int)text.length()) s1 = text.length();
  
  if(text.size() > 0) text.erase(s0, s1 - s0);
  
  sel0 = sel1 = cursor = s0;
}

void InputLine::activate(bool i_active)
{
  control_active = i_active;
}

void InputLine::handleImpl(const IInput& input) //both check if you pressed enter, and also check letter keys pressed, backspace, etc...
{
  if(mouseGrabbed(input))
  {
    cursor = mouseToCursor(input.mouseX());
    sel1 = mouseToCursor(input.mouseX());
  }
  if(mouseJustDownHere(input))
  {
    sel0 = mouseToCursor(input.mouseX());
    last_draw_time = draw_time;
  }
  
  bool shift = input.keyDown(SDLK_LSHIFT) || input.keyDown(SDLK_RSHIFT);
  bool ctrl = input.keyDown(SDLK_LCTRL) || input.keyDown(SDLK_RCTRL);
  
  autoActivate(input, auto_activate_mouse_state, control_active);
  if(control_active)
  {
    if(input.keyDown(SDLK_RETURN) || input.keyDown(SDLK_KP_ENTER))
    {
      control_active = false;
      entered = true;
    }
    draw_time = input.getSeconds();
    
    if(cursor >= text.length()) cursor = text.length();

    if(!ctrl)
    {
      int ascii = input.unicodeKey(0.5, 0.025);
      if(ascii)
      {
        last_draw_time = draw_time;
        switch(ascii)
        {
          case 8: //backspace
            //COMMENTED OUT!! For same reason as case 127, except that this one actually worked in both linux and windows, but you know, the fact that 127 doesn't reliably work everywhere doesn't give the feeling that backspase will. Handled with input.keyPressedTime instead.
            /*if(sel0 == sel1 && cursor > 0) text.erase(--cursor, 1);
            else if(sel0 != sel1) deleteSelectedText();*/
            break;
          case 13: //enter
            //COMMENTED OUT!!!! UNICODEKEY DOES NOT RELIABLY SAY THAT THIS KEY IS DOWN, IT MAY MISS THE UP EVENT. SO FOR THIS ENTER FEATURE, I USE THE SIMPLE KEY INPUT WAY INSTEAD.
            //So, do NOTHING with enter
            /*entered = 1;
            control_active = false;*/
            break;
          case 127: //delete
            //COMMENTED OUT!!! works with this in linux, but not in windows, so I check with keyPressedTime instead below
            /*if(sel0 == sel1 && cursor <= text.size()) text.erase(cursor, 1);
            else if(sel0 != sel1) deleteSelectedText();*/
            break;
          //a few certainly not meant to be printed ones
          case 0:
            break;
          case 10: break;
          default:
            if(sel0 != sel1) deleteSelectedText();
            if(text.length() < l) text.insert(cursor, 1, ascii);
            if(text.length() < l || cursor == l - 1) cursor++;
            break;
        }
      }
    }

    if(ctrl)
    {
      if(input.keyPressed(SDLK_v))
      {
        std::string pasteText;
        if(getClipboardString(pasteText))
        {
          if(sel0 != sel1) deleteSelectedText();
          for(size_t i = 0; i < pasteText.size(); i++)
          {
            char c = pasteText[i];
            if(c == 10 || c == 13) break; //no newlines in single-line text
            if(text.length() < l) text.insert(cursor, 1, c);
            if(text.length() < l || cursor == l - 1) cursor++;
          }
          last_draw_time = draw_time;
        }
      }
      bool doCopy = input.keyPressed(SDLK_c);
      bool doCut = input.keyPressed(SDLK_x);
      if(doCopy || doCut)
      {
        if(sel0 != sel1)
        {
          std::string copyText;
		  if ((int)sel1 < (int)sel0)
		  {
		    int tmp = sel0;
			sel0 = sel1;
			sel1 = tmp;
		  }
          for(int i = (int)sel0; i < (int)sel1; i++)
          {
            if(i < 0) { i = -1; continue; }
            if(i >= (int)text.size()) break;
            copyText.push_back(text[i]);
          }
          if(!copyText.empty()) setClipboardString(copyText);
          if(doCut) deleteSelectedText();
        }
      }
      if(input.keyPressed(SDLK_a) || input.keyPressed(SDLK_q)) //the q is for the azerty problem in Windows
      {
        last_draw_time = draw_time;
        selectAll();
      }

    }
    
    if(input.keyPressed(SDLK_HOME))
    {
      last_draw_time = draw_time;
      if(shift)
      {
        if(sel0 == sel1)
        {
          sel0 = cursor;
          sel1 = 0;
        }
        else
        {
          sel1 = 0;
        }
      }
      else selectNone();
      cursor = 0;
    }
    if(input.keyPressed(SDLK_END))
    {
      last_draw_time = draw_time;
      unsigned long pos = 0;
      while(text[pos] != 0 && pos < text.length()) pos++;
      if(shift)
      {
        if(sel0 == sel1)
        {
          sel0 = cursor;
          sel1 = pos;
        }
        else
        {
          sel1 = pos;
        }
      }
      else selectNone();
      cursor = pos;
    }
    if(input.keyPressedTime(SDLK_LEFT, 0.5, 0.025))
    {
      last_draw_time = draw_time;
      if(shift)
      {
        if(sel0 == sel1) sel0 = sel1 = cursor;
        if(cursor > 0) cursor--;
        if(sel1 > 0) sel1--;
      }
      else
      {
        if(sel0 == sel1)
          {if(cursor > 0) cursor--;}
        else
          cursor = sel1 = sel0 = (sel1 > sel0 ? sel0 : sel1);
      }
    }
    if(input.keyPressedTime(SDLK_RIGHT, 0.5, 0.025))
    {
      last_draw_time = draw_time;
      if(shift)
      {
        if(sel0 == sel1) sel0 = sel1 = cursor;
        if(cursor < text.length()) cursor++;
        if(sel1 < text.length()) sel1++;
      }
      else
      {
        if(sel0 == sel1)
          {if(cursor < text.length()) cursor++;}
        else
          cursor = sel0 = sel1 = (sel1 < sel0 ? sel0 : sel1);
      }
    }
    if(input.keyPressedTime(SDLK_DELETE, 0.5, 0.025))
    {
      last_draw_time = draw_time;
      if(sel0 == sel1 && cursor <= text.size()) text.erase(cursor, 1);
      else if(sel0 != sel1) deleteSelectedText();
    }
    if(input.keyPressedTime(SDLK_BACKSPACE, 0.5, 0.025))
    {
      last_draw_time = draw_time;
      if(sel0 == sel1 && cursor > 0) text.erase(--cursor, 1);
      else if(sel0 != sel1) deleteSelectedText();
    }
  }
  else selectNone();
  
  if(mouseDoubleClicked(input))
  {
    last_draw_time = draw_time;
    selectAll();
  }
}

void InputLine::selectAll()
{
  sel0 = 0;
  sel1 = text.size();
}

void InputLine::selectNone()
{
  sel0 = 0;
  sel1 = 0;
}

int InputLine::mouseToCursor(int mouseX) const
{
  static const int FONTSIZE = 8; //TODO: use drawer to find out cursor position in text and such
  int relMouse = mouseX - x0 + FONTSIZE / 2;
  relMouse -= title.length() * FONTSIZE;
  if(relMouse < 0) return 0;
  else if(relMouse / FONTSIZE < int(text.length())) return relMouse / FONTSIZE;
  else return text.length();
}

void InputLine::clear()
{
  setText("");
  cursor = 0;
}

void InputLine::setText(const std::string& i_text)
{
  this->text = i_text;
}

const std::string& InputLine::getText() const
{
  return text;
}

void InputLine::setTitle(const std::string& i_title)
{
  static const int FONTSIZE = 8; //TODO: use drawer to find out cursor position in text and such
  int size_diff = i_title.size() - title.size();
  this->title = i_title;
  growX1(size_diff * FONTSIZE);
}

const std::string& InputLine::getTitle() const
{
  return title;
}

int InputLine::getInteger() const
{
  return std::atoi(text.c_str());
}

int InputLine::getKeyboardFocus() const
{
  if(control_active) return KM_TEXT;
  else return 0;
}

////////////////////////////////////////////////////////////////////////////////

static const int SPINNERW = 16;

ISpinner::ISpinner()
: changed(false)
{
  setEnabled(true);
  
  addSubElement(&line, Sticky(0.0,0, 0.0,0, 1.0,-SPINNERW, 1.0,0));
  
  line.make(0, 0, 10);
}

void ISpinner::drawImpl(IGUIDrawer& drawer) const
{
  drawer.drawRectangle(getX0(), getY0(), getX1(), getY1(), RGB_White, true);
  drawer.drawRectangle(getX0(), getY0(), getX1(), getY1(), RGB_Grey, false);
  line.draw(drawer);
  drawer.drawGUIPart(GP_SPINNER_UP, x1 - SPINNERW, y0, x1, (y0 + y1) / 2);
  drawer.drawGUIPart(GP_SPINNER_DOWN, x1 - SPINNERW, (y0 + y1) / 2, x1, y1);
}

void ISpinner::handleImpl(const IInput& input)
{
  line.handle(input);
  if(mouseOver(input) && input.mouseButtonDownTimed(LMB))
  {
    int x = input.mouseX();
    int y = input.mouseY();
    if(x >= x1 - SPINNERW && x < x1)
    {
      if(y < (y0 + y1) / 2) increment();
      else decrement();
      changed = true;
    }
  }
}

bool ISpinner::hasChanged()
{
  bool result = changed;
  changed = false;
  return result || line.enteringDone();
}

bool ISpinner::isControlActive() const
{
  return line.isControlActive();
}

void ISpinner::activate(bool i_active)
{
  line.activate(i_active);
}

void ISpinner::selectAll()
{
  line.selectAll();
}

void ISpinner::selectNone()
{
  line.selectNone();
}

//////////////////////////////////////////////////////////////////////////////////
////MultiLineText class
//////////////////////////////////////////////////////////////////////////////////

//MultiLineText::MultiLineText()
//{
  //this->text = "";
  //resetLines();
  //char10 = 1;
  //useFormatting = 1;
  //markup = TS_W;
//}

//void MultiLineText::resetLines()
//{
  //line.clear(); //reset lines
  //line.push_back(0); //character 0 is always a new line since it's the beginning of the text
//}

//void MultiLineText::generateLineMarkup()
//{
  //lineMarkup.clear();
  //lineMarkup.push_back(markup);
  
  //Markup markup_ = markup;
  
  //for(unsigned long i = 0; i < getNumLines(); i++)
  //{
    //std::string s = getLine(i);
    //getFormattedMarkup(s, markup_, markup);
    //lineMarkup.push_back(markup_);
  //}
//}

////from now on, this will also split words that are too long into multiple parts
//void MultiLineText::splitWords(int width)
//{
  //resetLines();
  
  //unsigned long charsPerLine = width / markup.getWidth();
  
  //unsigned long pos = 0; //current position in the string
  //unsigned long cline = 1; //current line
  //unsigned long linepos = 0; //*printable* character position on current line, becomes 0 at every "newline"
  
  //while(pos < text.length())
  //{
    ////get length of next word
    //unsigned long wordpos = pos;
    //unsigned long wordlength = 0;
    //unsigned long steplength = 0; //word length + length of possible format symbols
    //bool formattedEnd = false;
    //while(!formattedEnd && !(text[wordpos] == ' ' || text[wordpos] == 0 || (char10 && (text[wordpos] == 10 || text[wordpos] == 13)) || wordlength >= charsPerLine))
    //{
      //if(!useFormatting || text[wordpos] != '#')
      //{
        //wordpos++;
        //wordlength++;
        //steplength++;
      //}
      //else
      //{
        //wordpos++;
        //steplength++;
        //if(wordpos < text.size())
        //{
          //char c = text[wordpos];
          //switch(c)
          //{
            //case 'n':
              //formattedEnd = true; //it breaks out of this loop, and the next part of the function will then detect the 'n' and treat it as newline
              //break;
            //default:
              //wordpos += getFormattedTextAttrSize(c) + 1;
              //wordlength += getFormattedTextSymbolPrintSize(c);
              //steplength += getFormattedTextAttrSize(c) + 1;
              //break;
          //}
        //}
      //}
    //}
    
    ////for now, words longer than width aren't split in two parts

    ////see if word can be drawn on current line
    //if(linepos + wordlength > charsPerLine)
    //{
      //line.push_back(pos); //the word is split by inserting newline in front of the word
      //cline++;
      //linepos = 0; //you're at a new line, so you're at the first char of the new line again
    //}
    
    ////pos pointer now goes to behind the following word
    //pos += steplength;
    //linepos += wordlength; //since you go to behind the word drawn on the current line

    ////now check what is there behind the word: is it a newline? a space? or a null-termination char?
    //switch(text[pos])
    //{
      //case ' ': //space
        //pos++;
        //linepos++;
        //break;
      //case 10: //newline
        //if(char10)
        //{
          //line.push_back(pos);
          //cline++;
          //linepos = 0;
          //pos++;
        //}
        //break;
      //case 13: //ms windows newline, ignore
        //if(char10)
        //{
          //pos++;
          ////linepos++;
        //}
        //break;
      //case 'n': //it's the newline, there's a # in front of it for sure because that was already checked higher
        //if(useFormatting)
        //{
          //line.push_back(pos - 1);
          //cline++;
          //linepos = 0;
          //pos++;
        //}
        //break;
      ///*case 0: //null termination char
        //break;*/
      //default: //not supposed to happen
        ////dprint("WTF? Error in MultiLineText::splitWords");
        //break;
    //}
  //}
  
  //line.push_back(text.length());
  
  //if(useFormatting) generateLineMarkup();
//}

//void MultiLineText::splitLetters(int width)
//{
  //resetLines(); 
  
  //unsigned long charsPerLine = width / markup.getWidth();
  
  //unsigned long pos = 0; //current position in the string
  //unsigned long cline = 1; //current line
  //unsigned long linepos = 0; //character position on current line, becomes 0 at every "newline"
  
  //while(/*text[pos] != 0*/pos < text.length())
  //{
    //switch(text[pos])
    //{
      ///*case 0: //termination char
        //break;*/
      //case 10: //newline char
        //if(char10)
        //{
          //line[cline] = pos;
          //cline++;
          //linepos = 0;
          //pos++;
        //}
        //break;
      //default:
        //if(linepos >= charsPerLine)
        //{
          //line.push_back(pos);
          //cline++;
          //linepos = 0;
        //}
        //pos++;
        //linepos++;
        //break;
    //}
  //}
  
  //line.push_back(text.length());
  
  //if(useFormatting) generateLineMarkup();
//}

//void MultiLineText::splitChar10()
//{
  //resetLines();
  
  //unsigned long pos = 0; //current position in the string
  //unsigned long cline = 1; //current line
  
  //while(/*text[pos] != 0*/pos < text.length())
  //{
    //switch(text[pos])
    //{
      ///*case 0: //termination char
        //break;*/
      //case 10: //newline char
        //line.push_back(pos);
        //cline++;
        //pos++;
        //break;
      //default:
        //pos++;
        //break;
    //}
  //}
  
  //line.push_back(text.length());
  
  //if(useFormatting) generateLineMarkup();
//}

//void MultiLineText::setText(const std::string& text)
//{
  //this->text = text;
//}

//void MultiLineText::addText(const std::string& text)
//{
  //this->text += text;
//}

//void MultiLineText::draw(int x, int y, unsigned long startLine, unsigned long endLine) const
//{
  //Markup markup_ = markup;
  
  //if(startLine == endLine) return;
  
  //unsigned long cline = startLine;
  
  //while(cline < endLine)
  //{
    //int printy = y + markup.getHeight() * (cline - startLine); //y coordinate to draw the text
    //std::string sub = getLine(cline); //string containing the current line
    //if(useFormatting)
    //{
      //if(cline < lineMarkup.size()) markup_ = lineMarkup[cline];
      //printFormattedM(sub, x, printy, markup_, markup);
    //}
    //else print(sub, x, printy, markup);
    //cline++;
  //}
//}

//void MultiLineText::draw(int x, int y) const
//{
  //draw(x, y, 0, getNumLines());
//}

//unsigned long MultiLineText::getNumLines() const
//{
  //if(text.length() == 0) return 0; //no text!
  
  //return line.size() - 1; //the size of the std::vector is also the number of lines, minus one because the last value denotes the position of the final character of the text, which is no new line
//}

//std::string MultiLineText::getLine(unsigned long i) const
//{
  //if(i >= getNumLines()/* || i < 0*/) return "";
  
  //int length = line[i + 1] - line[i];
  
  //int start = line[i]; //the first character of the substring
  //if(char10 && text[line[i]] == 10) //do NOT draw newline chars if it's used as newline (i.e. when char10 is true)
  //{
    //start++;
    //length--;
  //}
  
  //if(length < 0) length = 0;
  
  ////std::string sub = text.substr(start, length);
  //std::string sub;
  //for(int j = start; j < start + length; j++)
  //{
    //if(!(char10 && text[j] == 13)) sub += text[j];
  //}
  
  //return sub;
//}

////returns line and column of character at pos (handy for cursors)
//void MultiLineText::cursorAtCharPos(unsigned long pos, unsigned long& cline, unsigned long& column) const
//{
  //cline = 0;
  //column = 0;
  
  //int extraPos = 0;
  
  //if(text[pos] == 10) {pos--; extraPos++;}

  //if(text.length() == 0)
  //{
    //cline = 0;
    //column = 0;
  //}
  //else if(getNumLines() == 1)
  //{
    //cline = 0;
    //column = pos;
  //}
  //else if(pos < line[1])
  //{
    //cline = 0;
    //column = pos;
  //}
  //else
  //{
    //if(pos >= text.length())
    //{
      //extraPos = pos - text.length() + 1;
      //pos = text.length() - 1;
    //}
  
    //while(line[cline] <= pos)
    //{
      //cline++;
    //}
    //cline--;
    //column = pos - line[cline];
    
    //if(text[line[cline]] == 10) {column--; } //lines start with a 10 char if you pressed enter but this char isn't drawn. If the line does not start with a 10 char, column must be done + 1
  //}
  //column += extraPos;
//}

////returns position of letter at given line and column
//int MultiLineText::charAtCursorPos(unsigned long cline, unsigned long column) const
//{
  //if(getNumLines() <= cline) return text.length();
  
  //unsigned long lineLength;
  
  //if(cline == getNumLines() - 1) lineLength = text.length() - line[cline];
  //else if(cline == 0) 
  //{
    //lineLength = line[1];
    //if(text[line[1]] != 10) lineLength--;
  //}
  //else lineLength = line[cline + 1] - line[cline] - 1;
  
  //unsigned long pos = line[cline];
  
  //if(column > lineLength) column = lineLength;
  
  //if(text[line[cline]] == 10) pos += 1;
  
  //pos += column;
  
  //if(pos > text.length()) pos = text.length();
  
  ///*if(cline == 0)
  //{
    //pos = column;
  //}*/
  
  //return pos;
//}

//////////////////////////////////////////////////////////////////////////////////
////STRINGSTACK///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//The String Stack is a rotating stack of strings, that is very helpful for the
//Console.

//It's actually not a real stack: you don't have to pop() to get values out of it,
//you only have to push values into it, it rotates, and you can access any value.

//You can access values relative to the newest, or relative to the oldest value.

//The StringStack has a finite number of positions, when it's full, it'll overwrite
//the oldest value. It can however use indexes as if the newely added text has always
//the highest index when the stack is full.

//First it must allocate memory for the wanted number of strings, and it must
//destroy it again when it's no longer used.

//num = max number of strings
//*/

//StringStack::StringStack()
//{
  //this->num = 0;
  //this->oldest = 0;
  //this->next = 0;
  //this->numFilled = 0;
//}

//void StringStack::make(int num)
//{
  //if(num < 1) num = 1;
  //this->num = num;
  //this->oldest = 0;
  //this->next = 0;
  //this->numFilled = 0;
  
  //for(int i = 0; i < num; i++) text.push_back("");
//}

//void StringStack::push(const std::string& text)
//{
  //this->text[next] = text;
  //if(next == oldest && numFilled > 0) oldest = increase(oldest);
  //next = increase(next);
  
  //if(numFilled < num) numFilled++;
//}

////returns the text that is position positions away from the oldest text
//const std::string& StringStack::getOldest(int offset)
//{
  //int pointer = increase(oldest, offset);
  //return text[pointer];
//}

//const std::string& StringStack::getNewest(int offset)
//{
  //int pointer = decrease(next);
  //pointer = decrease(pointer, offset);
  
  //return text[pointer];
//}

////returns how full the memory is
//int StringStack::getNumText() const
//{
  //return numFilled;
//}

////increase the position of a pointer (like oldest or next), if it reaches the edge it goes back to 0
//int StringStack::increase(int pointer, int amount)
//{
  //pointer += amount;
  //if(pointer >= num) pointer %= num;
  //return pointer;
//}

//int StringStack::decrease(int pointer, int amount)
//{
  //pointer -= amount;
  //while(pointer < 0) pointer += num;
  //if(pointer >= num) pointer %= num;
  //return pointer;
//}

//////////////////////////////////////////////////////////////////////////////////
////GUICONSOLE////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//Console is a text window that contains different texts and can display
//some of them like a console.

//It's supposed to be used together with a scrollbar to be able to scroll it.
//*/

//Console::Console()
//{
  //this->num = 0;
  
  //this->active = 0;
  //this->visible = 0;

  //this->scroll = 0;
//}

//void Console::make(int x, int y, int sizex, int sizey, int num, const Font& font)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setSizeX(sizex);
  //this->setSizeY(sizey);
  //this->num = num;
  //this->font = font;
  
  //this->setEnabled(true);

  //this->scroll = 0;
  
  //this->stack.make(num);

  //calcNumLines();
//}

//void Console::push(const std::string& text/*, int length*/)
//{
  //stack.push(text);
  //calcNumLines();  
//}

//void Console::calcNumLines()
//{
  //int line = 0;
  //MultiLineText t;
  //t.markup = markup;
  //for(int i = 0; i < num; i++)
  //{
    //t.setText(stack.getOldest(i));
    //t.splitWords(getSizeX());
    //line += t.getNumLines();
  //}
  //numLines = line;
//}

//int Console::getNumLines() const
//{
  //return numLines;
//}

//int Console::getNumMessages() const
//{
  //return stack.getNumText();
//}

//int Console::getVisibleLines(const Font* font)
//{
  //return getSizeY() / markup.getHeight();
//}

//void Console::drawImpl(IGUIDrawer& /*drawer*/) const
//{
  //int line = 0;
  //int message = 0;
  //int maxScreenLines = getSizeY() / markup.getHeight();
  //int messageLines = 0;
  //int messageStartLine = 0;
  //int messageEndLine = 0;
  //int messageStartLineScreen = 0;
  //int messageEndLineScreen = 0;
  //int drawMessageStartLine = -1;
  //int drawMessageEndLine = -1;
  //int yDraw = 0;
   
  //MultiLineText t;
  //t.markup = markup;

  //while(line - scroll < maxScreenLines && message < stack.getNumText())
  //{
    //messageStartLine = line; //first line of the message in all messages
    
    //t.setText(stack.getOldest(message));
    //t.splitWords(getSizeX());
    //messageLines = t.getNumLines();
    ////messageLines = calcTextLines(text[message], sizex, markup); //number of lines of the message
    //messageEndLine = messageStartLine + messageLines - 1; //last line of the message
    //messageStartLineScreen = messageStartLine - scroll;
    //messageEndLineScreen = messageEndLine - scroll;
    //line += messageLines;
    //if(line >= scroll)
    //{
      //yDraw = y0 + (messageStartLine - scroll) * markup.getHeight();
      
      //if(messageStartLineScreen < 0)
      //{
        //drawMessageStartLine = -messageStartLineScreen;
        //yDraw = y0;
      //}
      //else drawMessageStartLine = -1;
      
      //if(messageEndLineScreen >= maxScreenLines) drawMessageEndLine = messageLines - (messageEndLineScreen - maxScreenLines + 1);
      //else drawMessageEndLine = -1;
      
      //t.draw(x0, yDraw, drawMessageStartLine, drawMessageEndLine);
    //}
    
    //message++;
  //}
//}


//////////////////////////////////////////////////////////////////////////////////
////GUITEXTAREA///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//hmm I forgot what a guitextarea is for...
//*/

//TextArea::TextArea()
//{
  //this->active = 0;
  //this->visible = 0;

  //this->scroll = 0;
  
  //this->scrollEnabled = 0;
//}

//void TextArea::make(int x, int y, int sizex, int sizey, const std::string& text, const Font& font)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setSizeX(sizex);
  //this->setSizeY(sizey);
  //this->font = font;
  //this->text.markup = markup;
  
  //this->text.setText(text);
  //this->text.splitWords(getSizeX());

  
  //this->setEnabled(true);

  //this->scroll = 0;
//}

//void TextArea::setText(const std::string& text, const Font& font)
//{
  //this->font = font;
  //this->text.markup = markup;
  
  //this->text.setText(text);
  //int width = getSizeX();
  //if(scrollEnabled) width -= scrollbar.getSizeX();
  //this->text.splitWords(width);

  //this->scroll = 0;
  
  //if(scrollEnabled) setScrollbarSize();
//}


//int TextArea::getNumLines() const
//{
  //return text.getNumLines();
//}

//void TextArea::resizeImpl()
//{
  //this->text.splitWords(getSizeX());
//}

//Element* TextArea::getAutoSubElement(unsigned long i)
//{
  //if(i == 0) return &scrollbar;
  //else return 0;
//}

//void TextArea::handleImpl(const IInput& input)
//{
  //if(scrollEnabled)
  //{
    //scrollbar.handle(input);
    //scroll = int(scrollbar.scrollPos);
  //}
//}

//int TextArea::getVisibleLines(const Font* font) const
//{
  //return getSizeY() / markup.getHeight();
//}

//void TextArea::drawImpl(IGUIDrawer& drawer) const
//{
  //int visible = getVisibleLines(markup);
  ////scroll is the startline, scroll + visible is the end line
  //int startLine = scroll;
  //int endLine = scroll + visible;
  
  //text.draw(x0, y0, startLine, endLine);
  
  //if(scrollEnabled) scrollbar.draw(drawer);
//}

//void TextArea::setScrollbarSize()
//{
  //int size = text.getNumLines() - (getSizeY() / markup.getHeight());
  //if(size < 0) size = 0;
  
  //scrollbar.scrollSize = size;
//}

//void TextArea::addScrollbar()
//{
  //scrollEnabled = true;
  
  //scrollbar.makeVertical(x1 - 16, 0, getSizeY());
  //this->text.splitWords(getSizeX() - scrollbar.getSizeX());
  
  //setScrollbarSize();
//}

//////////////////////////////////////////////////////////////////////////////////
////GUIINPUTBOX///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//The InputBox Class Functions
//InputBox  allows the user to enter a multi line text.
//*/

//InputBox::InputBox()
//{
  //this->visible = 0;
  //this->active = 0;
//}

//void InputBox::make(int x, int y, int sizex, int sizey, int maxLines, int border
                  //, const Font& font
                  ////, BackPanel panel
                  //, const ColorRGB& cursorColor)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setSizeX(sizex);
  //this->setSizeY(sizey);
  //this->maxLines = maxLines;
  //this->font = font;
  //this->multiText.markup = markup;
  //this->panel = panel;
  //this->cursorColor = cursorColor;
  //this->active = 1; 
  //this->visible = 1;
  //this->present = 1;
  //this->cursor = 0;
  //this->control_active = 0;
  //this->border = border;
  
  //init();
//}

//void InputBox::makeScrollbar(const GuiSet* set)
//{
  //bar.makeVertical(x1 - set->arrowN->getU(), y0, getSizeY(),
                   //100, 0, 0, 1,
                   //set, 1);
//}

//void InputBox::init()
//{
  //bar.makeVertical(x1 - builtInGuiSet.arrowN->getU(), y0, getSizeY(), 100, 0, 0, 1);
  //bar.scrollSize = 1;
  //bar.scrollPos = 0;
//}

//int InputBox::getLeftText() const
//{
  //return border;
//}

//int InputBox::getRightText() const
//{
  //return getSizeX() - border - bar.getSizeX();
//}

//int InputBox::getTopText() const
//{
  //return border;
//}

//int InputBox::getBottomText() const
//{
  //return getSizeY() - border;
//}

//int InputBox::getTextAreaHeight() const
//{
  //return getBottomText() - getTopText();
//}

//int InputBox::getTextAreaWidth() const
//{
  //return getRightText() - getLeftText();
//}

//int InputBox::getLinesVisible() const
//{
  //return getTextAreaHeight() / markup.getHeight();
//}

//void InputBox::drawImpl(IGUIDrawer& drawer) const
//{
  //panel.draw(drawer, x0, y0, getSizeX(), getSizeY());
  //bar.draw(drawer);
  
  ////draw the cursor if active
  //if(control_active && int(draw_time * 2.0) % 2 == 0)
  //{
    //unsigned long cursorLine;
    //unsigned long cursorColumn;
    //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    //int cursorXDraw = x0 + getLeftText() + cursorColumn * markup.getWidth();
    //int cursorYDraw = y0 + getTopText() + (cursorLine - firstVisibleLine) * markup.getHeight();
    //if(cursorYDraw >= y0 + getTopText() && cursorYDraw < y0 + getBottomText() - markup.getHeight()) drawer.drawLine(cursorXDraw, cursorYDraw, cursorXDraw, cursorYDraw + markup.getHeight(), cursorColor);
  //}

  //multiText.draw(x0 + getLeftText(), y0 + getTopText(), firstVisibleLine, firstVisibleLine + getLinesVisible());
//}

//void InputBox::handleImpl(const IInput& input)
//{
  //autoActivate(input, auto_activate_mouse_state, control_active);
  //if(!control_active) return;
  
  //draw_time = input.getSeconds();
  
  //bool scrollerMayJump = 0;
  
  //if(input.keyPressed(SDLK_RIGHT)) if(cursor < text.length()) cursor++, scrollerMayJump = 1;
  //if(input.keyPressed(SDLK_LEFT)) if(cursor > 0) cursor--, scrollerMayJump = 1;
  //if(input.keyPressed(SDLK_UP))
  //{
    //unsigned long cursorLine;
    //unsigned long cursorColumn;
    //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    //if(cursorLine > 0) cursor = multiText.charAtCursorPos(cursorLine - 1, cursorColumn);
    
    //scrollerMayJump = 1;
  //}
  //if(input.keyPressed(SDLK_DOWN))
  //{
    //unsigned long cursorLine;
    //unsigned long cursorColumn;
    //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    //cursor = multiText.charAtCursorPos(cursorLine + 1, cursorColumn);
    
    //scrollerMayJump = 1;
  //}
  //if(input.keyPressed(SDLK_HOME))
  //{
    //if(input.keyDown(SDLK_LCTRL)) cursor = 0;
    //else
    //{
      //unsigned long cursorLine;
      //unsigned long cursorColumn;
      //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
      //cursor = multiText.charAtCursorPos(cursorLine, 0);
    //}
    
     //scrollerMayJump = 1;
  //}
  //if(input.keyPressed(SDLK_END))
  //{
    //if(input.keyDown(SDLK_LCTRL)) cursor = text.length();
    //else
    //{
      //unsigned long cursorLine;
      //unsigned long cursorColumn;
      //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
      //cursor = multiText.charAtCursorPos(cursorLine, int(getSizeX() / markup.getWidth()));
    //}
    
     //scrollerMayJump = 1;
  //}

  //int ascii = input.unicodeKey(0.5, 0.025);
  //if(ascii)
  //{
    //switch(ascii)
    //{
      //case 8: //backspace
        //if(cursor > 0)
        //{
          //cursor--;
          //text.erase(cursor, 1);
        //}
        //break;
      ////a few certainly not meant to be printed ones
      //case 0:
        //break; 
      //case 10:
        //break;
      //case 13:
        //text.insert(cursor, 1, 10);
        //cursor++;
        //break;
      //default:
        //text.insert(cursor, 1, ascii);
        //cursor++;
        //break;
    //}

    //multiText.text = text;
    //multiText.splitWords(getTextAreaWidth());
    
    //scrollerMayJump = 1;
    
  //}
  
  //if(mouseDownHere(input) && !bar.mouseOver(input))
  //{
    //unsigned long mouseColumn = (mouseGetRelPosX(input) - getLeftText()) / markup.getWidth();
    //unsigned long mouseLine = (mouseGetRelPosY(input) - getTopText()) / markup.getHeight() + firstVisibleLine;
    
    //cursor = multiText.charAtCursorPos(mouseLine, mouseColumn);
  //}
  
  //int scrollSize = multiText.getNumLines() - getLinesVisible();
  //if(scrollSize < 1) scrollSize = 1;
  //bar.scrollSize = scrollSize;

  //if(mouseScrollUp(input) && !bar.mouseOver(input))
  //{
    //bar.scroll(input, -2);
  //}
  //if(mouseScrollDown(input) && !bar.mouseOver(input))
  //{
    //bar.scroll(input, 2);
  //}
  
  //bar.handle(input);
  
  //firstVisibleLine = int(bar.scrollPos);
  
  //if(scrollerMayJump)
  //{
    //unsigned long cursorLine;
    //unsigned long cursorColumn;
    //multiText.cursorAtCharPos(cursor, cursorLine, cursorColumn);
    
    ////int lineDifference = multiText.getNumLines() - getLinesVisible();
    ////if(lineDifference < 1) lineDifference = 1;
    
    //int cursorRelative = cursorLine - firstVisibleLine; //relative to the first visible line
    
    //if(cursorRelative >= getLinesVisible()) bar.scrollPos = cursorLine - getLinesVisible() + 1;
    //if(cursorRelative < 0) bar.scrollPos = cursorLine;
    ////if(cursorRelative < 0) bar.scrollPos = cursorRelative + 1;

    ////if((cursorRelative + 1) * markup.getHeight()

    ////if(-getTopText() + (cursorRelative + 1) * markup.getHeight() >= 0) bar.scrollPos = cursorRelative + 1;
    ////if(getBottomText() + cursorRelative * markup.getHeight() < (getBottomText() - getTopText())) bar.scrollPos = cursorLine;
  //}
  
//}

//Element* InputBox::getAutoSubElement(unsigned long i)
//{
  //if(i == 0) return &bar;
  //else return 0;
//}

//////////////////////////////////////////////////////////////////////////////////
////GUIFORMATTEDTEXT//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///*
//FormattedText is just some formatted text as child of Element
//*/

//FormattedText::FormattedText()
//{
  //this->visible = 0;
  //this->active = 0;
//}

//void FormattedText::make(int x, int y, const std::string& text, const Font& font)
//{
  //this->x0 = x;
  //this->y0 = y;
  //this->setSizeX(text.length() * /*markup.getWidth()*/8); //TODO: use IDrawer2D to determine text size
  //this->setSizeY(/*markup.getHeight()*/8);
  //this->text = text;
  //this->font = font;
  //this->setEnabled(true);
//}

//void FormattedText::drawImpl(IGUIDrawer& /*drawer*/) const
//{
  //printFormatted(text, x0, y0, markup);
//}

//void FormattedText::setText(const std::string& text)
//{
  //this->text = text;
//}

} //namespace gui
} //namespace lpi

////////////////////////////////////////////////////////////////////////////////

