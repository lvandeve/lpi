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

#include "lpi_text_drawer_gl.h"


namespace lpi
{

TextDrawerGL::TextDrawerGL(const TextureFactoryGL& factory, Drawer2DGL* drawer)
: drawer(drawer)
, glyphs(&factory, true)
{
}

void TextDrawerGL::drawText(const std::string& text, int x, int y, const Font& font, const TextAlign& align)
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


namespace
{
void drawFontTexture(int n, int x, int y, TextureGL* texture)
{
  int u = texture->getU() / 16;
  int v = texture->getV() / 16;
  
  float u3 = (float)u / (float)texture->getU2();
  float v3 = (float)v / (float)texture->getV2();
  
  float u0 = (n % 16) * u3;
  float v0 = (n / 16) * v3;
  float u1 = (n % 16 + 1) * u3;
  float v1 = (n / 16 + 1) * v3;

  //note how in the texture coordinates x and y are swapped because the texture buffers are 90 degrees rotated
  glTexCoord2f(u0, v0); glVertex2i(x + 0, y + 0);
  glTexCoord2f(u1, v0); glVertex2i(x + u, y + 0);
  glTexCoord2f(u1, v1); glVertex2i(x + u, y + v);
  glTexCoord2f(u0, v1); glVertex2i(x + 0, y + v);
  
}

//glBegin(GL_QUADS) must already have been called before using this function, as well as binding the texture
void renderText(TextureGL* texturegl
              , const std::string& text, int x, int y
              , int sw, int sh, const ColorRGB& color
              , unsigned long forceLength)
{

  //TODO: use VBO's

  glColor4ub(color.r, color.g, color.b, color.a);
  
  unsigned long pos = 0;
  int drawX = x;
  int drawY = y;
  int symbol;
  while((pos < text.length() && forceLength == 0) || (pos < forceLength && pos < text.length() && forceLength > 0))
  {
    symbol = text[pos];
    
    if(symbol == 10)
    {
      drawX = x;
      drawY += sh;
    }
    else if(symbol == 13) ; //ignore
    else if(symbol == 32)
    {
      drawX += sw; //don't draw since it's invisible texture anyway
    }
    else
    {
      drawFontTexture(text[pos], drawX, drawY, texturegl);
      drawX += sw;
    }
    pos++;
  }
}
} //namespace

const InternalGlyphs::Glyphs* TextDrawerGL::getGlyphsForFont(const Font& font) const
{
  if(font.typeface == "lpi8") return &glyphs.glyphs8x8;
  else if(font.typeface == "lpi7") return &glyphs.glyphs7x9;
  else if(font.typeface == "lpi6") return &glyphs.glyphs6x6;
  else if(font.typeface == "lpi4") return &glyphs.glyphs4x5;
  else return &glyphs.glyphs8x8;
}

//Draws a string of text, and uses some of the ascii control characters, e.g. newline
void TextDrawerGL::printText(const std::string& text, int x, int y, const Font& font, unsigned long forceLength)
{
  const InternalGlyphs::Glyphs* glyphs = getGlyphsForFont(font);
  TextureGL* texturegl = dynamic_cast<TextureGL*>(glyphs->texture[0]);
  
  drawer->prepareDrawTextured();
  texturegl->updateForNewOpenGLContextIfNeeded();
  texturegl->bind(false, 0);
  
  glBegin(GL_QUADS);
  
  if(font.shadow)
  {
    renderText(texturegl, text, x + 1, y + 1, glyphs->width, glyphs->height, font.shadowColor, forceLength);
  }
  
  renderText(texturegl, text, x, y, glyphs->width, glyphs->height, font.color, forceLength);
  
  if(font.bold) //bold
  {
    renderText(texturegl, text, x + 1, y, glyphs->width, glyphs->height, font.color, forceLength);
  }
  
  glEnd();
}

void TextDrawerGL::calcTextRectSize(int& w, int& h, const std::string& text, const Font& font) const
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

size_t TextDrawerGL::calcTextPosToChar(int x, int y, const std::string& text, const Font& font, const TextAlign& align) const
{
  (void)x;(void)y;(void)font;(void)text;(void)align;
  //TODO!
  return 0;
}

void TextDrawerGL::calcTextCharToPos(int& x, int& y, size_t index, const std::string& text, const Font& font, const TextAlign& align) const
{
  (void)x;(void)y;(void)font;(void)text;(void)align;(void)index;
  //TODO!
}


} // namespace lpi

