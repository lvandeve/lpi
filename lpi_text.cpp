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

#include "lpi_text.h"
#include "lodepng.h"


namespace lpi
{

namespace
{
  int getScreenHeight()
  {
    GLint array[4];
    glGetIntegerv(GL_VIEWPORT, array); //array[3] contains the height in pixels of the viewport
    return array[3];
  }
}

std::vector<Texture> builtInFontTexture8x8;
std::vector<Texture> builtInFontTexture6x6;
std::vector<Texture> builtInFontTexture4x5;

int Font::getWidth()
{
  if(texture.size() > 0) return texture[0].getU();
  else return 0;
}

int Font::getHeight()
{
  if(texture.size() > 0) return texture[0].getV();
  else return 0;
}

//Markup struct constructor
Markup::Markup(const ColorRGB& color1, const ColorRGB& color2, int style, Font* font, int charSpacing, int lineSpacing)
{
  this->color1 = color1;
  this->color2 = color2;
  this->style = style;
  this->font = font;
  this->charSpacing = charSpacing;
  this->lineSpacing = lineSpacing;
}

Markup operator/(Markup ts, int a)
{
  ts.color1 = ts.color1 / a;
  ts.color2 = ts.color2 / a;
  
  return ts;
}

int Markup::getWidth() const
{
  return font->getWidth() + charSpacing;
}

int Markup::getHeight() const
{
  return font->getHeight() + lineSpacing;
}

std::vector<Font> externFont; //loading up textures for the other fonts will be done in the textures.cpp file
Font builtInFont8x8;
Font builtInFont6x6;
Font builtInFont4x5;

//****************************************************************************//
////////////////////////////////////////////////////////////////////////////////
//BASIC PRINT FUNCTIONS/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//****************************************************************************//


/*
drawLetter draws a single letter at asked position, and is used by most print 
functions to draw the characters.

Parameters:
n: extended ascii code of the character to be drawn, for example code 65 is capital A
x: x position of top left corner of the letter
y: y position of top left corner of the letter
markup: the text style, see in the header near Markup for an explanation
*/

void drawLetter(unsigned char n, int x, int y, const Markup& markup)
{
  Font* FONT = markup.font;

  int shadowx = 0, shadowy = 0;
  if(markup.style & 2) shadowx++;
  if(markup.style & 4) shadowx--;
  if(markup.style & 8) shadowy--;
  if(markup.style & 16) shadowy++;
  if(markup.style & 32) shadowx += 2;
  if(markup.style & 64) shadowx -= 2;
  if(markup.style & 128) shadowy -= 2;
  if(markup.style & 256) shadowy += 2;
  
  int italic = 0;
  if(markup.style & 1024) italic = 1;
  
  //draw the background, using the "completely filled" letter: ascii char 219
  if(markup.style & 1)
  {
    (FONT->texture)[219].draw(x, y, markup.color2, -1, -1, italic);
  }
  if(markup.style > 1)
  {
    (FONT->texture)[n].draw(x + shadowx, y + shadowy, markup.color2, -1, -1, italic);
  }
  
  (FONT->texture)[n].draw(x, y, markup.color1, -1, -1, italic);
  
  if(markup.style & 512) //bold
  {
    (FONT->texture)[n].draw(x + 1, y, markup.color1, -1, -1, italic);
  }
}

//Draws a string of text, null terminated
int printString(std::string text, int x, int y, const Markup& markup, unsigned long forceLength)
{
  unsigned long pos = 0;
  while((pos < text.length() && forceLength == 0 /*&& text[pos] != 0*/) || (pos < forceLength && pos < text.length()  /*&& text[pos] != 0*/ && forceLength > 0))
  {
     drawLetter(text[pos], x, y, markup);
     pos++;
     x += markup.getWidth();
  }
  //return position of a next character, where you may want to start drawing another string
  return getScreenHeight() * x + y;
}

//Draws a string of text, and uses some of the ascii control characters, e.g. newline
//Other control characters (ascii value < 32) are ignored and have no effect.
int printText(std::string text, int x, int y, const Markup& markup, unsigned long forceLength)
{
  unsigned long pos = 0;
  int drawX = x;
  int drawY = y;
  int symbol;
  while((pos < text.length() && forceLength == 0) || (pos < forceLength && pos < text.length() && forceLength > 0))
  {
     symbol = text[pos];
     if(symbol > 31 || symbol < 0) //it's a signed char, below 0 are the ones above 128
     {
       drawLetter(text[pos], drawX, drawY, markup);
       drawX += markup.getWidth();
     }
     else
     {
       switch(symbol)
       {
         case 10: //newline
           drawX = x;
           drawY += markup.getHeight();
           break;
         default: break;
       }
     }
     pos++;
  }  
  //return pos;
  return getScreenHeight() * drawX + drawY;
}

///////////////////////////////////////////////////////////////////////////////
//FORMATTED TEXT///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int printFormattedM(std::string text, int x, int y, Markup& markup, const Markup& originalMarkup)
{
  unsigned long pos = 0;
  int drawX = x;
  int drawY = y;
  unsigned char symbol;
  while(pos < text.length())
  {
     symbol = text[pos];
     if(symbol == '#')
     {
       if(pos + 1 >= text.length()) return getScreenHeight() * drawX + drawY;
       pos++;
       
       symbol = text[pos];
       if(symbol == '#')
       {
         drawLetter('#', drawX, drawY, markup);
         drawX += markup.getWidth();
       }
       else if(symbol == '\'')
       {
         drawLetter('"', drawX, drawY, markup);
         drawX += markup.getWidth();
       }
       else if(symbol == 'n')
       {
         drawX = x;
         drawY += markup.getHeight();
       }
       else if(symbol == 'c')
       {
         if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         pos++;
         int r1, r2, g1, g2, b1, b2, a1, a2;
         r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         markup.color1.r = 16 * r1 + r2;
         markup.color1.g = 16 * g1 + g2;
         markup.color1.b = 16 * b1 + b2;
         markup.color1.a = 16 * a1 + a2;
       }
       else if(symbol == 'b')
       {
         if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         pos++;
         int r1, r2, g1, g2, b1, b2, a1, a2;
         r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         markup.color2.r = 16 * r1 + r2;
         markup.color2.g = 16 * g1 + g2;
         markup.color2.b = 16 * b1 + b2;
         markup.color2.a = 16 * a1 + a2;
       }
       else if(symbol == '!') //reset markup to the given one
       {
         markup = originalMarkup;
       }
       else if(symbol == '?') //draw any of the 256 glyphs, which one is given by 2 hex digits
       {
         if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         pos++;
         int g1, g2;
         g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         g2 = text[pos] - 48; if(g2 > 9) g2 -= 7;
         
         drawLetter(char(16 * g1 + g2), drawX, drawY, markup);
         drawX += markup.getWidth();
       }
     }
     else
     {
       drawLetter(text[pos], drawX, drawY, markup);
       drawX += markup.getWidth();
     }
     pos++;
  }  

  return getScreenHeight() * drawX + drawY;
}

int printFormatted(std::string text, int x, int y, const Markup& markup)
{
  Markup markup_ = markup;
  
  return printFormattedM(text, x, y, markup_, markup);
}

int getFormattedTextAttrSize(char c)
{
  switch(c)
  {
    case '#': return 0; break;
    case '\'': return 0; break;
    case 'n': return 0; break;
    case 'c': return 8; break;
    case 'b': return 8; break;
    case '!': return 0; break;
    case '?': return 2; break;
    default: return 0; break;
  }
}

int getFormattedTextSymbolPrintSize(char c)
{
  switch(c)
  {
    case '#': return 1; break;
    case '\'': return 1; break;
    case 'n': return 0; break;
    case 'c': return 0; break;
    case 'b': return 0; break;
    case '!': return 0; break;
    case '?': return 1; break;
    default: return 0; break;
  }
}

void getFormattedTextSize(std::string text, int& xsize, int& ysize, const Markup& markup_)
{
  xsize = 0;
  ysize = markup_.getHeight();
  int currentX = 0, currentY = markup_.getHeight();
  unsigned long pos = 0;
  Markup markup = markup_;
  unsigned char symbol;
  while(pos < text.length())
  {
     symbol = text[pos];
     if(symbol == '#')
     {
       if(pos + 1 >= text.length()) return;
       pos++;
       
       symbol = text[pos];
       if(symbol == 'n')
       {
         currentX = 0;
         currentY += markup.getHeight();
         if(currentY > ysize) ysize = currentY;
       }
       else if(symbol == '!') //reset markup to the given one
       {
         markup = markup_;
       }
       else
       {
         pos += getFormattedTextAttrSize(symbol);
         
         currentX += markup.getWidth() * getFormattedTextSymbolPrintSize(symbol);
         if(currentX > xsize) xsize = currentX;
       }
     }
     else
     {
      currentX += markup.getWidth();
      if(currentX > xsize) xsize = currentX;
     }
     pos++;
  }
}

void getFormattedMarkup(std::string text, Markup& markup, const Markup& originalMarkup) //this one calculates which markup you'll get after this text
{
  unsigned long pos = 0;
  unsigned char symbol;
  while(pos < text.length())
  {
     symbol = text[pos];
     if(symbol == '#')
     {
       if(pos + 1 >= text.length()) return;
       pos++;
       
       symbol = text[pos];

       if(symbol == 'c')
       {
         if(pos + 8 >= text.length()) return;
         pos++;
         int r1, r2, g1, g2, b1, b2, a1, a2;
         r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         markup.color1.r = 16 * r1 + r2;
         markup.color1.g = 16 * g1 + g2;
         markup.color1.b = 16 * b1 + b2;
         markup.color1.a = 16 * a1 + a2;
       }
       else if(symbol == 'b')
       {
         if(pos + 8 >= text.length()) return;
         pos++;
         int r1, r2, g1, g2, b1, b2, a1, a2;
         r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         markup.color2.r = 16 * r1 + r2;
         markup.color2.g = 16 * g1 + g2;
         markup.color2.b = 16 * b1 + b2;
         markup.color2.a = 16 * a1 + a2;
       }
       else if(symbol == '!') //reset markup to the given one
       {
         markup = originalMarkup;
       }

     }
     pos++;
  }
}

////////////////////////////////////////////////////////////////////////////////
//ADVANCED TEXT DRAWING FUNCTIONS///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void printCentered(std::string text, int x, int y, const Markup& markup, unsigned long forceLength)
{
  int pos = 0;
  while(text[pos] != 0) pos++;
  
  x -= pos * markup.getWidth() / 2;
  y -= markup.getHeight() / 2;
  
  print(text, x, y, markup, forceLength);
}

namespace //empty namespace
{
  LodePNG::Decoder pngdec;
}

void initBuiltInFontTextures()
{
  loadTexturesFromBase64PNG(builtInFont8x8.texture, font8x8string, 8, 8, AE_BlackKey);
  loadTexturesFromBase64PNG(builtInFont6x6.texture, font6x6string, 6, 6, AE_BlackKey);
  loadTexturesFromBase64PNG(builtInFont4x5.texture, font4x5string, 4, 5, AE_BlackKey);
}


////////////////////////////////////////////////////////////////////////////////
//DATA//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//NOTE: calling this all may be done by lpi_texture.cpp, in the initTextures() function

/*
A PNG image, encoded in base64, containing the full extended ASCII character
set, in the form 256 bitmap symbols of 8x8 pixels in one 128x128 PNG.
First use the function decodeBase64
Then use the function LodePNG::decodePNG32
And you get the pixels in a buffer
NOTE: the background color is black, not transparent, so may need conversion
*/
const std::string font8x8string = "\
iVBORw0KGgoAAAANSUhEUgAAAIAAAACAAQMAAAD58POIAAAABlBMVEUAAAD///+l2Z/dAAAEtklE\n\
QVRIiY1VP2scRxR/OKA0g6RyQIcFwR/ggcN5i0WCfIc0qQbZjF1Mcbi4PMhm5XQp8gEsMKRKkzpl\n\
DAOCIcVD7sTBKeaqcyPMQYK8xTCb9/ZOUowS8Nzs7OzvfvP+zXszAG0bdqtd6KHfqQ+PTwB+6EvV\n\
CFCffu4fPv4G4Jd5aarKvvMnO/7wuB4YpTT16Vevv/f4+B3A69OmKUUBrgfg5F1VnTe6hO3k6bBk\n\
V5oK5Xa1FKFtC1BVqpbsIp7Ai3vWH7dgrYV1W2zXfn4KdW0twgPbQ3fQ+jnJQmtc9HUPpVj/HeuS\n\
UmIvArsD6/dYCK0y+lpktLBHbW1ri+VBb0VL7fd+am+1AMisEzNuGoaAUDnrhq8tAAqhScEhehTA\n\
KFAiV9HZGtfrEQLZKTjbZ8jyJb0YvIANY5AUOs+gMiaAcViCaQqqBQFfwL/bwCckGuUqE6kKR8Ar\n\
5jo6Zh6WMFygMaGLhhnRZWsgY1Wx6Sw1KCqMhUt0phhGFqOFgSBTFr5jixNUQylnMkSOJoiiBe+Y\n\
AQ3mOsveMt1frlKqIJH34ejIMxrv3i8DvGUf/bNnUYDgylUSoImeKGY05M7fCpAHxpmoD0fpLEFk\n\
leFEhkxSCkCsWlYHTLQsquVOyyQ/5pS4UKRdAM+efxsAds5WA7AQ/1Lglw5tENO5qQQ4rwidtUk8\n\
uqCRACXgkTOaAY6CmEDvEya3pcBqtCTKmBIVukd3reglaAsYQQULtIvJkC8SKANBw7WOn8RfYuqm\n\
YUIeLzuRYjw3sn1TsQqL7l/jI2fHHi3huQLiC0fXeGUkBaa/05RWZkmzJSW+saQDmNxas7t+jexi\n\
up5u9g8tXlRwMZ01zRCgLIBY6DnETYDUh6QADQGSDBJGgqZx5kumEpona8CZVWenGJLJogUKrO7f\n\
WjI0glHLCwQZuFmw5BjDqVoigwhAhAhUE1Ejw6SaiCuSwlvGmCg+ixZxTnb/oGma2Ga1VARY1j+5\n\
cTEKkDOYlg7btt1SQTUJYxOR+iYWIzggtQYS24pX21pyLDYtuAIbeLYH+ZjX1IbbJzSTdDdFxIme\n\
xHGV/26BjnndBHDIwth8Z1myzG+tyDiTJW1rKrIZeXajvhsOgu0JjDR3c62Fn9LGtFEHwYM6bh+K\n\
6bqDHPQ0mc3AezBI3F0e1C7ng78sP5SaU50AMZrHT60wtG75om05mrM3tchoxQhNE/H35c+3QRja\n\
/meayV98/aeccmOA8Vi6ID/++u3HwMDoxtBdjcvVuLtl2K3x1tY1o+uuijCuSum6a8ZY+lgz/VrL\n\
LXCHISetDqpUf/8L7I8fHX7oH/Uf+hsG4iaj9/te+6ND+XtgwMCxgwAdPgHQbvWxUsSrnf4/AOn7\n\
+/L6iGHldQ30fX+4vy9mdmLOakeVjPVRJz4ZkFY0RapqBJLPsoelePDT4d4xjmEqFYqtv6CUbNtG\n\
Oei7GCx7yyU183lm+INjQAWklOfzDUPOGCtXlVx/KiPGUMhZrN4TuFoyJariKNcTbOZyJm5LjUu6\n\
7ugdI7cH4p7F53JwmmCUka3bs/BqAKp6zbA2Q2UFcPVahgTouQT6MjTKUIdn+EqLb12cOWcBPi5V\n\
O9lUrLR/ABF/3H2EtBmWAAAAAElFTkSuQmCC\
";

/*
A PNG image, encoded in base64, containing the full extended ASCII character
set, in the form 256 bitmap symbols of 6x6 pixels in one 96x96 PNG.
First use the function decodeBase64
Then use the function LodePNG::decodePNG32
And you get the pixels in a buffer
NOTE: the background color is black, not transparent, so may need conversion
*/
const std::string font6x6string = "\
iVBORw0KGgoAAAANSUhEUgAAAGAAAABgAQMAAADYVuV7AAAABlBMVEUAAAD///+l2Z/dAAADo0lE\n\
QVQ4jW1TTWjbZhh+ZCvtl6DZivex+WCKZCs/7XZwxg4uhETOcCzlMBx62Q5l7f5zs7pLGIwpih05\n\
wympaCGHkEPo2NZLzrsU1zFVCWyjsPu6uKWBHRYGpRtL8N5PbrsF9iHE9+h53+d79b7PBySvax6O\n\
kj86p7chzd12QhwfzM7XrgPJ23//3vzhtc+dsgvp3uaVHQKf6jIxaw3KOT5Y/OylbQpzpZAEWhoA\n\
1+OLh3i27vj823WmLTHJBLoeX3iQqfpb8f0pYnD28aZ+hxXLGZFzrtzQXCwmGyIt3nougDrPzjJd\n\
7GIEpqoWM+sMkOkg3lpxTS8jtQhjamIQEaMK5tdRl3I0WRSE6AUqABN+Zrh7Jt+CKmdgud6EHLM1\n\
6O0YrGVlPpTz+8yUFFi12KLRsAQD5P3N4V2WbzNVZi+qinbVUHHChL2bmjGaWDXWbVm1lvhbHwf4\n\
fky1w1cWl9IzXwf47iExKWuVzxgBnDHF6STsu6krlHNiVcOEY2zbY4pLh9rGum7csnVmlgRo6iPn\n\
qDaN/sEZixGwDKYSMDubWptbocJaL/5TrDMwZTAJpqeKj9pjpdBOVJHKhQokKvSTlOXx7M0gAl7C\n\
8jICQITFLC+VvfmsNgWnnktG4oruNVDdv+oYN8S86hx2Lq0bN1JFFWYWdvaUPjI03Gaam6WwjLo/\n\
NF5u6u4UzChdciOpieLVs3Hos9DlZjQWV6mAV5aBYi5R6K7nd2m+TRRHAmrDux81ND/AxGiK2nB5\n\
hWm1LVzqUtiGCEMK5ukt29+wjcYbr8vI7UUntBR7bgjOjpLvCI1LzQze3yOB4LyxXtxjKP4W2Dll\n\
0GgeLo+jCoUcUdiNvJOWVhwaNahBH5IOMMBBtckBCvFUnnwAll8dJIcE8+VMMcy8UwsINOdNnJdG\n\
NymnEIfwet8i+sV7Wh1SCfMzwFffWLk6uqXJ+29HzIO1Sa0kv9wu9BneLb15mGD9HK6XuJAg5l+g\n\
uQwerwygMtAHyz9fPO69d9zDdO+n3h+vFvsmR0wwX3z5VDACeFwyRdpJQC3X6HGn40s9XDjkk4f8\n\
qA+Okr9o/tP4ck+A+BNMPuFHseneEnWjLi6k9n8gnPoTvOrL+ABYGOH2o3HdajidNK51eLbELhxs\n\
3NoxcM3gc2WmlwynY2Chhb9qrLq2EY8bkE7chRpTPbJcMnJNWx13hypgl/2I0XwqnFX8KDLrDknu\n\
eCWki3E3rce4tJL+r5AYwj/AsDX88LifHQAAAABJRU5ErkJggg==\
";

/*
A PNG image, encoded in base64, containing printable part of ASCII character
set, in the form 256 bitmap symbols of 4x5 pixels in one 64x80 PNG.
First use the function decodeBase64
Then use the function LodePNG::decodePNG32
And you get the pixels in a buffer
NOTE: the background color is black, not transparent, so may need conversion
*/
const std::string font4x5string = "\
iVBORw0KGgoAAAANSUhEUgAAAEAAAABQAQMAAACTceEGAAAABlBMVEUAAAD///+l2Z/dAAAAwklE\n\
QVQokWNgoBlgWcDJpMLAwARkMLE4LWFgAYkxOPmpsQApTgaVJQ4eCMVO59z0nriouTEsU2txWsXg\n\
oMSwyOnRkhAXNQcGp3cuLhUsQHPgwKXt3LMlWm0pDKtOrOh4otW9iuHcijWnlqzpXMXw6ti5tiWn\n\
Xi5BKD527NXKVc/cHBhWrfFa6aKitAAo4rXaxUWJgaFpjZ/bimdi/FhM1oKZzAczmQuXySFQk4HO\n\
Bpvs48wFNdmNj2aBO0gBPykMWgAArlFWRoV1udAAAAAASUVORK5CYII=\
";

} //namespace lpi