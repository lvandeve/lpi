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

#include "lpi_color.h"

#include <cmath>
#include <algorithm> //std::min and std::max
#include <iostream>

namespace lpi
{


////////////////////////////////////////////////////////////////////////////////
//COLOR STRUCTS/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Add two colors
ColorRGB operator+(const ColorRGB& color, const ColorRGB& color2)
{
  ColorRGB c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  return c;
}

//Subtract two colors
ColorRGB operator-(const ColorRGB& color, const ColorRGB& color2)
{
  ColorRGB c;
  c.r = color.r - color2.r;
  c.g = color.g - color2.g;
  c.b = color.b - color2.b;
  return c;
}

//Multiplying with a constant does NOT affect the alpha channel, use the "&" operator on two colors instead if you need that

//Multiplies a color with an integer
ColorRGB operator*(const ColorRGB& color, int a)
{
  ColorRGB c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiplies a color with an integer
ColorRGB operator*(int a, const ColorRGB& color)
{
  ColorRGB c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiplies a color with a double
ColorRGB operator*(const ColorRGB& color, double a)
{
  ColorRGB c;
  c.r = int(color.r * a);
  c.g = int(color.g * a);
  c.b = int(color.b * a);
  c.a = color.a;
  return c;
}

//Multiplies a color with a double
ColorRGB operator*(double a, const ColorRGB& color)
{
  ColorRGB c;
  c.r = int(color.r * a);
  c.g = int(color.g * a);
  c.b = int(color.b * a);
  c.a = color.a;
  return c;
}

//Multiply two colors (component by component)
ColorRGB operator*(const ColorRGB& color1, const ColorRGB& color2)
{
  ColorRGB c;
  c.r = (color1.r * color2.r) / 255;
  c.g = (color1.g * color2.g) / 255;
  c.b = (color1.b * color2.b) / 255;
  c.a = (color1.a * color2.a) / 255;
  return c;
}

//Divides a color through an integer
ColorRGB operator/(const ColorRGB& color, int a)
{
  if(a == 0) return color;
  ColorRGB c;
  c.r = color.r / a;
  c.g = color.g / a;
  c.b = color.b / a;
  c.a = color.a;
  return c;
}

//Add two colors including their alpha channel
ColorRGB operator|(const ColorRGB& color, const ColorRGB& color2)
{
  ColorRGB c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  c.a = color.a + color2.a;
  return c;
}

//Multiplies a color with an integer, including alpha channel
ColorRGB operator&(const ColorRGB& color, int a)
{
  ColorRGB c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Multiplies a color with an integer, including alpha channel
ColorRGB operator&(int a, const ColorRGB& color)
{
  ColorRGB c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGB operator&(const ColorRGB& color, double a)
{
  ColorRGB c;
  c.r = int(color.r * a);
  c.g = int(color.g * a);
  c.b = int(color.b * a);
  c.a = int(color.a * a);
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGB operator&(double a, const ColorRGB& color)
{
  ColorRGB c;
  c.r = int(color.r * a);
  c.g = int(color.g * a);
  c.b = int(color.b * a);
  c.a = int(color.a * a);
  return c;
}

//Are both colors equal?
bool operator==(const ColorRGB& color, const ColorRGB& color2)
{
  return(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a);
}

//Are both colors not equal?
bool operator!=(const ColorRGB& color, const ColorRGB& color2)
{
  return(!(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a));
}

////////////////////////////////////////////////////////////////////////////////

//Add two colors
ColorRGBd operator+(const ColorRGBd& color, const ColorRGBd& color2)
{
  ColorRGBd c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  return c;
}

//Subtract two colors
ColorRGBd operator-(const ColorRGBd& color, const ColorRGBd& color2)
{
  ColorRGBd c;
  c.r = color.r - color2.r;
  c.g = color.g - color2.g;
  c.b = color.b - color2.b;
  return c;
}

//Multiplying with a constant does NOT affect the alpha channel, use the "&" operator on two colors instead if you need that

//Multiplies a color with a double
ColorRGBd operator*(const ColorRGBd& color, double a)
{
  ColorRGBd c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiplies a color with a double
ColorRGBd operator*(double a, const ColorRGBd& color)
{
  ColorRGBd c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiply two colors (component by component)
ColorRGBd operator*(const ColorRGBd& color1, const ColorRGBd& color2)
{
  ColorRGBd c;
  c.r = (color1.r * color2.r);
  c.g = (color1.g * color2.g);
  c.b = (color1.b * color2.b);
  c.a = (color1.a * color2.a);
  return c;
}

//Divides a color through an value
ColorRGBd operator/(const ColorRGBd& color, double d)
{
  if(d == 0.0) return color;
  ColorRGBd c;
  c.r = color.r / d;
  c.g = color.g / d;
  c.b = color.b / d;
  c.a = color.a;
  return c;
}

//Add two colors including their alpha channel
ColorRGBd operator|(const ColorRGBd& color, const ColorRGBd& color2)
{
  ColorRGBd c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  c.a = color.a + color2.a;
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGBd operator&(const ColorRGBd& color, double a)
{
  ColorRGBd c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGBd operator&(double a, const ColorRGBd& color)
{
  ColorRGBd c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Are both colors equal?
bool operator==(const ColorRGBd& color, const ColorRGBd& color2)
{
  return(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a);
}

//Are both colors not equal?
bool operator!=(const ColorRGBd& color, const ColorRGBd& color2)
{
  return(!(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a));
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

//Add two colors
ColorRGBf operator+(const ColorRGBf& color, const ColorRGBf& color2)
{
  ColorRGBf c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  return c;
}

//Subtract two colors
ColorRGBf operator-(const ColorRGBf& color, const ColorRGBf& color2)
{
  ColorRGBf c;
  c.r = color.r - color2.r;
  c.g = color.g - color2.g;
  c.b = color.b - color2.b;
  return c;
}

//Multiplying with a constant does NOT affect the alpha channel, use the "&" operator on two colors instead if you need that

//Multiplies a color with a double
ColorRGBf operator*(const ColorRGBf& color, double a)
{
  ColorRGBf c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiplies a color with a double
ColorRGBf operator*(double a, const ColorRGBf& color)
{
  ColorRGBf c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a;
  return c;
}

//Multiply two colors (component by component)
ColorRGBf operator*(const ColorRGBf& color1, const ColorRGBf& color2)
{
  ColorRGBf c;
  c.r = (color1.r * color2.r);
  c.g = (color1.g * color2.g);
  c.b = (color1.b * color2.b);
  c.a = (color1.a * color2.a);
  return c;
}

//Divides a color through an value
ColorRGBf operator/(const ColorRGBf& color, double d)
{
  if(d == 0.0) return color;
  ColorRGBf c;
  c.r = color.r / d;
  c.g = color.g / d;
  c.b = color.b / d;
  c.a = color.a;
  return c;
}

//Add two colors including their alpha channel
ColorRGBf operator|(const ColorRGBf& color, const ColorRGBf& color2)
{
  ColorRGBf c;
  c.r = color.r + color2.r;
  c.g = color.g + color2.g;
  c.b = color.b + color2.b;
  c.a = color.a + color2.a;
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGBf operator&(const ColorRGBf& color, double a)
{
  ColorRGBf c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Multiplies a color with a double, including alpha channel
ColorRGBf operator&(double a, const ColorRGBf& color)
{
  ColorRGBf c;
  c.r = color.r * a;
  c.g = color.g * a;
  c.b = color.b * a;
  c.a = color.a * a;
  return c;
}

//Are both colors equal?
bool operator==(const ColorRGBf& color, const ColorRGBf& color2)
{
  return(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a);
}

//Are both colors not equal?
bool operator!=(const ColorRGBf& color, const ColorRGBf& color2)
{
  return(!(color.r == color2.r && color.g == color2.g && color.b == color2.b && color.a == color2.a));
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//COLOR CONVERSIONS/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
These functions convert colors from one color model to another
r=red  g=green  b=blue  h=hue  s=saturation  l=lightness  v=value
*/

//Converts an RGB color to HSL color
ColorHSL RGBtoHSL(const ColorRGB& colorRGB)
{
  ColorHSL colorHSL;
  colorHSL.a = colorRGB.a;

  int maxColor = std::max(colorRGB.r, std::max(colorRGB.g, colorRGB.b));
  int minColor = std::min(colorRGB.r, std::min(colorRGB.g, colorRGB.b));

  if(minColor == maxColor) //R = G = B, so it's a shade of grey
  {
    colorHSL.h = 0; //it doesn't matter what value it has
    colorHSL.s = 0;
    colorHSL.l = colorRGB.r; //doesn't matter if you pick r, g, or b
  }
  else
  {
    colorHSL.l = (minColor + maxColor) / 2;

    int div = colorHSL.l < 128 ? (maxColor + minColor) : (512 - maxColor - minColor);
    colorHSL.s = div == 0 ? 0 : (255 * (maxColor - minColor)) / div;

    if(colorRGB.r == maxColor) colorHSL.h = (255 * (colorRGB.g - colorRGB.b)) / (maxColor - minColor);
    else if(colorRGB.g == maxColor) colorHSL.h = 512 + (255 * (colorRGB.b - colorRGB.r)) / (maxColor - minColor);
    else colorHSL.h = 1024 + (255 * (colorRGB.r - colorRGB.g)) / (maxColor - minColor); //colorRGB.b == maxColor

    colorHSL.h /= 6; //to bring it to a number between 0 and 255
    if(colorHSL.h < 0) colorHSL.h += 256;
  }
  
  return colorHSL;
}

//Converts an HSL color to RGB color
ColorRGB HSLtoRGB(const ColorHSL& colorHSL)
{
  ColorRGB colorRGB;
  colorRGB.a = colorHSL.a;
  int temp1, temp2, tempr, tempg, tempb;

  //If saturation is 0, the color is a shade of grey
  if(colorHSL.s == 0) colorRGB.r = colorRGB.g = colorRGB.b = colorHSL.l;
  //If saturation > 0, more complex calculations are needed
  else
  {
    //set the temporary values
    if(colorHSL.l < 128) temp2 = colorHSL.l * (256 + colorHSL.s);
    else temp2 = 256 * (colorHSL.l + colorHSL.s) - (colorHSL.l * colorHSL.s);
    temp1 = 2 * 256 * colorHSL.l - temp2;
    tempr = colorHSL.h + 256 / 3;
    if(tempr > 255) tempr -= 256;
    tempg = colorHSL.h;
    tempb = colorHSL.h - 256 / 3;
    if(tempb < 0) tempb += 256;

    //red
    if(tempr < 256 / 6) colorRGB.r = (temp1 + ((temp2 - temp1) * 6 * tempr) / 256) / 256;
    else if(tempr < 128) colorRGB.r = temp2 / 256;
    else if(tempr < 512 / 3) colorRGB.r = (temp1 + ((temp2 - temp1) * (((512 / 3) - tempr) * 6)) / 256) / 256;
    else colorRGB.r = temp1 / 256;
    
     //green
    if(tempg < 256 / 6) colorRGB.g = (temp1 + ((temp2 - temp1) * 6 * tempg) / 256) / 256;
    else if(tempg < 128) colorRGB.g = temp2 / 256;
    else if(tempg < 512 / 3) colorRGB.g = (temp1 + ((temp2 - temp1) * (((512 / 3) - tempg) * 6)) / 256) / 256;
    else colorRGB.g = temp1 / 256;

    //blue
    if(tempb < 256 / 6) colorRGB.b = (temp1 + ((temp2 - temp1) * 6 * tempb) / 256) / 256;
    else if(tempb < 128) colorRGB.b = temp2 / 256;
    else if(tempb < 512 / 3) colorRGB.b = (temp1 + ((temp2 - temp1) * (((512 / 3) - tempb) * 6)) / 256) / 256;
    else colorRGB.b = temp1 / 256;
  }

  return colorRGB;
}

//Converts an RGB color to HSV color
ColorHSV RGBtoHSV(const ColorRGB& colorRGB)
{
  ColorHSV colorHSV;
  colorHSV.a = colorRGB.a;

  int maxColor = std::max(colorRGB.r, std::max(colorRGB.g, colorRGB.b));
  int minColor = std::min(colorRGB.r, std::min(colorRGB.g, colorRGB.b));

  colorHSV.v = maxColor;

  if(maxColor == 0) //avoid division by zero when the color is black
  {
    colorHSV.s = 0;
  }
  else
  {
    colorHSV.s = (255 * (maxColor - minColor)) / maxColor;
  }
  if(colorHSV.s == 0)
  {
    colorHSV.h = 0; //it doesn't matter what value it has
  }
  else
  {
    if(colorRGB.r == maxColor) colorHSV.h = (255 * (colorRGB.g - colorRGB.b)) / (maxColor - minColor);
    else if(colorRGB.g == maxColor) colorHSV.h = 512 + (255 * (colorRGB.b - colorRGB.r)) / (maxColor - minColor);
    else colorHSV.h = 1024 + (255 * (colorRGB.r - colorRGB.g)) / (maxColor - minColor); //colorRGB.b == maxColor

    colorHSV.h /= 6; //to bring it to a number between 0 and 1
    if(colorHSV.h < 0) colorHSV.h += 256;
  }

  return colorHSV;
}

//Converts an HSV color to RGB color
ColorRGB HSVtoRGB(const ColorHSV& colorHSV)
{
  ColorRGB colorRGB;
  colorRGB.a = colorHSV.a;

  //if saturation is 0, the color is a shade of grey
  if(colorHSV.s == 0) colorRGB.r = colorRGB.g = colorRGB.b = colorHSV.v;
  //if saturation > 0, more complex calculations are needed
  else
  {
    int f, p, q, t;
    int h = colorHSV.h * 6; //to bring hue to a number between (0 and 6) * 256, better for the calculations
    f = h % 256;

    p = (colorHSV.v * (255 - colorHSV.s)) / 256;
    q = (colorHSV.v * (255 - ((colorHSV.s * f) / 256))) / 256;
    t = (colorHSV.v * (255 - ((colorHSV.s * (255 - f)) / 256))) / 256;

    switch(h / 256)
    {
      case 0: colorRGB.r=colorHSV.v; colorRGB.g=t; colorRGB.b=p; break;
      case 1: colorRGB.r=q; colorRGB.g=colorHSV.v; colorRGB.b=p; break;
      case 2: colorRGB.r=p; colorRGB.g=colorHSV.v; colorRGB.b=t; break;
      case 3: colorRGB.r=p; colorRGB.g=q; colorRGB.b=colorHSV.v; break;
      case 4: colorRGB.r=t; colorRGB.g=p; colorRGB.b=colorHSV.v; break;
      default:colorRGB.r=colorHSV.v; colorRGB.g=p; colorRGB.b=q; break; //this be case 5, it's mathematically impossible for i to be something else
    }
  }
  
  return colorRGB;
}

//converts an RGB color from 3 unsigned char's to a single integer
unsigned long RGBtoINT(const ColorRGB& colorRGB)
{
  return 65536 * colorRGB.r + 256 * colorRGB.g + colorRGB.b;
}

//converts an RGB color given as a single integer to three unsigned char's
ColorRGB INTtoRGB(unsigned long colorINT)
{
  ColorRGB colorRGB;
  colorRGB.r = (colorINT / 65536) % 256;
  colorRGB.g = (colorINT / 256) % 256;
  colorRGB.b = colorINT % 256;
  return colorRGB;
}

ColorCMY RGBtoCMY(const ColorRGB& colorRGB)
{
  return ColorCMY(255 - colorRGB.r, 255 - colorRGB.g, 255 - colorRGB.b, colorRGB.a);
}

ColorRGB CMYtoRGB(const ColorCMY& colorCMY)
{
  return ColorRGB(255 - colorCMY.c, 255 - colorCMY.m, 255 - colorCMY.y, colorCMY.a);
}

ColorCMYK RGBtoCMYK(const ColorRGB& colorRGB)
{
  ColorCMYK result(255 - colorRGB.r, 255 - colorRGB.g, 255 - colorRGB.b, 255, colorRGB.a);
  if(result.c < result.k) result.k = result.c;
  if(result.m < result.k) result.k = result.m;
  if(result.y < result.k) result.k = result.y;
  if(result.k == 255)
  {
    result.c = result.m = result.y = 0;
  }
  else
  {
    result.c = (int)((result.c - result.k) / (1.0 - (result.k / 255.0)));
    result.m = (int)((result.m - result.k) / (1.0 - (result.k / 255.0)));
    result.y = (int)((result.y - result.k) / (1.0 - (result.k / 255.0)));
  }
  return result;
}

ColorRGB CMYKtoRGB(const ColorCMYK& colorCMYK)
{
  double k = colorCMYK.k / 255.0;
  ColorCMY colorCMY((int)(colorCMYK.c * (1.0 - k) + colorCMYK.k)
                  , (int)(colorCMYK.m * (1.0 - k) + colorCMYK.k)
                  , (int)(colorCMYK.y * (1.0 - k) + colorCMYK.k)
                  , colorCMYK.a);
  return ColorRGB(255 - colorCMY.c, 255 - colorCMY.m, 255 - colorCMY.y, colorCMY.a);
}

ColorCIEXYZ RGBtoCIEXYZ(const ColorRGB& colorRGB)
{
  ColorRGBd temp(colorRGB.r / 255.0, colorRGB.g / 255.0, colorRGB.b / 255.0, 255);
  ColorCIEXYZd temp2 = RGBtoCIEXYZ(temp);
  return ColorCIEXYZ((int)(temp2.x * 255.0), (int)(temp2.y * 255.0), (int)(temp2.z * 255.0), colorRGB.a);
}

ColorRGB CIEXYZtoRGB(const ColorCIEXYZ& colorCIEXYZ)
{
  ColorCIEXYZd temp(colorCIEXYZ.x / 255.0, colorCIEXYZ.y / 255.0, colorCIEXYZ.z / 255.0, 255);
  ColorRGBd temp2 = CIEXYZtoRGB(temp);
  return ColorRGB((int)(temp2.r * 255.0), (int)(temp2.g * 255.0), (int)(temp2.b * 255.0), colorCIEXYZ.alpha);
}

ColorCIELab RGBtoCIELab(const ColorRGB& colorRGB)
{
  ColorRGBd temp(colorRGB.r / 255.0, colorRGB.g / 255.0, colorRGB.b / 255.0, 255);
  ColorCIELabd temp2 = RGBtoCIELab(temp);
  return ColorCIELab((int)(temp2.l * 255.0), (int)(temp2.a * 255.0), (int)(temp2.b * 255.0), colorRGB.a);
}

ColorRGB CIELabtoRGB(const ColorCIELab& colorCIELab)
{
  ColorCIELabd temp(colorCIELab.l / 255.0, colorCIELab.a / 255.0, colorCIELab.b / 255.0, 255);
  ColorRGBd temp2 = CIELabtoRGB(temp);
  return ColorRGB((int)(temp2.r * 255.0), (int)(temp2.g * 255.0), (int)(temp2.b * 255.0), colorCIELab.alpha);
}

ColorYPbPr RGBtoYPbPr(const ColorRGB& colorRGB)
{
  ColorYPbPr result;
  result.y = colorRGB.r / 3 + colorRGB.g / 3 + colorRGB.b / 3;
  result.pb = colorRGB.b - result.y;
  result.pr = colorRGB.r - result.y;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGB YPbPrtoRGB(const ColorYPbPr& colorYPbPr)
{
  ColorRGB result;
  result.r = colorYPbPr.y + colorYPbPr.pr;
  result.g = colorYPbPr.y - colorYPbPr.pr - colorYPbPr.pb;
  result.b = colorYPbPr.y + colorYPbPr.pb;
  result.a = colorYPbPr.alpha;
  return result;
}

ColorYCbCr RGBtoYCbCr(const ColorRGB& colorRGB)
{
  ColorYCbCr result;
  result.y   = (int)( 0.299  * colorRGB.r + 0.587  * colorRGB.g + 0.114  * colorRGB.b);
  result.cb  = (int)(-0.1687 * colorRGB.r - 0.3313 * colorRGB.g + 0.5    * colorRGB.b) + 128;
  result.cr  = (int)( 0.5    * colorRGB.r - 0.4187 * colorRGB.g - 0.0813 * colorRGB.b) + 128;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGB YCbCrtoRGB(const ColorYCbCr& colorYCbCr)
{
  ColorRGB result;
  result.r = (int)(colorYCbCr.y                                   + 1.402   * (colorYCbCr.cr - 128));
  result.g = (int)(colorYCbCr.y - 0.34414 * (colorYCbCr.cb - 128) - 0.71414 * (colorYCbCr.cr - 128));
  result.b = (int)(colorYCbCr.y + 1.772   * (colorYCbCr.cb - 128));
  result.a = colorYCbCr.alpha;
  return result;
}

ColorYUV RGBtoYUV(const ColorRGB& colorRGB)
{
  ColorYUV result;
  double y   = 0.299   * colorRGB.r + 0.587   * colorRGB.g + 0.114   * colorRGB.b;
  double u  = -0.14713 * colorRGB.r - 0.28886 * colorRGB.g + 0.436   * colorRGB.b;
  double v  =  0.615   * colorRGB.r - 0.51499 * colorRGB.g - 0.10001 * colorRGB.b;
  result.y = (int)(y);
  result.u = (int)(u / 0.436);
  result.v = (int)(v / 0.615);
  result.alpha = colorRGB.a;
  return result;
}

ColorRGB YUVtoRGB(const ColorYUV& colorYUV)
{
  ColorRGB result;
  double y = colorYUV.y;
  double u = colorYUV.u * 0.436;
  double v = colorYUV.v * 0.615;
  result.r = (int)(y               + 1.402   * v);
  result.g = (int)(y - 0.39465 * u - 0.58060 * v);
  result.b = (int)(y + 2.03211 * u              );
  result.a = colorYUV.alpha;
  return result;
}

ColorYIQ RGBtoYIQ(const ColorRGB& colorRGB)
{
  ColorYIQ result;
  double y   = 0.299      * colorRGB.r + 0.587    * colorRGB.g + 0.114    * colorRGB.b;
  double i  = -0.595716   * colorRGB.r - 0.274453 * colorRGB.g - 0.321263 * colorRGB.b;
  double q  =  0.211456   * colorRGB.r - 0.522591 * colorRGB.g + 0.311135 * colorRGB.b;
  result.y = (int)(y);
  result.i = (int)(i / 0.5957);
  result.q = (int)(q / 0.5226);
  result.alpha = colorRGB.a;
  return result;
}

ColorRGB YIQtoRGB(const ColorYIQ& colorYIQ)
{
  ColorRGB result;
  double y = colorYIQ.y;
  double i = colorYIQ.i * 0.5957;
  double q = colorYIQ.q * 0.5226;
  result.r = (int)(y + 0.9563 * i + 0.6210 * q);
  result.g = (int)(y - 0.2721 * i - 0.6474 * q);
  result.b = (int)(y - 1.1070 * i + 1.7046 * q);
  result.a = colorYIQ.alpha;
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



//Converts an RGB color to HSL color
ColorHSLd RGBtoHSL(const ColorRGBd& colorRGB)
{
  double r, g, b, h, s, l; //this function works with floats between 0 and 1
  r = colorRGB.r;
  g = colorRGB.g;
  b = colorRGB.b;

  double maxColor = std::max(r, std::max(g, b));
  double minColor = std::min(r, std::min(g, b));

  if(minColor == maxColor) //R = G = B, so it's a shade of grey
  {
    h = 0.0; //it doesn't matter what value it has
    s = 0.0;
    l = r; //doesn't matter if you pick r, g, or b
  }
  else
  {
    l = (minColor + maxColor) / 2.0;
    
    double div = l < 0.5 ? (maxColor + minColor) : (2.0 - maxColor - minColor);
    s = div == 0.0 ? 0.0 : (maxColor - minColor) / div;

    if(r == maxColor) h = (g - b) / (maxColor - minColor);
    else if(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
    else h = 4.0 + (r - g) / (maxColor - minColor); //b == maxColor

    h /= 6.0; //to bring it to a number between 0 and 1
    if(h < 0.0) h += 1.0;
  }
  
  ColorHSLd colorHSL;
  colorHSL.h = h;
  colorHSL.s = s;
  colorHSL.l = l;
  colorHSL.a = colorRGB.a;
  return colorHSL;
}

//Converts an HSL color to RGB color
ColorRGBd HSLtoRGB(const ColorHSLd& colorHSL)
{
  double r, g, b, h, s, l; //this function works with floats between 0 and 1
  double temp1, temp2, tempr, tempg, tempb;
  h = colorHSL.h;
  s = colorHSL.s;
  l = colorHSL.l;

  //If saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = l;
  //If saturation > 0, more complex calculations are needed
  else
  {
    //set the temporary values
    if(l < 0.5) temp2 = l * (1 + s);
    else temp2 = (l + s) - (l * s);
    temp1 = 2.0 * l - temp2;
    tempr=h + 1.0 / 3.0;
    if(tempr > 1.0) tempr--;
    tempg=h;
    tempb=h-1.0 / 3.0;
    if(tempb < 0.0) tempb++;

    //red
    if(tempr < 1.0 / 6.0) r = temp1 + (temp2 - temp1) * 6.0 * tempr;
    else if(tempr < 0.5) r = temp2;
    else if(tempr < 2.0 / 3.0) r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
    else r = temp1;
    
     //green
    if(tempg < 1.0 / 6.0) g = temp1 + (temp2 - temp1) * 6.0 * tempg;
    else if(tempg < 0.5) g=temp2;
    else if(tempg < 2.0 / 3.0) g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
    else g = temp1;

    //blue
    if(tempb < 1.0 / 6.0) b = temp1 + (temp2 - temp1) * 6.0 * tempb;
    else if(tempb < 0.5) b = temp2;
    else if(tempb < 2.0 / 3.0) b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
    else b = temp1;
  }

  ColorRGBd colorRGB;
  colorRGB.r = r;
  colorRGB.g = g;
  colorRGB.b = b;
  colorRGB.a = colorHSL.a;
  return colorRGB;
}

//Converts an RGB color to HSV color
ColorHSVd RGBtoHSV(const ColorRGBd& colorRGB)
{
  double r, g, b, h, s, v; //this function works with floats between 0 and 1
  r = colorRGB.r;
  g = colorRGB.g;
  b = colorRGB.b;

  double maxColor = std::max(r, std::max(g, b));
  double minColor = std::min(r, std::min(g, b));

  v = maxColor;

  if(maxColor == 0.0) //avoid division by zero when the color is black
  {
    s = 0.0;
  }
  else
  {
    s = (maxColor - minColor) / maxColor;
  }
  if(s == 0.0)
  {
    h = 0.0; //it doesn't matter what value it has
  }
  else
  {
    if(r == maxColor) h = (g - b) / (maxColor - minColor);
    else if(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
    else h = 4.0 + (r - g) / (maxColor - minColor); //b == maxColor

    h /= 6.0; //to bring it to a number between 0 and 1
    if(h < 0.0) h++;
  }

  ColorHSVd colorHSV;
  colorHSV.h = h;
  colorHSV.s = s;
  colorHSV.v = v;
  colorHSV.a = colorRGB.a;
  return colorHSV;
}

//Converts an HSV color to RGB color
ColorRGBd HSVtoRGB(const ColorHSVd& colorHSV)
{
  double r, g, b, h, s, v; //this function works with floats between 0 and 1
  h = colorHSV.h;
  s = colorHSV.s;
  v = colorHSV.v;

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = v;
  //if saturation > 0, more complex calculations are needed
  else
  {
    double f, p, q, t;
    int i;
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - floor(h); //the fractional part of h

    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      default: r=v; g=p; b=q; break; //this be case 5, it's mathematically impossible for i to be something else
    }
  }
  
  ColorRGBd colorRGB;
  colorRGB.r = r;
  colorRGB.g = g;
  colorRGB.b = b;
  colorRGB.a = colorHSV.a;
  return colorRGB;
}

ColorCMYd RGBtoCMY(const ColorRGBd& colorRGB)
{
  return ColorCMYd(1.0 - colorRGB.r, 1.0 - colorRGB.g, 1.0 - colorRGB.b, colorRGB.a);
}

ColorRGBd CMYtoRGB(const ColorCMYd& colorCMY)
{
  return ColorRGBd(1.0 - colorCMY.c, 1.0 - colorCMY.m, 1.0 - colorCMY.y, colorCMY.a);
}

ColorCMYKd RGBtoCMYK(const ColorRGBd& colorRGB)
{
  ColorCMYKd result(1.0 - colorRGB.r, 1.0 - colorRGB.g, 1.0 - colorRGB.b, 1.0, colorRGB.a);
  if(result.c < result.k) result.k = result.c;
  if(result.m < result.k) result.k = result.m;
  if(result.y < result.k) result.k = result.y;
  if(result.k == 1.0)
  {
    result.c = result.m = result.y = 0.0;
  }
  else
  {
    result.c = (result.c - result.k) / (1.0 - result.k);
    result.m = (result.m - result.k) / (1.0 - result.k);
    result.y = (result.y - result.k) / (1.0 - result.k);
  }
  result.a = colorRGB.a;
  return result;
}

ColorRGBd CMYKtoRGB(const ColorCMYKd& colorCMYK)
{
  double k = colorCMYK.k;
  ColorCMYd colorCMY(colorCMYK.c * (1.0 - k) + k, colorCMYK.m * (1.0 - k) + k, colorCMYK.y * (1.0 - k) + k, colorCMYK.a);
  return ColorRGBd(1.0 - colorCMY.c, 1.0 - colorCMY.m, 1.0 - colorCMY.y, colorCMY.a);
}

ColorCIEXYZd RGBtoCIEXYZ(const ColorRGBd& colorRGB)
{
// var_R = ( R / 255 )        //R from 0 to 255
// var_G = ( G / 255 )        //G from 0 to 255
// var_B = ( B / 255 )        //B from 0 to 255
// 
// if ( var_R > 0.04045 ) var_R = ( ( var_R + 0.055 ) / 1.055 ) ^ 2.4
// else                   var_R = var_R / 12.92
// if ( var_G > 0.04045 ) var_G = ( ( var_G + 0.055 ) / 1.055 ) ^ 2.4
// else                   var_G = var_G / 12.92
// if ( var_B > 0.04045 ) var_B = ( ( var_B + 0.055 ) / 1.055 ) ^ 2.4
// else                   var_B = var_B / 12.92
// 
// var_R = var_R * 100
// var_G = var_G * 100
// var_B = var_B * 100
// 
// //Observer. = 2°, Illuminant = D65
// X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805
// Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722
// Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505

  ColorCIEXYZd result;
  double factor = 1.0;//1.0/0.17697;
  result.x = factor * (0.49 * colorRGB.r + 0.31 * colorRGB.g + 0.20 * colorRGB.b);
  result.y = factor * (0.17697 * colorRGB.r + 0.81240 * colorRGB.g + 0.01063 * colorRGB.b);
  result.z = factor * (0.00 * colorRGB.r + 0.01 * colorRGB.g + 0.99 * colorRGB.b);
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd CIEXYZtoRGB(const ColorCIEXYZd& colorCIEXYZ)
{
  ColorRGBd result;
  double factor = 1.0;//0.17697;
  result.r = factor * (2.3646138 * colorCIEXYZ.x - 0.8965406 * colorCIEXYZ.y - 0.4680733 * colorCIEXYZ.z);
  result.g = factor * ((-0.5151662) * colorCIEXYZ.x + 1.4264081 * colorCIEXYZ.y + 0.0887581 * colorCIEXYZ.z);
  result.b = factor * (0.0052037 * colorCIEXYZ.x - 0.0144082 * colorCIEXYZ.y + 1.0092045 * colorCIEXYZ.z);
  result.a = colorCIEXYZ.alpha;
  return result;

//   var_X = X / 100        //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
//   var_Y = Y / 100        //Y from 0 to 100.000
//   var_Z = Z / 100        //Z from 0 to 108.883
//   
//   var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986
//   var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415
//   var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570
//   
//   if ( var_R > 0.0031308 ) var_R = 1.055 * ( var_R ^ ( 1 / 2.4 ) ) - 0.055
//   else                     var_R = 12.92 * var_R
//   if ( var_G > 0.0031308 ) var_G = 1.055 * ( var_G ^ ( 1 / 2.4 ) ) - 0.055
//   else                     var_G = 12.92 * var_G
//   if ( var_B > 0.0031308 ) var_B = 1.055 * ( var_B ^ ( 1 / 2.4 ) ) - 0.055
//   else                     var_B = 12.92 * var_B
//   
//   R = var_R * 255
//   G = var_G * 255
//   B = var_B * 255 
}

namespace
{
  double labf(double t)
  {
    static const double delta = 6.0 / 29.0;
    static const double v = delta * delta * delta; //(6/29)^3
    static const double f = (1.0 / 3.0) / (delta * delta);
    if(t > v) return std::pow(t, 1.0 / 3.0);
    else return f * t + 4.0 / 29.0;
  }
}

ColorCIELabd RGBtoCIELab(const ColorRGBd& colorRGB)
{
  ColorCIEXYZd colorXYZ = RGBtoCIEXYZ(colorRGB);
  ColorCIELabd result;
  
  //CIE XYZ tristimulus values of white point
  static const double xn = 1.0;
  static const double yn = 1.0;
  static const double zn = 1.0;

  result.l = 1.16 * labf(colorXYZ.y / yn) - 0.16;
  result.a = 5.0 * (labf(colorXYZ.x / xn) - labf(colorXYZ.y / yn));
  result.b = 2.0 * (labf(colorXYZ.y / yn) - labf(colorXYZ.z / zn));
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd CIELabtoRGB(const ColorCIELabd& colorCIELab)
{
  
  ColorCIEXYZd colorXYZ;
  
  static const double delta = 6.0 / 29.0;
  double fy = (colorCIELab.l + 0.16) / 1.16;
  double fx = fy + colorCIELab.a / 5.0;
  double fz = fy - colorCIELab.b / 2.0;
  
  //CIE XYZ tristimulus values of white point
  static const double xn = 1.0;
  static const double yn = 1.0;
  static const double zn = 1.0;
  
  double sixteens = 16.0 / 116.0;
  double tds = 3.0 * delta * delta; //"three delta square"
  
  colorXYZ.x = fx > delta ? xn * fx * fx * fx : (fx - sixteens) * tds * xn;
  colorXYZ.y = fy > delta ? yn * fy * fy * fy : (fy - sixteens) * tds * yn;
  colorXYZ.z = fz > delta ? zn * fz * fz * fz : (fz - sixteens) * tds * zn;
  
  ColorRGBd result = CIEXYZtoRGB(colorXYZ);
  result.a = colorCIELab.alpha;
  return result;
}

ColorYPbPrd RGBtoYPbPr(const ColorRGBd& colorRGB)
{
  ColorYPbPrd result;
  result.y = (colorRGB.r + colorRGB.g + colorRGB.b) / 3.0;
  result.pb = colorRGB.b - result.y;
  result.pr = colorRGB.r - result.y;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd YPbPrtoRGB(const ColorYPbPrd& colorYPbPr)
{
  ColorRGBd result;
  result.r = colorYPbPr.y + colorYPbPr.pr;
  result.g = colorYPbPr.y - colorYPbPr.pr - colorYPbPr.pb;
  result.b = colorYPbPr.y + colorYPbPr.pb;
  result.a = colorYPbPr.alpha;
  return result;
}

ColorYCbCrd RGBtoYCbCr(const ColorRGBd& colorRGB)
{
  ColorYCbCrd result;
  result.y   =  0.299  * colorRGB.r + 0.587  * colorRGB.g + 0.114  * colorRGB.b;
  result.cb  = -0.1687 * colorRGB.r - 0.3313 * colorRGB.g + 0.5    * colorRGB.b + 0.5;
  result.cr  =  0.5    * colorRGB.r - 0.4187 * colorRGB.g - 0.0813 * colorRGB.b + 0.5;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd YCbCrtoRGB(const ColorYCbCrd& colorYCbCr)
{
  ColorRGBd result;
  result.r = colorYCbCr.y                                   + 1.402   * (colorYCbCr.cr - 0.5);
  result.g = colorYCbCr.y - 0.34414 * (colorYCbCr.cb - 0.5) - 0.71414 * (colorYCbCr.cr - 0.5);
  result.b = colorYCbCr.y + 1.772   * (colorYCbCr.cb - 0.5);
  result.a = colorYCbCr.alpha;
  return result;
}


ColorYUVd RGBtoYUV(const ColorRGBd& colorRGB)
{
  ColorYUVd result;
  double y   = 0.299   * colorRGB.r + 0.587   * colorRGB.g + 0.114   * colorRGB.b;
  double u  = -0.14713 * colorRGB.r - 0.28886 * colorRGB.g + 0.436   * colorRGB.b;
  double v  =  0.615   * colorRGB.r - 0.51499 * colorRGB.g - 0.10001 * colorRGB.b;
  result.y = y;
  result.u = u / 0.436;
  result.v = v / 0.615;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd YUVtoRGB(const ColorYUVd& colorYUV)
{
  ColorRGBd result;
  double y = colorYUV.y;
  double u = colorYUV.u * 0.436;
  double v = colorYUV.v * 0.615;
  result.r = y               + 1.402   * v;
  result.g = y - 0.39465 * u - 0.58060 * v;
  result.b = y + 2.03211 * u              ;
  result.a = colorYUV.alpha;
  return result;
}

ColorYIQd RGBtoYIQ(const ColorRGBd& colorRGB)
{
  ColorYIQd result;
  double y   = 0.299      * colorRGB.r + 0.587    * colorRGB.g + 0.114    * colorRGB.b;
  double i  = -0.595716   * colorRGB.r - 0.274453 * colorRGB.g - 0.321263 * colorRGB.b;
  double q  =  0.211456   * colorRGB.r - 0.522591 * colorRGB.g + 0.311135 * colorRGB.b;
  result.y = y;
  result.i = i / 0.5957;
  result.q = q / 0.5226;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBd YIQtoRGB(const ColorYIQd& colorYIQ)
{
  ColorRGBd result;
  double y = colorYIQ.y;
  double i = colorYIQ.i * 0.5957;
  double q = colorYIQ.q * 0.5226;
  result.r = y + 0.9563 * i + 0.6210 * q;
  result.g = y - 0.2721 * i - 0.6474 * q;
  result.b = y - 1.1070 * i + 1.7046 * q;
  result.a = colorYIQ.alpha;
  return result;
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



//Converts an RGB color to HSL color
ColorHSLf RGBtoHSL(const ColorRGBf& colorRGB)
{
  float r, g, b, h, s, l; //this function works with floats between 0 anf 1
  r = colorRGB.r;
  g = colorRGB.g;
  b = colorRGB.b;

  float maxColor = std::max(r, std::max(g, b));
  float minColor = std::min(r, std::min(g, b));

  if(minColor == maxColor) //R = G = B, so it's a shade of grey
  {
    h = 0.0; //it doesn't matter what value it has
    s = 0.0;
    l = r; //doesn't matter if you pick r, g, or b
  }
  else
  {
    l = (minColor + maxColor) / 2.0;
    
    float fiv = l < 0.5 ? (maxColor + minColor) : (2.0 - maxColor - minColor);
    s = fiv == 0.0 ? 0.0 : (maxColor - minColor) / fiv;

    if(r == maxColor) h = (g - b) / (maxColor - minColor);
    else if(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
    else h = 4.0 + (r - g) / (maxColor - minColor); //b == maxColor

    h /= 6.0; //to bring it to a number between 0 and 1
    if(h < 0.0) h += 1.0;
  }
  
  ColorHSLf colorHSL;
  colorHSL.h = h;
  colorHSL.s = s;
  colorHSL.l = l;
  colorHSL.a = colorRGB.a;
  return colorHSL;
}

//Converts an HSL color to RGB color
ColorRGBf HSLtoRGB(const ColorHSLf& colorHSL)
{
  float r, g, b, h, s, l; //this function works with floats between 0 anf 1
  float temp1, temp2, tempr, tempg, tempb;
  h = colorHSL.h;
  s = colorHSL.s;
  l = colorHSL.l;

  //If saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = l;
  //If saturation > 0, more complex calculations are needed
  else
  {
    //set the temporary values
    if(l < 0.5) temp2 = l * (1 + s);
    else temp2 = (l + s) - (l * s);
    temp1 = 2.0 * l - temp2;
    tempr=h + 1.0 / 3.0;
    if(tempr > 1.0) tempr--;
    tempg=h;
    tempb=h-1.0 / 3.0;
    if(tempb < 0.0) tempb++;

    //red
    if(tempr < 1.0 / 6.0) r = temp1 + (temp2 - temp1) * 6.0 * tempr;
    else if(tempr < 0.5) r = temp2;
    else if(tempr < 2.0 / 3.0) r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
    else r = temp1;
    
     //green
    if(tempg < 1.0 / 6.0) g = temp1 + (temp2 - temp1) * 6.0 * tempg;
    else if(tempg < 0.5) g=temp2;
    else if(tempg < 2.0 / 3.0) g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
    else g = temp1;

    //blue
    if(tempb < 1.0 / 6.0) b = temp1 + (temp2 - temp1) * 6.0 * tempb;
    else if(tempb < 0.5) b = temp2;
    else if(tempb < 2.0 / 3.0) b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
    else b = temp1;
  }

  ColorRGBf colorRGB;
  colorRGB.r = r;
  colorRGB.g = g;
  colorRGB.b = b;
  colorRGB.a = colorHSL.a;
  return colorRGB;
}

//Converts an RGB color to HSV color
ColorHSVf RGBtoHSV(const ColorRGBf& colorRGB)
{
  float r, g, b, h, s, v; //this function works with floats between 0 anf 1
  r = colorRGB.r;
  g = colorRGB.g;
  b = colorRGB.b;

  float maxColor = std::max(r, std::max(g, b));
  float minColor = std::min(r, std::min(g, b));

  v = maxColor;

  if(maxColor == 0.0) //avoid division by zero when the color is black
  {
    s = 0.0;
  }
  else
  {
    s = (maxColor - minColor) / maxColor;
  }
  if(s == 0.0)
  {
    h = 0.0; //it doesn't matter what value it has
  }
  else
  {
    if(r == maxColor) h = (g - b) / (maxColor - minColor);
    else if(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
    else h = 4.0 + (r - g) / (maxColor - minColor); //b == maxColor

    h /= 6.0; //to bring it to a number between 0 anf 1
    if(h < 0.0) h++;
  }

  ColorHSVf colorHSV;
  colorHSV.h = h;
  colorHSV.s = s;
  colorHSV.v = v;
  colorHSV.a = colorRGB.a;
  return colorHSV;
}

//Converts an HSV color to RGB color
ColorRGBf HSVtoRGB(const ColorHSVf& colorHSV)
{
  float r, g, b, h, s, v; //this function works with floats between 0 anf 1
  h = colorHSV.h;
  s = colorHSV.s;
  v = colorHSV.v;

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = v;
  //if saturation > 0, more complex calculations are needed
  else
  {
    float f, p, q, t;
    int i;
    h *= 6.0; //to bring hue to a number between 0 anf 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 anf 3.01 becomes 3 or 4.9999 becomes 4
    f = h - floor(h); //the fractional part of h

    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      default: r=v; g=p; b=q; break; //this be case 5, it's mathematically impossible for i to be something else
    }
  }
  
  ColorRGBf colorRGB;
  colorRGB.r = r;
  colorRGB.g = g;
  colorRGB.b = b;
  colorRGB.a = colorHSV.a;
  return colorRGB;
}

ColorCMYf RGBtoCMY(const ColorRGBf& colorRGB)
{
  return ColorCMYf(1.0 - colorRGB.r, 1.0 - colorRGB.g, 1.0 - colorRGB.b, colorRGB.a);
}

ColorRGBf CMYtoRGB(const ColorCMYf& colorCMY)
{
  return ColorRGBf(1.0 - colorCMY.c, 1.0 - colorCMY.m, 1.0 - colorCMY.y, colorCMY.a);
}

ColorCMYKf RGBtoCMYK(const ColorRGBf& colorRGB)
{
  ColorCMYKf result(1.0 - colorRGB.r, 1.0 - colorRGB.g, 1.0 - colorRGB.b, 1.0, colorRGB.a);
  if(result.c < result.k) result.k = result.c;
  if(result.m < result.k) result.k = result.m;
  if(result.y < result.k) result.k = result.y;
  if(result.k == 1.0)
  {
    result.c = result.m = result.y = 0.0;
  }
  else
  {
    result.c = (result.c - result.k) / (1.0 - result.k);
    result.m = (result.m - result.k) / (1.0 - result.k);
    result.y = (result.y - result.k) / (1.0 - result.k);
  }
  result.a = colorRGB.a;
  return result;
}

ColorRGBf CMYKtoRGB(const ColorCMYKf& colorCMYK)
{
  float k = colorCMYK.k;
  ColorCMYf colorCMY(colorCMYK.c * (1.0 - k) + k, colorCMYK.m * (1.0 - k) + k, colorCMYK.y * (1.0 - k) + k, colorCMYK.a);
  return ColorRGBf(1.0 - colorCMY.c, 1.0 - colorCMY.m, 1.0 - colorCMY.y, colorCMY.a);
}

ColorCIEXYZf RGBtoCIEXYZ(const ColorRGBf& colorRGB)
{
  ColorCIEXYZf result;
  float factor = 1.0;//1.0/0.17697;
  result.x = factor * (0.49 * colorRGB.r + 0.31 * colorRGB.g + 0.20 * colorRGB.b);
  result.y = factor * (0.17697 * colorRGB.r + 0.81240 * colorRGB.g + 0.01063 * colorRGB.b);
  result.z = factor * (0.00 * colorRGB.r + 0.01 * colorRGB.g + 0.99 * colorRGB.b);
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf CIEXYZtoRGB(const ColorCIEXYZf& colorCIEXYZ)
{
  ColorRGBf result;
  float factor = 1.0;//0.17697;
  result.r = factor * (2.3646138 * colorCIEXYZ.x - 0.8965406 * colorCIEXYZ.y - 0.4680733 * colorCIEXYZ.z);
  result.g = factor * ((-0.5151662) * colorCIEXYZ.x + 1.4264081 * colorCIEXYZ.y + 0.0887581 * colorCIEXYZ.z);
  result.b = factor * (0.0052037 * colorCIEXYZ.x - 0.0144082 * colorCIEXYZ.y + 1.0092045 * colorCIEXYZ.z);
  result.a = colorCIEXYZ.alpha;
  return result;
}

namespace
{
  float labf(float t)
  {
    static const float felta = 6.0 / 29.0;
    static const float v = felta * felta * felta; //(6/29)^3
    static const float f = (1.0 / 3.0) / (felta * felta);
    if(t > v) return std::pow(t, 1.0f / 3.0f);
    else return f * t + 4.0 / 29.0;
  }
}

ColorCIELabf RGBtoCIELab(const ColorRGBf& colorRGB)
{
  ColorCIEXYZf colorXYZ = RGBtoCIEXYZ(colorRGB);
  ColorCIELabf result;
  
  //CIE XYZ tristimulus values of white point
  static const float xn = 1.0;
  static const float yn = 1.0;
  static const float zn = 1.0;

  result.l = 1.16 * labf(colorXYZ.y / yn) - 0.16;
  result.a = 5.0 * (labf(colorXYZ.x / xn) - labf(colorXYZ.y / yn));
  result.b = 2.0 * (labf(colorXYZ.y / yn) - labf(colorXYZ.z / zn));
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf CIELabtoRGB(const ColorCIELabf& colorCIELab)
{
  
  ColorCIEXYZf colorXYZ;
  
  static const float felta = 6.0 / 29.0;
  float fy = (colorCIELab.l + 0.16) / 1.16;
  float fx = fy + colorCIELab.a / 5.0;
  float fz = fy - colorCIELab.b / 2.0;
  
  //CIE XYZ tristimulus values of white point
  static const float xn = 1.0;
  static const float yn = 1.0;
  static const float zn = 1.0;
  
  float sixteens = 16.0 / 116.0;
  float tfs = 3.0 * felta * felta; //"three delta square"
  
  colorXYZ.x = fx > felta ? xn * fx * fx * fx : (fx - sixteens) * tfs * xn;
  colorXYZ.y = fy > felta ? yn * fy * fy * fy : (fy - sixteens) * tfs * yn;
  colorXYZ.z = fz > felta ? zn * fz * fz * fz : (fz - sixteens) * tfs * zn;
  
  ColorRGBf result = CIEXYZtoRGB(colorXYZ);
  result.a = colorCIELab.alpha;
  return result;
}

ColorYPbPrf RGBtoYPbPr(const ColorRGBf& colorRGB)
{
  ColorYPbPrf result;
  result.y = (colorRGB.r + colorRGB.g + colorRGB.b) / 3.0;
  result.pb = colorRGB.b - result.y;
  result.pr = colorRGB.r - result.y;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf YPbPrtoRGB(const ColorYPbPrf& colorYPbPr)
{
  ColorRGBf result;
  result.r = colorYPbPr.y + colorYPbPr.pr;
  result.g = colorYPbPr.y - colorYPbPr.pr - colorYPbPr.pb;
  result.b = colorYPbPr.y + colorYPbPr.pb;
  result.a = colorYPbPr.alpha;
  return result;
}

ColorYCbCrf RGBtoYCbCr(const ColorRGBf& colorRGB)
{
  ColorYCbCrf result;
  result.y   =  0.299f  * colorRGB.r + 0.587f  * colorRGB.g + 0.114f  * colorRGB.b;
  result.cb  = -0.1687f * colorRGB.r - 0.3313f * colorRGB.g + 0.5f    * colorRGB.b + 0.5f;
  result.cr  =  0.5f    * colorRGB.r - 0.4187f * colorRGB.g - 0.0813f * colorRGB.b + 0.5f;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf YCbCrtoRGB(const ColorYCbCrf& colorYCbCr)
{
  ColorRGBf result;
  result.r = colorYCbCr.y                                   + 1.402   * (colorYCbCr.cr - 0.5f);
  result.g = colorYCbCr.y - 0.34414 * (colorYCbCr.cb - 0.5) - 0.71414 * (colorYCbCr.cr - 0.5f);
  result.b = colorYCbCr.y + 1.772   * (colorYCbCr.cb - 0.5);
  result.a = colorYCbCr.alpha;
  return result;
}


ColorYUVf RGBtoYUV(const ColorRGBf& colorRGB)
{
  ColorYUVf result;
  float y   = 0.299f   * colorRGB.r + 0.587f   * colorRGB.g + 0.114f   * colorRGB.b;
  float u  = -0.14713f * colorRGB.r - 0.28886f * colorRGB.g + 0.436f   * colorRGB.b;
  float v  =  0.615f   * colorRGB.r - 0.51499f * colorRGB.g - 0.10001f * colorRGB.b;
  result.y = y;
  result.u = u / 0.436f;
  result.v = v / 0.615f;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf YUVtoRGB(const ColorYUVf& colorYUV)
{
  ColorRGBf result;
  float y = colorYUV.y;
  float u = colorYUV.u * 0.436f;
  float v = colorYUV.v * 0.615f;
  result.r = y               + 1.402f   * v;
  result.g = y - 0.39465f * u - 0.58060f * v;
  result.b = y + 2.03211f * u              ;
  result.a = colorYUV.alpha;
  return result;
}

ColorYIQf RGBtoYIQ(const ColorRGBf& colorRGB)
{
  ColorYIQf result;
  float y   = 0.299f      * colorRGB.r + 0.587f    * colorRGB.g + 0.114f    * colorRGB.b;
  float i  = -0.595716f   * colorRGB.r - 0.274453f * colorRGB.g - 0.321263f * colorRGB.b;
  float q  =  0.211456f   * colorRGB.r - 0.522591f * colorRGB.g + 0.311135f * colorRGB.b;
  result.y = y;
  result.i = i / 0.5957f;
  result.q = q / 0.5226f;
  result.alpha = colorRGB.a;
  return result;
}

ColorRGBf YIQtoRGB(const ColorYIQf& colorYIQ)
{
  ColorRGBf result;
  float y = colorYIQ.y;
  float i = colorYIQ.i * 0.5957f;
  float q = colorYIQ.q * 0.5226f;
  result.r = y + 0.9563f * i + 0.6210f * q;
  result.g = y - 0.2721f * i - 0.6474f * q;
  result.b = y - 1.1070f * i + 1.7046f * q;
  result.a = colorYIQ.alpha;
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ColorRGB RGBdtoRGB(const ColorRGBd& color)
{
  return ColorRGB((int)(color.r*255 + 0.5),(int)(color.g*255 + 0.5),(int)(color.b*255 + 0.5),(int)(color.a*255 + 0.5));
}

ColorRGBd RGBtoRGBd(const ColorRGB& color)
{
  return ColorRGBd(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
}

ColorRGB RGBftoRGB(const ColorRGBf& color)
{
  return ColorRGB((int)(color.r*255 + 0.5f),(int)(color.g*255 + 0.5f),(int)(color.b*255 + 0.5f),(int)(color.a*255 + 0.5f));
}

ColorRGBf RGBtoRGBf(const ColorRGB& color)
{
  return ColorRGBf(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

ColorRGBd RGBftoRGBd(const ColorRGBf& color)
{
  return ColorRGBd(color.r, color.g, color.b, color.a);
}

ColorRGBf RGBdtoRGBf(const ColorRGBd& color)
{
  return ColorRGBf(color.r, color.g, color.b, color.a);
}


} // namespace lpi
