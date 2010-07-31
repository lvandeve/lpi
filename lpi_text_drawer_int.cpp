/*
Copyright (c) 2005-2010 Lode Vandevenne
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

#include "lpi_text_drawer_int.h"

#include "lpi_draw2d.h"
#include "lodepng.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
//DATA//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

InternalTextDrawer::InternalTextDrawer(const ITextureFactory& factory, IDrawer2D* drawer)
: drawer(drawer)
, glyphs(&factory, false)
{
}

void InternalTextDrawer::drawText(const std::string& text, int x, int y, const Font& font, const TextAlign& align)
{
  if(align.halign != HA_LEFT || align.valign != VA_TOP) 
  {
    int w, h;
    calcTextRectSize(w, h, text, font);
    if(align.halign == HA_CENTER) x -= w / 2;
    else if(align.halign == HA_RIGHT) x -= w;
    if(align.valign == VA_CENTER) y -= h / 2;
    else if(align.valign == VA_BOTTOM) y -= h;
  }
  
  print(text, x, y, font);
}

void InternalTextDrawer::drawLetter(unsigned char n, int x, int y, const InternalGlyphs::Glyphs* glyphs, const Font& font)
{
  //int italic = 0; //todo: this doesn't work anymore, no function to draw skewed texture available currently!!
  
  //draw the background, using the "completely filled" letter: ascii char 219
  /*if(font->background)
  {
    drawer->drawTexture(glyphs->texture[219], x, y, font.backgroundColor);
  }*/
  if(font.shadow)
  {
    drawer->drawTexture(glyphs->texture[n], x + 1, y + 1, font.shadowColor);
  }
  
  drawer->drawTexture(glyphs->texture[n], x, y, font.color);
  
  if(font.bold) //bold
  {
    drawer->drawTexture(glyphs->texture[n], x + 1, y, font.color);
  }
}

const InternalGlyphs::Glyphs* InternalTextDrawer::getGlyphsForFont(const Font& font) const
{
  if(font.typeface == "lpi8") return &glyphs.glyphs8x8;
  else if(font.typeface == "lpi7") return &glyphs.glyphs7x9;
  else if(font.typeface == "lpi6") return &glyphs.glyphs6x6;
  else if(font.typeface == "lpi4") return &glyphs.glyphs4x5;
  else return &glyphs.glyphs8x8;
}

//Draws a string of text, and uses some of the ascii control characters, e.g. newline
//Other control characters (ascii value < 32) are ignored and have no effect.
void InternalTextDrawer::printText(const std::string& text, int x, int y, const Font& font, unsigned long forceLength)
{
  const InternalGlyphs::Glyphs* glyphs = getGlyphsForFont(font);
  unsigned long pos = 0;
  int drawX = x;
  int drawY = y;
  int symbol;
  while((pos < text.length() && forceLength == 0) || (pos < forceLength && pos < text.length() && forceLength > 0))
  {
     symbol = text[pos];
     if(symbol > 31 || symbol < 0) //it's a signed char, below 0 are the ones above 128
     {
       drawLetter(text[pos], drawX, drawY, glyphs, font);
       drawX += glyphs->width;
     }
     else
     {
       switch(symbol)
       {
         case 10: //newline
           drawX = x;
           drawY += glyphs->height;
           break;
         default: break;
       }
     }
     pos++;
  }
}



//int InternalTextDrawer::printFormattedM(const std::string& text, int x, int y, Font* font, const Markup& originalMarkup)
//{
  //unsigned long pos = 0;
  //int drawX = x;
  //int drawY = y;
  //unsigned char symbol;
  //while(pos < text.length())
  //{
     //symbol = text[pos];
     //if(symbol == '#')
     //{
       //if(pos + 1 >= text.length()) return getScreenHeight() * drawX + drawY;
       //pos++;
       
       //symbol = text[pos];
       //if(symbol == '#')
       //{
         //drawLetter('#', drawX, drawY, markup);
         //drawX += markup.getWidth();
       //}
       //else if(symbol == '\'')
       //{
         //drawLetter('"', drawX, drawY, markup);
         //drawX += markup.getWidth();
       //}
       //else if(symbol == 'n')
       //{
         //drawX = x;
         //drawY += markup.getHeight();
       //}
       //else if(symbol == 'c')
       //{
         //if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         //pos++;
         //int r1, r2, g1, g2, b1, b2, a1, a2;
         //r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         //r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         //g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         //g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         //b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         //b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         //a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         //a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         //markup.color1.r = 16 * r1 + r2;
         //markup.color1.g = 16 * g1 + g2;
         //markup.color1.b = 16 * b1 + b2;
         //markup.color1.a = 16 * a1 + a2;
       //}
       //else if(symbol == 'b')
       //{
         //if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         //pos++;
         //int r1, r2, g1, g2, b1, b2, a1, a2;
         //r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         //r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         //g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         //g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         //b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         //b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         //a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         //a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         //markup.color2.r = 16 * r1 + r2;
         //markup.color2.g = 16 * g1 + g2;
         //markup.color2.b = 16 * b1 + b2;
         //markup.color2.a = 16 * a1 + a2;
       //}
       //else if(symbol == '!') //reset markup to the given one
       //{
         //markup = originalMarkup;
       //}
       //else if(symbol == '?') //draw any of the 256 glyphs, which one is given by 2 hex digits
       //{
         //if(pos + 8 >= text.length()) return getScreenHeight() * drawX + drawY;
         //pos++;
         //int g1, g2;
         //g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         //g2 = text[pos] - 48; if(g2 > 9) g2 -= 7;
         
         //drawLetter(char(16 * g1 + g2), drawX, drawY, markup);
         //drawX += markup.getWidth();
       //}
     //}
     //else
     //{
       //drawLetter(text[pos], drawX, drawY, markup);
       //drawX += markup.getWidth();
     //}
     //pos++;
  //}  

  //return getScreenHeight() * drawX + drawY;
//}

//int InternalTextDrawer::printFormatted(const std::string& text, int x, int y, const Font& font)
//{
  //Markup markup_ = markup;
  
  //return printFormattedM(text, x, y, markup_, markup);
//}

//int InternalTextDrawer::getFormattedTextAttrSize(char c)
//{
  //switch(c)
  //{
    //case '#': return 0; break;
    //case '\'': return 0; break;
    //case 'n': return 0; break;
    //case 'c': return 8; break;
    //case 'b': return 8; break;
    //case '!': return 0; break;
    //case '?': return 2; break;
    //default: return 0; break;
  //}
//}

//int InternalTextDrawer::getFormattedTextSymbolPrintSize(char c)
//{
  //switch(c)
  //{
    //case '#': return 1; break;
    //case '\'': return 1; break;
    //case 'n': return 0; break;
    //case 'c': return 0; break;
    //case 'b': return 0; break;
    //case '!': return 0; break;
    //case '?': return 1; break;
    //default: return 0; break;
  //}
//}

//void InternalTextDrawer::getFormattedTextSize(const std::string& text, int& xsize, int& ysize, const Font& font_)
//{
  //xsize = 0;
  //ysize = markup_.getHeight();
  //int currentX = 0, currentY = markup_.getHeight();
  //unsigned long pos = 0;
  //Markup markup = markup_;
  //unsigned char symbol;
  //while(pos < text.length())
  //{
     //symbol = text[pos];
     //if(symbol == '#')
     //{
       //if(pos + 1 >= text.length()) return;
       //pos++;
       
       //symbol = text[pos];
       //if(symbol == 'n')
       //{
         //currentX = 0;
         //currentY += markup.getHeight();
         //if(currentY > ysize) ysize = currentY;
       //}
       //else if(symbol == '!') //reset markup to the given one
       //{
         //markup = markup_;
       //}
       //else
       //{
         //pos += getFormattedTextAttrSize(symbol);
         
         //currentX += markup.getWidth() * getFormattedTextSymbolPrintSize(symbol);
         //if(currentX > xsize) xsize = currentX;
       //}
     //}
     //else
     //{
      //currentX += markup.getWidth();
      //if(currentX > xsize) xsize = currentX;
     //}
     //pos++;
  //}
//}

//void InternalTextDrawer::getFormattedMarkup(const std::string& text, Font* font, const Markup& originalMarkup) //this one calculates which markup you'll get after this text
//{
  //unsigned long pos = 0;
  //unsigned char symbol;
  //while(pos < text.length())
  //{
     //symbol = text[pos];
     //if(symbol == '#')
     //{
       //if(pos + 1 >= text.length()) return;
       //pos++;
       
       //symbol = text[pos];

       //if(symbol == 'c')
       //{
         //if(pos + 8 >= text.length()) return;
         //pos++;
         //int r1, r2, g1, g2, b1, b2, a1, a2;
         //r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         //r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         //g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         //g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         //b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         //b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         //a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         //a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         //markup.color1.r = 16 * r1 + r2;
         //markup.color1.g = 16 * g1 + g2;
         //markup.color1.b = 16 * b1 + b2;
         //markup.color1.a = 16 * a1 + a2;
       //}
       //else if(symbol == 'b')
       //{
         //if(pos + 8 >= text.length()) return;
         //pos++;
         //int r1, r2, g1, g2, b1, b2, a1, a2;
         //r1 = text[pos] - 48; if(r1 > 9) r1 -= 7; pos++;
         //r2 = text[pos] - 48; if(r2 > 9) r2 -= 7; pos++;
         //g1 = text[pos] - 48; if(g1 > 9) g1 -= 7; pos++;
         //g2 = text[pos] - 48; if(g2 > 9) g2 -= 7; pos++;
         //b1 = text[pos] - 48; if(b1 > 9) b1 -= 7; pos++;
         //b2 = text[pos] - 48; if(b2 > 9) b2 -= 7; pos++;
         //a1 = text[pos] - 48; if(a1 > 9) a1 -= 7; pos++;
         //a2 = text[pos] - 48; if(a2 > 9) a2 -= 7; 
         //markup.color2.r = 16 * r1 + r2;
         //markup.color2.g = 16 * g1 + g2;
         //markup.color2.b = 16 * b1 + b2;
         //markup.color2.a = 16 * a1 + a2;
       //}
       //else if(symbol == '!') //reset markup to the given one
       //{
         //markup = originalMarkup;
       //}

     //}
     //pos++;
  //}
//}

void InternalTextDrawer::calcTextRectSize(int& w, int& h, const std::string& text, const Font& font) const
{
  const InternalGlyphs::Glyphs* glyphs = getGlyphsForFont(font);
  //w = glyphs->width * text.size();
  //h = glyphs->height;
  
  int numlines = 1;
  int linelength = 0;
  int longestline = 0;
  for(size_t i = 0; i < text.size(); i++)
  {
    if(text[i] == 10 || (text[i] == 13 && (i == 0 || text[i - 1] != 10)))
    {
      numlines++;
      if(linelength > longestline) longestline = linelength;
      linelength = 0;
    }
    else linelength++;
  }
  if(linelength > longestline) longestline = linelength;
  w = glyphs->width * longestline;
  h = glyphs->height * numlines;
}



size_t InternalTextDrawer::calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align) const
{
  (void)x;(void)y;(void)font;(void)text;(void)align;
  //TODO!
  return 0;
}

void InternalTextDrawer::calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align) const
{
  (void)x;(void)y;(void)font;(void)text;(void)align;(void)index;
  //TODO!
}

InternalGlyphs::InternalGlyphs(const ITextureFactory* factory, bool allInOneBigTexture)
{
  initBuiltInFontTextures(factory, allInOneBigTexture);
}

InternalGlyphs::Glyphs::~Glyphs()
{
  for(size_t i = 0; i < texture.size(); i++) delete texture[i];
}

InternalGlyphs::~InternalGlyphs()
{
}

const std::string& getBuiltIn8x8FontTexture()
{
  /*
  A PNG image, encoded in base64, containing the full extended ASCII character
  set, in the form 256 bitmap symbols of 8x8 pixels in one 128x128 PNG.
  First use the function decodeBase64
  Then use the function LodePNG::decodePNG32
  And you get the pixels in a buffer
  NOTE: the background color is black, not transparent, so may need conversion
  */
  static const std::string font8x8string = "\
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
  return font8x8string;
}

const std::string& getBuiltIn7x9FontTexture()
{
  /*
  A PNG image, encoded in base64, containing a partial font (pure ASCII only)
  of 7x9 pixels
  First use the function decodeBase64
  Then use the function LodePNG::decodePNG32
  And you get the pixels in a buffer
  NOTE: the background color is black, not transparent, so may need conversion
  */
  static const std::string font7x9string = "\
iVBORw0KGgoAAAANSUhEUgAAAHAAAACQAQMAAADqan2KAAAABlBMVEX///8AAABVwtN+AAACW0lE\n\
QVRIie3UP2gTURwH8CcW0i3tVjDNlSyugoIpTc3i7OxQaqFQ8U9iFi8tIXeFQuuUQt2s2tFBbIWC\n\
qV6SEzpEJ6cQS/rytOciNb3Ykj7l5e7X37uLECqIW5fc8vjw3v1+v/c9OHKuHWgq7/XOQs6K7vlf\n\
wS/x+c5yZgR2a5ID56apAzjIycK9Oc5LQaRATlw45txmZoduweFgswmlqXNJM3MHCbiLhPbQ8asO\n\
bblbhBRwEzT9CAlOFkCyDcCQ3Q+xPi0rhdiUVZY6ItRW7XFxQA08yhtI+6DIXWqsrUVHHSR7Vkg5\n\
1jaWcjNI84eSz1IBuCuQmm3kM5RDnHGsvBBQQrPCWl7TZeVTfU+x/DQ80Jz6Ohjof7TXN02MeqOW\n\
UanBKD2coWRzvSHetRiyKPlyfVhs/WQLbDfbuovMyd3PRp0W88hNFd9NUyTN36ekNBK+1Jy2BgPR\n\
D5E+i2z/c4ylXPhmpK8/GgmO/RZAaunEk3ySGqqddVoeXZ6sVDdY1jmQTF7jiQp+I82RV0grdf6w\n\
UmW6R5ZOSBp12+Ny7kps9mh/NBfUs7Jv+U/P+NzpMeYJwy8Vb2OMb9tyKtsFMAWY4LjezLnnoa3m\n\
txfh4fDeR4uAmqhdzeysMlpqYTiw2BCvMxjdrtZKItWGyAuZZMkjCYeGUpjzztAI5uw3dMCNd6Vh\n\
/R1OSfgUPr0bXVy5MTBzvX8lEvS4entc8CStqkxzDoEYaW1MhrNR0trfJfXaGwxn44F3eGkxFJu7\n\
vP+47r9rcr8u8ysrnQkUv2/vv9HNXhq9NHpp/AdPALuxukSDNvmKAAAAAElFTkSuQmCC\
";
  return font7x9string;

}

const std::string& getBuiltIn6x6FontTexture()
{
  /*
  A PNG image, encoded in base64, containing the full extended ASCII character
  set, in the form 256 bitmap symbols of 6x6 pixels in one 96x96 PNG.
  First use the function decodeBase64
  Then use the function LodePNG::decodePNG32
  And you get the pixels in a buffer
  NOTE: the background color is black, not transparent, so may need conversion
  */
  static const std::string font6x6string = "\
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
  return font6x6string;

}

const std::string& getBuiltIn4x5FontTexture()
{
  /*
  A PNG image, encoded in base64, containing printable part of ASCII character
  set, in the form 256 bitmap symbols of 4x5 pixels in one 64x80 PNG.
  First use the function decodeBase64
  Then use the function LodePNG::decodePNG32
  And you get the pixels in a buffer
  NOTE: the background color is black, not transparent, so may need conversion
  */
static const std::string font4x5string = "\
iVBORw0KGgoAAAANSUhEUgAAAEAAAABQAQMAAACTceEGAAAABlBMVEUAAAD///+l2Z/dAAAAwklE\n\
QVQokWNgoBlgWcDJpMLAwARkMLE4LWFgAYkxOPmpsQApTgaVJQ4eCMVO59z0nriouTEsU2txWsXg\n\
oMSwyOnRkhAXNQcGp3cuLhUsQHPgwKXt3LMlWm0pDKtOrOh4otW9iuHcijWnlqzpXMXw6ti5tiWn\n\
Xi5BKD527NXKVc/cHBhWrfFa6aKitAAo4rXaxUWJgaFpjZ/bimdi/FhM1oKZzAczmQuXySFQk4HO\n\
Bpvs48wFNdmNj2aBO0gBPykMWgAArlFWRoV1udAAAAAASUVORK5CYII=\
";
  return font4x5string;
}


void InternalGlyphs::initBuiltInFontTextures(const ITextureFactory* factory, bool allInOneBigTexture)
{
  AlphaEffect AE_BlackKey(128, 255, lpi::RGB_Black);

  glyphs8x8.width = 8;
  glyphs8x8.height = 8;
  glyphs7x9.width = 7;
  glyphs7x9.height = 9;
  glyphs6x6.width = 6;
  glyphs6x6.height = 6;
  glyphs4x5.width = 4;
  glyphs4x5.height = 5;

  if(allInOneBigTexture)
  {
    loadTexturesFromBase64PNG(glyphs8x8.texture, factory, getBuiltIn8x8FontTexture(), 8*16, 8*16, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs7x9.texture, factory, getBuiltIn7x9FontTexture(), 7*16, 9*16, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs6x6.texture, factory, getBuiltIn6x6FontTexture(), 6*16, 6*16, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs4x5.texture, factory, getBuiltIn4x5FontTexture(), 4*16, 5*16, AE_BlackKey);
  }
  else
  {
    loadTexturesFromBase64PNG(glyphs8x8.texture, factory, getBuiltIn8x8FontTexture(), 8, 8, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs7x9.texture, factory, getBuiltIn7x9FontTexture(), 7, 9, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs6x6.texture, factory, getBuiltIn6x6FontTexture(), 6, 6, AE_BlackKey);
    loadTexturesFromBase64PNG(glyphs4x5.texture, factory, getBuiltIn4x5FontTexture(), 4, 5, AE_BlackKey);
  }
}


} //namespace lpi
