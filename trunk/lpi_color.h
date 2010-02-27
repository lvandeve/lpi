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

#pragma once

namespace lpi
{

template<typename T>
struct TColorRGB255
{
  T r;
  T g;
  T b;
  T a;
  
  TColorRGB255(T r, T g, T b, T a)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
  }
  TColorRGB255(T r, T g, T b)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
  }
  
  TColorRGB255()
  {
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->a = 255;
  }
  
  void negateRGB()
  {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  
  void negateRGBA()
  {
    negateRGB();
    a = 255 - a;
  }
  
  void clamp()
  {
    if(r > 255) r = 255;
    if(r < 0) r = 0;
    if(g > 255) g = 255;
    if(g < 0) g = 0;
    if(b > 255) b = 255;
    if(b < 0) b = 0;
    if(a > 255) a = 255;
    if(a < 0) a = 0;
  }
  
};

template<typename T>
struct TColorRGB1
{
  T r;
  T g;
  T b;
  T a;
  
  TColorRGB1(T r, T g, T b, T a)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
  }
  TColorRGB1(T r, T g, T b)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 1.0;
  }
  
  TColorRGB1()
  {
    this->r = 0.0;
    this->g = 0.0;
    this->b = 0.0;
    this->a = 1.0;
  }
  
  void negateRGB()
  {
    r = 1.0 - r;
    g = 1.0 - g;
    b = 1.0 - b;
  }
  
  void negateRGBA()
  {
    negateRGB();
    a = 1.0 - a;
  }
  
  void clamp()
  {
    if(r > 1.0) r = 1.0;
    if(r < 0.0) r = 0.0;
    if(g > 1.0) g = 1.0;
    if(g < 0.0) g = 0.0;
    if(b > 1.0) b = 1.0;
    if(b < 0.0) b = 0.0;
  }
};

typedef TColorRGB255<int> ColorRGB;
typedef TColorRGB1<double> ColorRGBd;
typedef TColorRGB1<float> ColorRGBf;

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
//Compare
bool operator==(const ColorRGB& color, const ColorRGB& color2);
bool operator!=(const ColorRGB& color, const ColorRGB& color2);

//the + and - operator add/subtract the R G and B channels but not the alpha channel
ColorRGBd operator+(const ColorRGBd& color, const ColorRGBd& color2);
ColorRGBd operator-(const ColorRGBd& color, const ColorRGBd& color2);
//these multiplication operators with a constant don't affect the alpha channel
ColorRGBd operator*(const ColorRGBd& color, double a);
ColorRGBd operator*(double a, const ColorRGBd& color);
ColorRGBd operator/(const ColorRGBd& color, double a);
//this multiplies two colors, and the alpha channels too
ColorRGBd operator*(const ColorRGBd& color1, const ColorRGBd& color2);
//The | operator adds two color including their alpha channel
ColorRGBd operator|(const ColorRGBd& color, const ColorRGBd& color2);
//The & operator multiplies a color and also its alpha channel with a constant
ColorRGBd operator&(const ColorRGBd& color, double a);
ColorRGBd operator&(double a, const ColorRGBd& color);
//Compare
bool operator==(const ColorRGBd& color, const ColorRGBd& color2);
bool operator!=(const ColorRGBd& color, const ColorRGBd& color2);

//the + and - operator add/subtract the R G and B channels but not the alpha channel
ColorRGBf operator+(const ColorRGBf& color, const ColorRGBf& color2);
ColorRGBf operator-(const ColorRGBf& color, const ColorRGBf& color2);
//these multiplication operators with a constant don't affect the alpha channel
ColorRGBf operator*(const ColorRGBf& color, double a);
ColorRGBf operator*(double a, const ColorRGBf& color);
ColorRGBf operator/(const ColorRGBf& color, double a);
//this multiplies two colors, and the alpha channels too
ColorRGBf operator*(const ColorRGBf& color1, const ColorRGBf& color2);
//The | operator adds two color including their alpha channel
ColorRGBf operator|(const ColorRGBf& color, const ColorRGBf& color2);
//The & operator multiplies a color and also its alpha channel with a constant
ColorRGBf operator&(const ColorRGBf& color, double a);
ColorRGBf operator&(double a, const ColorRGBf& color);
//Compare
bool operator==(const ColorRGBf& color, const ColorRGBf& color2);
bool operator!=(const ColorRGBf& color, const ColorRGBf& color2);

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

static const ColorRGB RGB_Alpha(255, 255, 255, 192);
static const ColorRGB RGB_Translucent(255, 255, 255, 128);
static const ColorRGB RGB_Ghost(255, 255, 255, 64);
static const ColorRGB RGB_Invisible(  0,   0,   0,   0);

static const ColorRGBd RGBd_Black        (0.0, 0.0, 0.0, 1.0);
static const ColorRGBd RGBd_Red          (1.0, 0.0, 0.0, 1.0);
static const ColorRGBd RGBd_Green        (0.0, 1.0, 0.0, 1.0);
static const ColorRGBd RGBd_Blue         (0.0, 0.0, 1.0, 1.0);
static const ColorRGBd RGBd_Cyan         (0.0, 1.0, 1.0, 1.0);
static const ColorRGBd RGBd_Magenta      (1.0, 0.0, 1.0, 1.0);
static const ColorRGBd RGBd_Yellow       (1.0, 1.0, 0.0, 1.0);
static const ColorRGBd RGBd_White        (1.0, 1.0, 1.0, 1.0);
static const ColorRGBd RGBd_Gray         (0.5, 0.5, 0.5, 1.0);
static const ColorRGBd RGBd_Grey         (.75, .75, .75, 1.0);

static const ColorRGBf RGBf_Black        (0.0f, 0.0f, 0.0f, 1.0f);
static const ColorRGBf RGBf_Red          (1.0f, 0.0f, 0.0f, 1.0f);
static const ColorRGBf RGBf_Green        (0.0f, 1.0f, 0.0f, 1.0f);
static const ColorRGBf RGBf_Blue         (0.0f, 0.0f, 1.0f, 1.0f);
static const ColorRGBf RGBf_Cyan         (0.0f, 1.0f, 1.0f, 1.0f);
static const ColorRGBf RGBf_Magenta      (1.0f, 0.0f, 1.0f, 1.0f);
static const ColorRGBf RGBf_Yellow       (1.0f, 1.0f, 0.0f, 1.0f);
static const ColorRGBf RGBf_White        (1.0f, 1.0f, 1.0f, 1.0f);
static const ColorRGBf RGBf_Gray         (0.5f, 0.5f, 0.5f, 1.0f);
static const ColorRGBf RGBf_Grey         (.75f, .75f, .75f, 1.0f);


template<typename T>
struct TColorHSL255
{
  T h; //range 0-255 (not 0-360)
  T s;
  T l;
  T a;
  
  TColorHSL255(T h, T s, T l)
  {
    this->h = h;
    this->s = s;
    this->l = l;
    this->a = 255;
  }
  
  TColorHSL255(T h, T s, T l, T a)
  {
    this->h = h;
    this->s = s;
    this->l = l;
    this->a = a;
  }
  
  TColorHSL255()
  {
    this->h = 0;
    this->s = 0;
    this->l = 0;
    this->a = 255;
  }
};

template<typename T>
struct TColorHSL1
{
  T h; //range 0.0-1.0 (not 0-360)
  T s;
  T l;
  T a;
  
  TColorHSL1(T h, T s, T l)
  {
    this->h = h;
    this->s = s;
    this->l = l;
    this->a = 1.0;
  }
  
  TColorHSL1(T h, T s, T l, T a)
  {
    this->h = h;
    this->s = s;
    this->l = l;
    this->a = a;
  }
  
  TColorHSL1()
  {
    this->h = 0.0;
    this->s = 0.0;
    this->l = 0.0;
    this->a = 1.0;
  }
};

typedef TColorHSL255<int> ColorHSL;
typedef TColorHSL1<double> ColorHSLd;
typedef TColorHSL1<float> ColorHSLf;

template<typename T>
struct TColorHSV255
{
  T h; //range 0-255 (not 0-360)
  T s;
  T v;
  T a;
  
  TColorHSV255(T h, T s, T v)
  {
    this->h = h;
    this->s = s;
    this->v = v;
    this->a = 255;
  }
  
  TColorHSV255(T h, T s, T v, T a)
  {
    this->h = h;
    this->s = s;
    this->v = v;
    this->a = a;
  }
  
  TColorHSV255()
  {
    this->h = 0;
    this->s = 0;
    this->v = 0;
    this->a = 255;
  }
};

template<typename T>
struct TColorHSV1
{
  T h; //range 0.0-1.0 (not 0-360)
  T s;
  T v;
  T a;
  
  TColorHSV1(T h, T s, T v)
  {
    this->h = h;
    this->s = s;
    this->v = v;
    this->a = 1.0;
  }
  
  TColorHSV1(T h, T s, T v, T a)
  {
    this->h = h;
    this->s = s;
    this->v = v;
    this->a = a;
  }
  
  TColorHSV1()
  {
    this->h = 0.0;
    this->s = 0.0;
    this->v = 0.0;
    this->a = 1.0;
  }
};

typedef TColorHSV255<int> ColorHSV;
typedef TColorHSV1<double> ColorHSVd;
typedef TColorHSV1<float> ColorHSVf;

template<typename T>
struct TColorCIELab255 //NOTE: most colors here are in range 0 to 255, BUT, this one has range ~ -255 to ~ +255 for a and b (but l is still 0-255)
{
  T l; //range 0 - +255
  T a; //range -255 - +255
  T b; //range -255 - +255
  T alpha;
  
  TColorCIELab255(T l, T a, T b, T alpha)
  {
    this->l = l;
    this->a = a;
    this->b = b;
    this->alpha = alpha;
  }
  TColorCIELab255(T l, T a, T b)
  {
    this->l = l;
    this->a = a;
    this->b = b;
    this->alpha = 255;
  }
  
  TColorCIELab255()
  {
    this->l = 0;
    this->a = 0;
    this->b = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorCIELab1 //NOTE: most colors here are in range 0.0 to 1.0, BUT, this one has range ~ -1.0 to ~ +1.0 for a and b (but l is still 0.0-1.0)
{
  T l; //range 0.0 - 1.0
  T a; //range -1.0 - +1.0
  T b; //range -1.0 - +1.0
  T alpha;
  
  TColorCIELab1(T l, T a, T b, T alpha)
  {
    this->l = l;
    this->a = a;
    this->b = b;
    this->alpha = alpha;
  }
  TColorCIELab1(T l, T a, T b)
  {
    this->l = l;
    this->a = a;
    this->b = b;
    this->alpha = 1.0;
  }
  
  TColorCIELab1()
  {
    this->l = 0.0;
    this->a = 0.0;
    this->b = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorCIELab255<int> ColorCIELab;
typedef TColorCIELab1<double> ColorCIELabd;
typedef TColorCIELab1<float> ColorCIELabf;

template<typename T>
struct TColorCIEXYZ255
{
  T x;
  T y;
  T z;
  T alpha;
  
  TColorCIEXYZ255(T x, T y, T z, T alpha)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->alpha = alpha;
  }
  TColorCIEXYZ255(T x, T y, T z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->alpha = 255;
  }
  
  TColorCIEXYZ255()
  {
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorCIEXYZ1
{
  T x;
  T y;
  T z;
  T alpha;
  
  TColorCIEXYZ1(T x, T y, T z, T alpha)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->alpha = alpha;
  }
  TColorCIEXYZ1(T x, T y, T z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->alpha = 1.0;
  }
  
  TColorCIEXYZ1()
  {
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorCIEXYZ255<int> ColorCIEXYZ;
typedef TColorCIEXYZ1<double> ColorCIEXYZd;
typedef TColorCIEXYZ1<float> ColorCIEXYZf;

template<typename T>
struct TColorCMY255
{
  T c;
  T m;
  T y;
  T a;
  
  TColorCMY255(T c, T m, T y, T a)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->a = a;
  }
  TColorCMY255(T c, T m, T y)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->a = 255;
  }
  
  TColorCMY255()
  {
    this->c = 0;
    this->m = 0;
    this->y = 0;
    this->a = 255;
  }
};

template<typename T>
struct TColorCMY1
{
  T c;
  T m;
  T y;
  T a;
  
  TColorCMY1(T c, T m, T y, T a)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->a = a;
  }
  TColorCMY1(T c, T m, T y)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->a = 1.0;
  }
  
  TColorCMY1()
  {
    this->c = 0.0;
    this->m = 0.0;
    this->y = 0.0;
    this->a = 1.0;
  }
};

typedef TColorCMY255<int> ColorCMY;
typedef TColorCMY1<double> ColorCMYd;
typedef TColorCMY1<float> ColorCMYf;

template<typename T>
struct TColorCMYK255
{
  T c;
  T m;
  T y;
  T k;
  T a;
  
  TColorCMYK255(T c, T m, T y, T k, T a)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->k = k;
    this->a = a;
  }
  
  TColorCMYK255(T c, T m, T y, T k)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->k = k;
    this->a = 255;
  }
  
  TColorCMYK255()
  {
    this->c = 0;
    this->m = 0;
    this->y = 0;
    this->k = 0;
    this->a = 255;
  }
};

template<typename T>
struct TColorCMYK1
{
  T c;
  T m;
  T y;
  T k;
  T a;
  
  TColorCMYK1(T c, T m, T y, T k, T a)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->k = k;
    this->a = a;
  }
  
  TColorCMYK1(T c, T m, T y, T k)
  {
    this->c = c;
    this->m = m;
    this->y = y;
    this->k = k;
    this->a = 1.0;
  }
  
  TColorCMYK1()
  {
    this->c = 0.0;
    this->m = 0.0;
    this->y = 0.0;
    this->k = 0.0;
    this->a = 1.0;
  }
};

typedef TColorCMYK255<int> ColorCMYK;
typedef TColorCMYK1<double> ColorCMYKd;
typedef TColorCMYK1<float> ColorCMYKf;

template<typename T>
struct TColorYPbPr255 //NOTE: most colors here are in range 0 to 255, BUT, this one has range -128 to +128 for pb and pr (but y is still 0-255)
{
  T y; //range 0 - 255
  T pb; //range -128 - +128
  T pr; //range -128 - +128
  T alpha;
  
  TColorYPbPr255(T y, T pb, T pr, T alpha)
  {
    this->y = y;
    this->pb = pb;
    this->pr = pr;
    this->alpha = alpha;
  }
  TColorYPbPr255(T y, T pb, T pr)
  {
    this->y = y;
    this->pb = pb;
    this->pr = pr;
    this->alpha = 255;
  }
  
  TColorYPbPr255()
  {
    this->y = 0;
    this->pb = 0;
    this->pr = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorYPbPr1 //NOTE: most colors here are in range 0.0 to 1.0, BUT, this one has range -0.5 to +0.5 for pb and pr (but y is still 0.0-1.0)
{
  T y; //range 0.0 - +1.0
  T pb; //range -0.5-+0.5
  T pr; //range -0.5-+0.5
  T alpha;
  
  TColorYPbPr1(T y, T pb, T pr, T alpha)
  {
    this->y = y;
    this->pb = pb;
    this->pr = pr;
    this->alpha = alpha;
  }
  TColorYPbPr1(T y, T pb, T pr)
  {
    this->y = y;
    this->pb = pb;
    this->pr = pr;
    this->alpha = 1.0;
  }
  
  TColorYPbPr1()
  {
    this->y = 0.0;
    this->pb = 0.0;
    this->pr = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorYPbPr255<int> ColorYPbPr;
typedef TColorYPbPr1<double> ColorYPbPrd;
typedef TColorYPbPr1<float> ColorYPbPrf;

template<typename T>
struct TColorYCbCr255 //this presents JFIF JPEG's YCbCr
{
  T y;
  T cb;
  T cr;
  T alpha;
  
  TColorYCbCr255(T y, T cb, T cr, T alpha)
  {
    this->y = y;
    this->cb = cb;
    this->cr = cr;
    this->alpha = alpha;
  }
  TColorYCbCr255(T y, T cb, T cr)
  {
    this->y = y;
    this->cb = cb;
    this->cr = cr;
    this->alpha = 255;
  }
  
  TColorYCbCr255()
  {
    this->y = 0;
    this->cb = 0;
    this->cr = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorYCbCr1 //this presents JFIF JPEG's YCbCr
{
  T y;
  T cb;
  T cr;
  T alpha;
  
  TColorYCbCr1(T y, T cb, T cr, T alpha)
  {
    this->y = y;
    this->cb = cb;
    this->cr = cr;
    this->alpha = alpha;
  }
  TColorYCbCr1(T y, T cb, T cr)
  {
    this->y = y;
    this->cb = cb;
    this->cr = cr;
    this->alpha = 1.0;
  }
  
  TColorYCbCr1()
  {
    this->y = 0.0;
    this->cb = 0.0;
    this->cr = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorYCbCr255<int> ColorYCbCr;
typedef TColorYCbCr1<double> ColorYCbCrd;
typedef TColorYCbCr1<float> ColorYCbCrf;

template<typename T>
struct TColorYUV255
{
  T y; //range 0-255
  T u; //range -255 - +255 (even though the real YUV has -0.436 - +0.436)
  T v; //range -255 - +255 (even though the real YUV has -0.615 - +0.615)
  T alpha;
  
  TColorYUV255(T y, T u, T v, T alpha)
  {
    this->y = y;
    this->u = u;
    this->v = v;
    this->alpha = alpha;
  }
  TColorYUV255(T y, T u, T v)
  {
    this->y = y;
    this->u = u;
    this->v = v;
    this->alpha = 255;
  }
  
  TColorYUV255()
  {
    this->y = 0;
    this->u = 0;
    this->v = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorYUV1
{
  T y; //range 0.0-1.0
  T u; //range -1.0 - +1.0 (even though the real YUV has -0.436 - +0.436)
  T v; //range -1.0 - +1.0 (even though the real YUV has -0.615 - +0.615)
  T alpha;
  
  TColorYUV1(T y, T u, T v, T alpha)
  {
    this->y = y;
    this->u = u;
    this->v = v;
    this->alpha = alpha;
  }
  TColorYUV1(T y, T u, T v)
  {
    this->y = y;
    this->u = u;
    this->v = v;
    this->alpha = 1.0;
  }
  
  TColorYUV1()
  {
    this->y = 0.0;
    this->u = 0.0;
    this->v = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorYUV255<int> ColorYUV;
typedef TColorYUV1<double> ColorYUVd;
typedef TColorYUV1<float> ColorYUVf;

template<typename T>
struct TColorYIQ255
{
  T y; //range 0-255
  T i; //range -255 - +255 (even though the real YIQ has -0.5957 - +0.5957)
  T q; //range -255 - +255 (even though the real YIQ has -0.5226 - +0.5226)
  T alpha;
  
  TColorYIQ255(T y, T i, T q, T alpha)
  {
    this->y = y;
    this->i = i;
    this->q = q;
    this->alpha = alpha;
  }
  TColorYIQ255(T y, T i, T q)
  {
    this->y = y;
    this->i = i;
    this->q = q;
    this->alpha = 255;
  }
  
  TColorYIQ255()
  {
    this->y = 0;
    this->i = 0;
    this->q = 0;
    this->alpha = 255;
  }
};

template<typename T>
struct TColorYIQ1
{
  T y; //range 0.0-1.0
  T i; //range -1.0 - +1.0 (even though the real YIQ has -0.5957 - +0.5957)
  T q; //range -1.0 - +1.0 (even though the real YIQ has -0.5226 - +0.5226)
  T alpha;
  
  TColorYIQ1(T y, T i, T q, T alpha)
  {
    this->y = y;
    this->i = i;
    this->q = q;
    this->alpha = alpha;
  }
  TColorYIQ1(T y, T i, T q)
  {
    this->y = y;
    this->i = i;
    this->q = q;
    this->alpha = 1.0;
  }
  
  TColorYIQ1()
  {
    this->y = 0.0;
    this->i = 0.0;
    this->q = 0.0;
    this->alpha = 1.0;
  }
};

typedef TColorYIQ255<int> ColorYIQ;
typedef TColorYIQ1<double> ColorYIQd;
typedef TColorYIQ1<float> ColorYIQf;

//TODO: HunterLab
//TODO: make conversion function from CIELab to XYZ and vica versa, since it's already internally used by the CIELab to RGB and vica versa
//TODO: idem for CMYK to/from CMY
//TODO: transformations between different RGB models (and sometimes as preparation step for some models like YPbPr etc...)

ColorHSL RGBtoHSL(const ColorRGB& colorRGB);
ColorRGB HSLtoRGB(const ColorHSL& colorHSL);
ColorHSV RGBtoHSV(const ColorRGB& colorRGB);
ColorRGB HSVtoRGB(const ColorHSV& colorHSV);
ColorCMY RGBtoCMY(const ColorRGB& colorRGB);
ColorRGB CMYtoRGB(const ColorCMY& colorCMY);
ColorCMYK RGBtoCMYK(const ColorRGB& colorRGB);
ColorRGB CMYKtoRGB(const ColorCMYK& colorCMYK);
ColorCIEXYZ RGBtoCIEXYZ(const ColorRGB& colorRGB);
ColorRGB CIEXYZtoRGB(const ColorCIEXYZ& colorCIEXYZ);
ColorCIELab RGBtoCIELab(const ColorRGB& colorRGB);
ColorRGB CIELabtoRGB(const ColorCIELab& colorCIELab);
ColorYPbPr RGBtoYPbPr(const ColorRGB& colorRGB);
ColorRGB YPbPrtoRGB(const ColorYPbPr& colorYPbPr);
ColorYCbCr RGBtoYCbCr(const ColorRGB& colorRGB);
ColorRGB YCbCrtoRGB(const ColorYCbCr& colorYCbCr);
ColorYUV RGBtoYUV(const ColorRGB& colorRGB);
ColorRGB YUVtoRGB(const ColorYUV& colorYUV);
ColorYIQ RGBtoYIQ(const ColorRGB& colorRGB);
ColorRGB YIQtoRGB(const ColorYIQ& colorYIQ);

ColorHSLd RGBtoHSL(const ColorRGBd& colorRGB);
ColorRGBd HSLtoRGB(const ColorHSLd& colorHSL);
ColorHSVd RGBtoHSV(const ColorRGBd& colorRGB);
ColorRGBd HSVtoRGB(const ColorHSVd& colorHSV);
ColorCMYd RGBtoCMY(const ColorRGBd& colorRGB);
ColorRGBd CMYtoRGB(const ColorCMYd& colorCMY);
ColorCMYKd RGBtoCMYK(const ColorRGBd& colorRGB);
ColorRGBd CMYKtoRGB(const ColorCMYKd& colorCMYK);
ColorCIEXYZd RGBtoCIEXYZ(const ColorRGBd& colorRGB);
ColorRGBd CIEXYZtoRGB(const ColorCIEXYZd& colorCIEXYZ);
ColorCIELabd RGBtoCIELab(const ColorRGBd& colorRGB);
ColorRGBd CIELabtoRGB(const ColorCIELabd& colorCIELab);
ColorYPbPrd RGBtoYPbPr(const ColorRGBd& colorRGB);
ColorRGBd YPbPrtoRGB(const ColorYPbPrd& colorYPbPr);
ColorYCbCrd RGBtoYCbCr(const ColorRGBd& colorRGB);
ColorRGBd YCbCrtoRGB(const ColorYCbCrd& colorYCbCr);
ColorYUVd RGBtoYUV(const ColorRGBd& colorRGB);
ColorRGBd YUVtoRGB(const ColorYUVd& colorYUV);
ColorYIQd RGBtoYIQ(const ColorRGBd& colorRGB);
ColorRGBd YIQtoRGB(const ColorYIQd& colorYIQ);

ColorHSLf RGBtoHSL(const ColorRGBf& colorRGB);
ColorRGBf HSLtoRGB(const ColorHSLf& colorHSL);
ColorHSVf RGBtoHSV(const ColorRGBf& colorRGB);
ColorRGBf HSVtoRGB(const ColorHSVf& colorHSV);
ColorCMYf RGBtoCMY(const ColorRGBf& colorRGB);
ColorRGBf CMYtoRGB(const ColorCMYf& colorCMY);
ColorCMYKf RGBtoCMYK(const ColorRGBf& colorRGB);
ColorRGBf CMYKtoRGB(const ColorCMYKf& colorCMYK);
ColorCIEXYZf RGBtoCIEXYZ(const ColorRGBf& colorRGB);
ColorRGBf CIEXYZtoRGB(const ColorCIEXYZf& colorCIEXYZ);
ColorCIELabf RGBtoCIELab(const ColorRGBf& colorRGB);
ColorRGBf CIELabtoRGB(const ColorCIELabf& colorCIELab);
ColorYPbPrf RGBtoYPbPr(const ColorRGBf& colorRGB);
ColorRGBf YPbPrtoRGB(const ColorYPbPrf& colorYPbPr);
ColorYCbCrf RGBtoYCbCr(const ColorRGBf& colorRGB);
ColorRGBf YCbCrtoRGB(const ColorYCbCrf& colorYCbCr);
ColorYUVf RGBtoYUV(const ColorRGBf& colorRGB);
ColorRGBf YUVtoRGB(const ColorYUVf& colorYUV);
ColorYIQf RGBtoYIQ(const ColorRGBf& colorRGB);
ColorRGBf YIQtoRGB(const ColorYIQf& colorYIQ);

unsigned long RGBtoINT(const ColorRGB& colorRGB);
ColorRGB INTtoRGB(unsigned long colorINT);

ColorRGB RGBdtoRGB(const ColorRGBd& color);
ColorRGBd RGBtoRGBd(const ColorRGB& color);
ColorRGB RGBftoRGB(const ColorRGBf& color);
ColorRGBf RGBtoRGBf(const ColorRGB& color);
ColorRGBd RGBftoRGBd(const ColorRGBf& color);
ColorRGBf RGBdtoRGBf(const ColorRGBd& color);

}
