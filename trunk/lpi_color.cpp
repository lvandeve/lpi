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

namespace lpi
{



using namespace std;

////////////////////////////////////////////////////////////////////////////////
//COLOR STRUCTS/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ColorRGB::ColorRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}
ColorRGB::ColorRGB(unsigned char r, unsigned char g, unsigned char b)
{
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = 255;
}

ColorRGB::ColorRGB()
{
  this->r = 0;
  this->g = 0;
  this->b = 0;
  this->a = 255;
}


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
  c.r = (color1.r * color2.r) / 256;
  c.g = (color1.g * color2.g) / 256;
  c.b = (color1.b * color2.b) / 256;
  c.a = (color1.a * color2.a) / 256;
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

ColorHSL::ColorHSL(unsigned char h, unsigned char s, unsigned char l)
{
  this->h = h;
  this->s = s;
  this->l = l;
}
ColorHSL::ColorHSL()
{
  this->h = 0;
  this->s = 0;
  this->l = 0;
}
ColorHSV::ColorHSV(unsigned char h, unsigned char s, unsigned char v)
{
  this->h = h;
  this->s = s;
  this->v = v;
}
ColorHSV::ColorHSV()
{
  this->h = 0;
  this->s = 0;
  this->v = 0;
}

////////////////////////////////////////////////////////////////////////////////
//COLOR CONVERSIONS/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
These functions convert colors from one color model to another
r=red  g=green  b=blue  h=hue  s=saturation  l=lightness  v=value
Color components from the color structs are unsigned char's between 0 and 255
Color components used in the calculations are normalized between 0.0 and 1.0
*/

#if 0 //floating point version

//Converts an RGB color to HSL color
ColorHSL RGBtoHSL(const ColorRGB& colorRGB)
{
  float r, g, b, h, s, l; //this function works with floats between 0 and 1
  r = colorRGB.r / 256.0;
  g = colorRGB.g / 256.0;
  b = colorRGB.b / 256.0;

  float maxColor = std::max(r, std::max(g, b));
  float minColor = std::min(r, std::min(g, b));

  if(minColor == maxColor) //R = G = B, so it's a shade of grey
  {
    h = 0; //it doesn't matter what value it has
    s = 0;
    l = r; //doesn't matter if you pick r, g, or b
  }
  else
  {
    l = (minColor + maxColor) / 2;

    if(l < 0.5) s = (maxColor - minColor) / (maxColor + minColor);
    else s = (maxColor - minColor) / (2.0 - maxColor - minColor); //l >= 0.5

    if(r == maxColor) h = (g - b) / (maxColor - minColor);
    else if(g == maxColor) h = 2.0 + (b - r) / (maxColor - minColor);
    else h = 4.0 + (r - g) / (maxColor - minColor); //b == maxColor

    h /= 6; //to bring it to a number between 0 and 1
    if(h < 0) h += 1;
  }
  
  ColorHSL colorHSL;
  colorHSL.h = int(h * 256.0);
  colorHSL.s = int(s * 256.0);
  colorHSL.l = int(l * 256.0);
  return colorHSL;
}

//Converts an HSL color to RGB color
ColorRGB HSLtoRGB(const ColorHSL& colorHSL)
{
  float r, g, b, h, s, l; //this function works with floats between 0 and 1
  float temp1, temp2, tempr, tempg, tempb;
  h = colorHSL.h / 256.0;
  s = colorHSL.s / 256.0;
  l = colorHSL.l / 256.0;

  //If saturation is 0, the color is a shade of grey
  if(s == 0) r = g = b = l;
  //If saturation > 0, more complex calculations are needed
  else
  {
    //set the temporary values
    if(l < 0.5) temp2 = l * (1 + s);
    else temp2 = (l + s) - (l * s);
    temp1 = 2 * l - temp2;
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

  ColorRGB colorRGB;
  colorRGB.r = int(r * 256.0);
  colorRGB.g = int(g * 256.0);
  colorRGB.b = int(b * 256.0);
  return colorRGB;
}

//Converts an RGB color to HSV color
ColorHSV RGBtoHSV(const ColorRGB& colorRGB)
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  r = colorRGB.r / 256.0;
  g = colorRGB.g / 256.0;
  b = colorRGB.b / 256.0;

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

    h /= 6.0; //to bring it to a number between 0 and 1
    if(h < 0.0) h++;
  }

  ColorHSV colorHSV;
  colorHSV.h = int(h * 256.0);
  colorHSV.s = int(s * 256.0);
  colorHSV.v = int(v * 256.0);
  return colorHSV;
}

//Converts an HSV color to RGB color
ColorRGB HSVtoRGB(const ColorHSV& colorHSV)
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  h = colorHSV.h / 256.0;
  s = colorHSV.s / 256.0;
  v = colorHSV.v / 256.0;

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = v;
  //if saturation > 0, more complex calculations are needed
  else
  {
    float f, p, q, t;
    int i;
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = int(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

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
  ColorRGB colorRGB;
  colorRGB.r = int(r * 256.0);
  colorRGB.g = int(g * 256.0);
  colorRGB.b = int(b * 256.0);
  return colorRGB;
}

#else //pure integer version

//Converts an RGB color to HSL color
ColorHSL RGBtoHSL(const ColorRGB& colorRGB)
{
  ColorHSL colorHSL;

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

    if(colorHSL.l < 128) colorHSL.s = (255 * (maxColor - minColor)) / (maxColor + minColor);
    else colorHSL.s = (maxColor - minColor) / (512 - maxColor - minColor);

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

#endif //end of choice between floating point or integer version of color conversions

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

}
