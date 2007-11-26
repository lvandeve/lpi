/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_color.h"
#include <cmath>
#include "lpi_general.h"

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
