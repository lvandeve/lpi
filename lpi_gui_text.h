/*
Copyright (c) 2005-2008 Lode Vandevenne
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


/*
text related GUI elements for lpi_gui

NOTE: some of these elements may currently be broken due to refactoring and will be fixed later

TODO: fix possible broken elements
*/

#ifndef LPI_GUI_TEXT_H_INCLUDED
#define LPI_GUI_TEXT_H_INCLUDED

#include "lpi_gui.h"


namespace lpi
{
namespace gui
{

class InputLine : public Element //input text line
{
  private:
    //unsigned long titleLength;
    unsigned long cursor; //position of the cursor (0 = before first char)
    bool entered; //after you pressed enter!
    double draw_time; //for drawing the blinking cursor
    MouseState auto_activate_mouse_state;
    bool control_active;
    mutable bool entering_done;
    size_t sel0; //selection start
    size_t sel1; //selection end
    
    int mouseToCursor(int mouseX) const; //absolute mouse position to cursor position
    
  public:
    unsigned long l; //max length
    Font font;
    Font titleFont;
    ColorRGB cursorColor;
    /*
    types:
    0: normal text line
    1: password (displays * instead of text)
    2: integer number
    */
    int type;
    std::string title;
    std::string text;
        
    InputLine();
    void make(int x, int y, unsigned long l,
              const Font& font = FONT_Default,
              int type = 0, const std::string& title = "", const Font& titleFont = FONT_Default, const ColorRGB& cursorColor = RGB_White);
    
    virtual void drawImpl(IGUIDrawer& drawer) const;
    virtual void handleImpl(const IGUIInput& input);

    void setText(const std::string& i_text);
    const std::string& getText() const;
    
    void setTitle(const std::string& i_title);
    const std::string& getTitle() const;
    
    bool isControlActive() const { return control_active; } //i.e., the cursor is blinking and it's listening to keyboard input
    
    bool enteringDone() const; //returns true if the user has been changing (or not) the text but now is done editing because the control is no longer active for the first time since then
   
    void clear();

    bool enter();
    int getInteger() const; //returns any integer number that may be in the string you typed
    
    void selectAll();
    void deleteSelectedText();
    
    void activate(bool i_active = true);
};

///*
//MultiLineText
//text divided over multiple lines, and functions 
//to let it create the lines in different ways, such as (including, but not limited to)

//-use ascii 10 chars of the string for new lines
//-break off between letters in limited rectangle
//-break off between words in limited rectangle

//the multiple line text is represented by an array of integers (line) containing the positions of the first letter of each new line

//all functions that need to draw text this way HAVE to use this class from now on
//*/
//class MultiLineText
//{
  //private:
    //std::vector<unsigned long> line; //if the value is -1, it's no new line, it means the end is reached
    //std::vector<Markup> lineMarkup; //only used of useFormatting is true: then it has to remember the start-markup of every line, which will be generated at the start with the function generateLineMarkup
  //public:
    //std::string text;
    
    //Font font; //the markup currently used
    
    //bool char10; //if true, ascii code 10 will act as newline and ascii code 13 will be ignored
    //bool useFormatting; //if true, the # codes of formatted text are used. See the printFormatted function for more about this!
    
    //void resetLines();
    
    //void splitWords(int width); //markup must be given to be able to calculate width ==> max amount of letters in one line
    //void splitLetters(int width);
    //void splitChar10(); //only make a new line at ascii code 10 characters
    
    //void generateLineMarkup(); //only used it useFormatting is true
    
    //void setText(const std::string& text);
    //void addText(const std::string& text);
    
    //unsigned long getNumLines() const;
    
    //std::string getLine(unsigned long i) const; //returns string filled with only that line
    
    
    //void draw(int x, int y, unsigned long startLine, unsigned long endLine) const;
    //void draw(int x, int y) const;
    
    //void cursorAtCharPos(unsigned long pos, unsigned long& cline, unsigned long& column) const;
    //int charAtCursorPos(unsigned long cline, unsigned long column) const;
    
    //MultiLineText();
//};

////The String Stack
//class StringStack
//{
  //public:
    //int num; //max number of strings on your stringstack
    
    //int next; //physical address of next string that'll be written
    //int oldest; //physical address of oldest string that was written
    
    //std::vector<std::string> text;
    
    //StringStack();
    //StringStack(int num);
    
    ////void makeBuffer(int num);
    ////void deleteBuffer();
    //void make(int num);
    //void push(const std::string& text);
    //int increase(int pointer, int amount = 1);
    //int decrease(int pointer, int amount = 1);
    //int getNumText() const;
    //const std::string& getOldest(int offset = 0);
    //const std::string& getNewest(int offset = 0);
  //private:
    //int numFilled; //how much of the memory places are filled currently
//};

////The console (only shows output text, use for example InputLine to input text into the console)
//class Console : public Element
//{
  //private:
    //int numLines; //how much actual lines does it take on screen
    //mutable StringStack stack; //mutable because it changes while drawing this widget
    //void calcNumLines(); //calculates number of lines from the current messages; to be recalculated when sizex changes or messages are pushed/popped

  //public:
    //int scroll; //how much lines away from the first line (= oldest) are you scrolled, note: line != message, message can be multiple lines big!
    //int num; //max number of messages
    
    ////for now, no special text markup yet. All text has the same color and format.
    //Font font;
    
    //int getNumLines() const;
    //int getNumMessages() const;
    //int getVisibleLines(const Font* font);
    //virtual void drawImpl(IGUIDrawer& drawer) const;
    
    //void push(const std::string& text);
    //Console();
    //void make(int x, int y, int sizex, int sizey, int num, const Font& font = FONT_Default);
//};

////The textarea: can show you a single multilinetext and allows you to scroll (if you use optional or separate scrollbar or something else)
//class TextArea : public Element
//{
  //public:
    //int scroll; //how much lines away from the first line
    
    //MultiLineText text;
    
    //Font font;
    
    //int getNumLines() const;
    //int getVisibleLines(const Font* font) const;
    //virtual void drawImpl(IGUIDrawer& drawer) const;
    //virtual void handleImpl(const IGUIInput& input);
    //virtual void resizeImpl();

    //TextArea();
    //void make(int x, int y, int sizex, int sizey, const std::string& text, const Font& font = FONT_Default);
    //void setText(const std::string& text, const Font& font = FONT_Default);
    
    /////optional scrollbar
    //Scrollbar scrollbar;
    //bool scrollEnabled;
    //void addScrollbar();
    //void setScrollbarSize();
    
  //protected:
    //virtual Element* getAutoSubElement(unsigned long i);
//};

//class InputBox : public Element
//{
  //private:
    //unsigned long cursor; //position of cursor in the text, column and line position will be calculated by the MultiLineText
    //int textWidth; //sizex - width of the scrollbar
    //int firstVisibleLine; //depends on the scrollbar position
    
    //MultiLineText multiText;
    //std::string text;
    
    //double draw_time; //for drawing the blinking cursor
    
    //void init();
    
    //MouseState auto_activate_mouse_state;
    //bool control_active;
    
  //public:
    //int maxLines;
    
    //InputBox();
    //void make(int x, int y, int sizex, int sizey, int maxLines, int border = 2,
              //const Font* font = 0,
              ////BackPanel panel = COLORPANEL(RGB_White),
              //const ColorRGB& cursorColor = RGB_Black);
    //void makeScrollbar(const GuiSet* set = &builtInGuiSet);
    //const std::string& getText() const { return text; }
    
    //virtual void drawImpl(IGUIDrawer& drawer) const;
    //virtual void handleImpl(const IGUIInput& input);
    
    //int border;
    
    ////get the positions (relative to the area's x and y) of the text area
    //int getLeftText() const;
    //int getRightText() const;
    //int getTopText() const;
    //int getBottomText() const;
    //int getTextAreaHeight() const;
    //int getTextAreaWidth() const;
    //int getLinesVisible() const; //how much lines of text are visible
    
    //Scrollbar bar;
    
    //BackPanel panel;
    
    //Font font;
    //ColorRGB cursorColor;
    
  //protected:
    //virtual Element* getAutoSubElement(unsigned long i);
//};


//class FormattedText : public Element
//{
  //public:
    //Font font;
    //virtual void drawImpl(IGUIDrawer& drawer) const;
    //FormattedText();
    //void make(int x = 0, int y = 0, const std::string& text = "", const Font& font = FONT_Default);
    //void setText(const std::string& text);
    //const std::string& getText() const { return text; }
  //private:
    //std::string text;
//};

} //namespace gui
} //namespace lpi

#endif
