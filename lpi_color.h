/*
Copyright (c) 2005-2007 Lode Vandevenne
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

#ifndef LPI_COLOR_H_INCLUDED
#define LPI_COLOR_H_INCLUDED

namespace lpi
{

struct ColorRGB
{
  int r;
  int g;
  int b;
  int a;
  
  ColorRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  ColorRGB(unsigned char r, unsigned char g, unsigned char b);
  ColorRGB();
};

//the + and - operator add/subtract the R G and B channels but not the alpha channel
ColorRGB operator+(const ColorRGB& color, const ColorRGB& color2);
ColorRGB operator-(const ColorRGB& color, const ColorRGB& color2);
//these multiplication operators with a constant don't affect the alpha channel
ColorRGB operator*(const ColorRGB& color, int a);
ColorRGB operator*(int a, const ColorRGB& color);
ColorRGB operator*(const ColorRGB& color, double a);
ColorRGB operator*(double a, const ColorRGB& color);
ColorRGB operator/(const ColorRGB& color, int a);
//this multiplies two colors, and the alpha channels too
ColorRGB operator*(const ColorRGB& color1, const ColorRGB& color2);
//The | operator adds two color including their alpha channel
ColorRGB operator|(const ColorRGB& color, const ColorRGB& color2);
//The & operator multiplies a color and also its alpha channel with a constant
ColorRGB operator&(const ColorRGB& color, int a);
ColorRGB operator&(int a, const ColorRGB& color);
ColorRGB operator&(const ColorRGB& color, double a);
ColorRGB operator&(double a, const ColorRGB& color);

bool operator==(const ColorRGB& color, const ColorRGB& color2);
bool operator!=(const ColorRGB& color, const ColorRGB& color2);

static const ColorRGB RGB_Black        (  0,   0,   0, 255);
static const ColorRGB RGB_Red          (255,   0,   0, 255);
static const ColorRGB RGB_Green        (  0, 255,   0, 255);
static const ColorRGB RGB_Blue         (  0,   0, 255, 255);
static const ColorRGB RGB_Cyan         (  0, 255, 255, 255);
static const ColorRGB RGB_Magenta      (255,   0, 255, 255);
static const ColorRGB RGB_Yellow       (255, 255,   0, 255);
static const ColorRGB RGB_White        (255, 255, 255, 255);
static const ColorRGB RGB_Gray         (128, 128, 128, 255);
static const ColorRGB RGB_Grey         (192, 192, 192, 255);
static const ColorRGB RGB_Darkred      (128,   0,   0, 255);
static const ColorRGB RGB_Darkgreen    (  0, 128,   0, 255);
static const ColorRGB RGB_Darkblue     (  0,   0, 128, 255);
static const ColorRGB RGB_Darkcyan     (  0, 128, 128, 255);
static const ColorRGB RGB_Darkmagenta  (128,   0, 128, 255);
static const ColorRGB RGB_Darkyellow   (128, 128,   0, 255);
static const ColorRGB RGB_Darkgray     ( 64,  64,  64, 255);
static const ColorRGB RGB_Darkgrey     ( 96,  96,  96, 255);
static const ColorRGB RGB_Lightred     (255, 128, 128, 255);
static const ColorRGB RGB_Lightgreen   (128, 255, 128, 255);
static const ColorRGB RGB_Lightblue    (128, 128, 255, 255);
static const ColorRGB RGB_Lightcyan    (128, 255, 255, 255);
static const ColorRGB RGB_Lightmagenta (255, 128, 255, 255);
static const ColorRGB RGB_Lightyellow  (255, 255, 128, 255);
static const ColorRGB RGB_Lightgray    (224, 224, 224, 255);
static const ColorRGB RGB_Lightgrey    (240, 240, 240, 255);
static const ColorRGB RGB_Brightred    (255, 192, 192, 255);
static const ColorRGB RGB_Brightgreen  (192, 255, 192, 255);
static const ColorRGB RGB_Brightblue   (192, 192, 255, 255);
static const ColorRGB RGB_Brightcyan   (192, 255, 255, 255);
static const ColorRGB RGB_Brightmagenta(255, 192, 255, 255);
static const ColorRGB RGB_Brightyellow (255, 255, 192, 255);
static const ColorRGB RGB_Whitered     (255, 224, 224, 255);
static const ColorRGB RGB_Whitegreen   (224, 255, 224, 255);
static const ColorRGB RGB_Whiteblue    (224, 224, 255, 255);
static const ColorRGB RGB_Whitecyan    (224, 255, 255, 255);
static const ColorRGB RGB_Whitemagenta (255, 224, 255, 255);
static const ColorRGB RGB_Whiteyellow  (255, 255, 224, 255);
static const ColorRGB RGB_Orange       (255, 165,   0, 255);

static const ColorRGB RGB_Alpha(255, 255, 255, 192);
static const ColorRGB RGB_Translucent(255, 255, 255, 128);
static const ColorRGB RGB_Ghost(255, 255, 255, 64);
static const ColorRGB RGB_Invisible(  0,   0,   0,   0);

#define RGBA_Black(a)        ColorRGB(  0,   0,   0, a)
#define RGBA_Red(a)          ColorRGB(255,   0,   0, a)
#define RGBA_Green(a)        ColorRGB(  0, 255,   0, a)
#define RGBA_Blue(a)         ColorRGB(  0,   0, 255, a)
#define RGBA_Cyan(a)         ColorRGB(  0, 255, 255, a)
#define RGBA_Magenta(a)      ColorRGB(255,   0, 255, a)
#define RGBA_Yellow(a)       ColorRGB(255, 255,   0, a)
#define RGBA_White(a)        ColorRGB(255, 255, 255, a)
#define RGBA_Gray(a)         ColorRGB(128, 128, 128, a)
#define RGBA_Grey(a)         ColorRGB(192, 192, 192, a)
#define RGBA_Darkred(a)      ColorRGB(128,   0,   0, a)
#define RGBA_Darkgreen(a)    ColorRGB(  0, 128,   0, a)
#define RGBA_Darkblue(a)     ColorRGB(  0,   0, 128, a)
#define RGBA_Darkcyan(a)     ColorRGB(  0, 128, 128, a)
#define RGBA_Darkmagenta(a)  ColorRGB(128,   0, 128, a)
#define RGBA_Darkyellow(a)   ColorRGB(128, 128,   0, a)
#define RGBA_Darkgray(a)     ColorRGB( 64,  64,  64, a)
#define RGBA_Darkgrey(a)     ColorRGB( 96,  96,  96, a)
#define RGBA_Lightred(a)     ColorRGB(255, 128, 128, a)
#define RGBA_Lightgreen(a)   ColorRGB(128, 255, 128, a)
#define RGBA_Lightblue(a)    ColorRGB(128, 128, 255, a)
#define RGBA_Lightcyan(a)    ColorRGB(128, 255, 255, a)
#define RGBA_Lightmagenta(a )ColorRGB(255, 128, 255, a)
#define RGBA_Lightyellow(a)  ColorRGB(255, 255, 128, a)
#define RGBA_Brightred(a)    ColorRGB(255, 192, 192, a)
#define RGBA_Brightgreen(a)  ColorRGB(192, 255, 192, a)
#define RGBA_Brightblue(a)   ColorRGB(192, 192, 255, a)
#define RGBA_Brightcyan(a)   ColorRGB(192, 255, 255, a)
#define RGBA_Brightmagenta(a)ColorRGB(255, 192, 255, a)
#define RGBA_Brightyellow(a) ColorRGB(255, 255, 192, a)
#define RGBA_Orange(a)       ColorRGB(255, 165,   0, a)

//a color with 3 components: h, s and l
struct ColorHSL
{
  int h;
  int s;
  int l;
  
  ColorHSL(unsigned char h, unsigned char s, unsigned char l);
  ColorHSL();
}; 
   
//a color with 3 components: h, s and v
struct ColorHSV
{
  int h;
  int s;
  int v;
  
  ColorHSV(unsigned char h, unsigned char s, unsigned char v);
  ColorHSV();
};

ColorHSL RGBtoHSL(const ColorRGB& colorRGB);
ColorRGB HSLtoRGB(const ColorHSL& colorHSL);
ColorHSV RGBtoHSV(const ColorRGB& colorRGB);
ColorRGB HSVtoRGB(const ColorHSV& colorHSV);
unsigned long RGBtoINT(const ColorRGB& colorRGB);
ColorRGB INTtoRGB(unsigned long colorINT);

}

#endif
