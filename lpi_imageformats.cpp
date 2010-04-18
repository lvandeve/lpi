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

#include "lpi_imageformats.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "lodepng.h"
#include "stb_image.h"

namespace lpi
{

ImageEncodeOptions::ImageEncodeOptions()
: dataBitsPerChannel(8)
, dataColorType(4)
, formatBitsPerChannel(8)
, formatColorType(4)
{
}

//numChannels should be 3 (RGB) or 4 (RGBA).
//Input buffer is in RGBA order (unlike BMP which is in BGRA order)
void encodeBMP(std::vector<unsigned char>& bmp, const unsigned char* image, int w, int h, int numChannels)
{
    //convert to bmp
    //save bmp part

    //bytes 0-13
    bmp.push_back('B'); bmp.push_back('M'); //0: bfType
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); //2: bfSize; size not yet known for now
    bmp.push_back(0); bmp.push_back(0); //6: bfReserved1
    bmp.push_back(0); bmp.push_back(0); //8: bfReserved2
    bmp.push_back(54 % 256); bmp.push_back(54 / 256); bmp.push_back(0); bmp.push_back(0); //10: bfOffBits (54 because headers are 54 bytes together, yes indeed its in bytes not in bits)

    //bytes 14-53
    bmp.push_back(40); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //14: biSize
    bmp.push_back(w % 256); bmp.push_back(w / 256); bmp.push_back(0); bmp.push_back(0); //18: biWidth
    bmp.push_back(h % 256); bmp.push_back(h / 256); bmp.push_back(0); bmp.push_back(0); //22: biHeight
    bmp.push_back(1); bmp.push_back(0); //26: biPlanes
    bmp.push_back(numChannels * 8); bmp.push_back(0); //28: biBitCount
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //30: biCompression
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //34: biSizeImage (may be zero because no compression used)
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //38: biXPelsPerMeter
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //42: biYPelsPerMeter
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //46: biClrUsed
    bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //50: biClrImportant

    int imagerowbytes = numChannels * w;
    imagerowbytes = imagerowbytes % 4 == 0 ? imagerowbytes : imagerowbytes + (4 - imagerowbytes % 4); //must be multiple of 4
    
    for(int y = h - 1; y >= 0; y--) //the rows are stored inversed in bmp
    {
      int c = 0;
      for(int x = 0; x < imagerowbytes; x++)
      {
        if(x < w * numChannels)
        {
          int shift = 0; if(c == 0) shift = 2; if(c == 2) shift = -2; //Convert RGB(A) into BGR(A)
          bmp.push_back(image[numChannels * w * y + x + shift]);
        }
        else bmp.push_back(0);
        c++;
        if(c >= numChannels) c = 0;
      }
    }

    bmp[2] = bmp.size() % 256;
    bmp[3] = (bmp.size() / 256) % 256;
    bmp[4] = (bmp.size() / 65536) % 256;
    bmp[5] = bmp.size() / 16777216;
}

//numChannels: 3 for RGB, 4 for RGBA (both input and output)
void encodeTGA(std::vector<unsigned char>& buffer, const unsigned char* image, int w, int h, int numChannels)
{
  buffer.push_back(0); //ID length
  buffer.push_back(0); //color map type (none)
  buffer.push_back(2); //image type (true color)
  
  buffer.push_back(0); buffer.push_back(0); //colormap offset (none)
  buffer.push_back(0); buffer.push_back(0); //colormap length (0)
  buffer.push_back(0); //color map bpp (n/a)
  
  buffer.push_back(0); buffer.push_back(0); //x offset (0)
  buffer.push_back(0); buffer.push_back(0); //y offset (0)
  buffer.push_back(w % 256); buffer.push_back((w / 256) % 256); // width
  buffer.push_back(h % 256); buffer.push_back((h / 256) % 256); // height
  buffer.push_back(numChannels * 8); //bits per pixel
  buffer.push_back((numChannels == 4 ? 8 : 0) | 32); //"has alpha" + top-to-bottom ordering
  
  size_t size = w * h * numChannels;
  for(size_t i = 0; i < size / 4; i++)
  {
    buffer.push_back(image[4 * i + 2]);
    buffer.push_back(image[4 * i + 1]);
    buffer.push_back(image[4 * i + 0]);
    buffer.push_back(image[4 * i + 3]);
  }
}


/*
 * Windows Bitmap File Loader
 * Version 1.2.1 (20070430), edited for using std::vector by Lode V. (20091125)
 *
 * Supported Formats: 1, 4, 8, 16, 24, 32 Bit Images
 * Alpha Bitmaps are also supported.
 * Supported compression types: RLE 8, BITFIELDS
 *
 * Created by: Benjamin Kalytta, 2006 - 2007
 *
 * Licence: Free to use
 * Source can be found at http://www.kalytta.com/bitmap.h
 */
class CBitmap
{
  private:
    
#define BITMAP_SIGNATURE "BM"

  typedef struct {
    char Signature[2];
    unsigned int Size;
    unsigned int Reserved;
    unsigned int BitsOffset;
  } BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

  typedef struct {
    unsigned int HeaderSize;
    int Width;
    int Height;
    unsigned short int Planes;
    unsigned short int BitCount;
    unsigned int Compression;
    unsigned int SizeImage;
    int PelsPerMeterX;
    int PelsPerMeterY;
    unsigned int ClrUsed;
    unsigned int ClrImportant;
    unsigned int RedMask;
    unsigned int GreenMask;
    unsigned int BlueMask;
    unsigned int AlphaMask;
    unsigned int CsType;
    unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
    unsigned int GammaRed;
    unsigned int GammaGreen;
    unsigned int GammaBlue;
  } BITMAP_HEADER;

  typedef struct {
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
    unsigned char Alpha;
  } RGBA;

  typedef struct {
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
    unsigned char Alpha;
  } BGRA;

  typedef struct {
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
  } BGR;

  typedef struct {
    unsigned short int Blue:5;
    unsigned short int Green:5;
    unsigned short int Red:5;
    unsigned short int Reserved:1;
  } BGR16;

#if 0

#define RIFF_SIGNATURE  0x46464952
#define RIFF_TYPE    0x204c4150
#define PAL_SIGNATURE  0x61746164
#define PAL_UNKNOWN    0x01000300

  typedef struct {
    unsigned int Signature;
    unsigned int FileLength;
    unsigned int Type;
    unsigned int PalSignature;
    unsigned int ChunkSize;
    unsigned int Unkown;
  } PAL;

#endif

    BITMAP_FILEHEADER m_BitmapFileHeader;
    BITMAP_HEADER m_BitmapHeader;
    RGBA *m_BitmapData;
    unsigned int m_BitmapSize;
    BGRA *m_ColorTable;
    unsigned int m_ColorTableSize;

  private:

    unsigned int ShiftRightByMask(unsigned int Color, unsigned int Mask, unsigned int DistributeToBits = 8) {

      if (Mask == 0) return 0;

      unsigned int ShiftCount = 0;
      unsigned int Test = 0x00000001;

      while (ShiftCount < 32) {
        if (Mask & Test) {
          break;
        }
        Test <<= 1;
        ShiftCount++;
      }

      unsigned int BitCount = 32;
      Test = 0x80000000;

      while (BitCount) {
        if ((Mask >> ShiftCount) & Test) {
          break;
        }
        Test >>= 1;
        BitCount--;
      }

      unsigned int BaseColor = (Color & Mask) >> ShiftCount;

      if (DistributeToBits > BitCount) {
        /* We have to fill lower bits */
        unsigned int BitsToFill = DistributeToBits - BitCount;
        while (BitsToFill--) {
          BaseColor <<= 1;
          if (BaseColor & 1) {
            BaseColor |= 1;
          }
        }
      } else if (DistributeToBits < BitCount) {
        BaseColor >>= (BitCount - DistributeToBits);
      }
      return BaseColor;
    }

  public:

    CBitmap() : m_BitmapData(0), m_BitmapSize(0), m_ColorTable(0), m_ColorTableSize(0) {
      Dispose();
    }

    ~CBitmap() {
      Dispose();
    }

    void Dispose() {
      if (m_BitmapData) delete[] m_BitmapData;
      if (m_ColorTable) delete[] m_ColorTable;
      m_ColorTableSize = 0;
      m_BitmapData = 0;
      m_ColorTable = 0;
    }

    /*
    Load specified Bitmap and stores it as RGBA in an internal buffer
    */
    bool Load(const unsigned char* data, size_t datasize)
    {
      size_t pos = 0;
      Dispose();
      
      m_BitmapFileHeader.Signature[0] = data[pos++];
      m_BitmapFileHeader.Signature[1] = data[pos++];
      m_BitmapFileHeader.Size = 0;
      m_BitmapFileHeader.Size += data[pos++];
      m_BitmapFileHeader.Size += 256 * data[pos++];
      m_BitmapFileHeader.Size += 256 * 256 * data[pos++];
      m_BitmapFileHeader.Size += 256 * 256 * 256 * data[pos++];
      m_BitmapFileHeader.Reserved = 0;
      m_BitmapFileHeader.Reserved += data[pos++];
      m_BitmapFileHeader.Reserved += 256 * data[pos++];
      m_BitmapFileHeader.Reserved += 256 * 256 * data[pos++];
      m_BitmapFileHeader.Reserved += 256 * 256 * 256 * data[pos++];
      m_BitmapFileHeader.BitsOffset = 0;
      m_BitmapFileHeader.BitsOffset += data[pos++];
      m_BitmapFileHeader.BitsOffset += 256 * data[pos++];
      m_BitmapFileHeader.BitsOffset += 256 * 256 * data[pos++];
      m_BitmapFileHeader.BitsOffset += 256 * 256 * 256 * data[pos++];

      if(m_BitmapFileHeader.Signature[0] != BITMAP_SIGNATURE[0] || m_BitmapFileHeader.Signature[1] != BITMAP_SIGNATURE[1])  return false;

      for(size_t i = 0; i < sizeof(BITMAP_HEADER) && pos < datasize; i++) ((unsigned char*)(&m_BitmapHeader))[i] = data[pos++];

      /* Load Color Table */

      if (m_BitmapHeader.BitCount == 1)  m_ColorTableSize = 2;
      else if (m_BitmapHeader.BitCount == 4)  m_ColorTableSize = 16;
      else if (m_BitmapHeader.BitCount == 8)  m_ColorTableSize = 256;
      if(m_BitmapHeader.ClrUsed != 0) m_ColorTableSize = m_BitmapHeader.ClrUsed;

      m_ColorTable = new BGRA[m_ColorTableSize];

      pos = BITMAP_FILEHEADER_SIZE + m_BitmapHeader.HeaderSize;
      for(size_t i = 0; i < sizeof(BGRA) * m_ColorTableSize; i++) ((unsigned char*)(m_ColorTable))[i] = data[pos++];
      /* ... Color Table for 16 or 32 Bit Images are not supported yet */
      m_BitmapSize = GetWidth() * GetHeight();
      m_BitmapData = new RGBA[m_BitmapSize];

      unsigned int LineWidth = ((GetWidth() * GetBitCount() / 8) + 3) & ~3;
      unsigned char *Line = new unsigned char[LineWidth];

      int Index = 0;
      pos = m_BitmapFileHeader.BitsOffset;
      if (m_BitmapHeader.Compression == 0)
      {
        for (size_t i = 0; i < GetHeight(); i++)
        {
          for(size_t k = 0; k < LineWidth; k++) Line[k] = data[pos++];

          unsigned char *LinePtr = Line;

          for (size_t j = 0; j < GetWidth(); j++) {
            if (m_BitmapHeader.BitCount == 1) {
              unsigned int Color = *((unsigned char*) LinePtr);
              for (int k = 0; k < 8; k++) {
                m_BitmapData[Index].Red = m_ColorTable[Color & 0x80 ? 1 : 0].Red;
                m_BitmapData[Index].Green = m_ColorTable[Color & 0x80 ? 1 : 0].Green;
                m_BitmapData[Index].Blue = m_ColorTable[Color & 0x80 ? 1 : 0].Blue;
                m_BitmapData[Index].Alpha = 255;//m_ColorTable[Color & 0x80 ? 1 : 0].Alpha;
                Index++;
                Color <<= 1;
              }
              LinePtr++;
              j += 7;
            } else if (m_BitmapHeader.BitCount == 4) {
              unsigned int Color = *((unsigned char*) LinePtr);
              m_BitmapData[Index].Red = m_ColorTable[(Color >> 4) & 0x0f].Red;
              m_BitmapData[Index].Green = m_ColorTable[(Color >> 4) & 0x0f].Green;
              m_BitmapData[Index].Blue = m_ColorTable[(Color >> 4) & 0x0f].Blue;
              m_BitmapData[Index].Alpha = 255;//m_ColorTable[(Color >> 4) & 0x0f].Alpha;
              Index++;
              m_BitmapData[Index].Red = m_ColorTable[Color & 0x0f].Red;
              m_BitmapData[Index].Green = m_ColorTable[Color & 0x0f].Green;
              m_BitmapData[Index].Blue = m_ColorTable[Color & 0x0f].Blue;
              m_BitmapData[Index].Alpha = 255;//m_ColorTable[Color & 0x0f].Alpha;
              Index++;
              LinePtr++;
              j++;
            } else if (m_BitmapHeader.BitCount == 8) {
              unsigned int Color = *((unsigned char*) LinePtr);
              m_BitmapData[Index].Red = m_ColorTable[Color].Red;
              m_BitmapData[Index].Green = m_ColorTable[Color].Green;
              m_BitmapData[Index].Blue = m_ColorTable[Color].Blue;
              //it doesn't appear to be the intention of Windows BMP to actually use the alpha channel of the palette, because the palette of standard Windows XP BMP's contains 0's here.
              m_BitmapData[Index].Alpha = 255;//m_ColorTable[Color].Alpha;
              Index++;
              LinePtr++;
            } else if (m_BitmapHeader.BitCount == 16) {
              unsigned int Color = *((unsigned short int*) LinePtr);
              m_BitmapData[Index].Red = ((Color >> 10) & 0x1f) << 3;
              m_BitmapData[Index].Green = ((Color >> 5) & 0x1f) << 3;
              m_BitmapData[Index].Blue = (Color & 0x1f) << 3;
              m_BitmapData[Index].Alpha = 255;
              Index++;
              LinePtr += 2;
            } else if (m_BitmapHeader.BitCount == 24) {
              unsigned int Color = *((unsigned int*) LinePtr);
              m_BitmapData[Index].Blue = Color & 0xff;
              m_BitmapData[Index].Green = (Color >> 8) & 0xff;
              m_BitmapData[Index].Red = (Color >> 16) & 0xff;
              m_BitmapData[Index].Alpha = 255;
              Index++;
              LinePtr += 3;
            } else if (m_BitmapHeader.BitCount == 32) {
              unsigned int Color = *((unsigned int*) LinePtr);
              m_BitmapData[Index].Blue = Color & 0xff;
              m_BitmapData[Index].Green = (Color >> 8) & 0xff;
              m_BitmapData[Index].Red = (Color >> 16) & 0xff;
              m_BitmapData[Index].Alpha = Color >> 24;
              Index++;
              LinePtr += 4;
            }
          }
        }
      } else if (m_BitmapHeader.Compression == 1) { // RLE 8
        unsigned char Count = 0;
        unsigned char ColorIndex = 0;
        int x = 0, y = 0;

        while (pos < datasize) {
          Count = data[pos++];
          ColorIndex = data[pos++];

          if (Count > 0) {
            Index = x + y * GetWidth();
            for (int k = 0; k < Count; k++) {
              m_BitmapData[Index + k].Red = m_ColorTable[ColorIndex].Red;
              m_BitmapData[Index + k].Green = m_ColorTable[ColorIndex].Green;
              m_BitmapData[Index + k].Blue = m_ColorTable[ColorIndex].Blue;
              m_BitmapData[Index + k].Alpha = 255;//m_ColorTable[ColorIndex].Alpha;
            }
            x += Count;
          } else if (Count == 0) {
            int Flag = ColorIndex;
            if (Flag == 0) {
              x = 0;
              y++;
            } else if (Flag == 1) {
              break;
            } else if (Flag == 2) {
              char rx = (char)(data[pos++]);
              char ry = (char)(data[pos++]);
              x += rx;
              y += ry;
            } else {
              Count = Flag;
              Index = x + y * GetWidth();
              for (int k = 0; k < Count; k++) {
                ColorIndex = data[pos++];
                m_BitmapData[Index + k].Red = m_ColorTable[ColorIndex].Red;
                m_BitmapData[Index + k].Green = m_ColorTable[ColorIndex].Green;
                m_BitmapData[Index + k].Blue = m_ColorTable[ColorIndex].Blue;
                m_BitmapData[Index + k].Alpha = 255;//m_ColorTable[ColorIndex].Alpha;
              }
              x += Count;
              if(pos & 1) pos++;
            }
          }
        }
      } else if (m_BitmapHeader.Compression == 2) { // RLE 4
        /* RLE 4 is not supported */
      } else if (m_BitmapHeader.Compression == 3) { // BITFIELDS

        /* We assumes that mask of each color component can be in any order */

        for (size_t i = 0; i < GetHeight(); i++) {
          for(size_t k = 0; k < LineWidth; k++) Line[k] = data[pos++];

          unsigned char *LinePtr = Line;

          for (size_t j = 0; j < GetWidth(); j++) {

            unsigned int Color = 0;

            if (m_BitmapHeader.BitCount == 16)
            {
              Color = *((unsigned short int*) LinePtr);
              LinePtr += 2;
            }
            else if (m_BitmapHeader.BitCount == 32)
            {
              Color = *((unsigned int*) LinePtr);
              LinePtr += 4;
            }
            m_BitmapData[Index].Red = ShiftRightByMask(Color, m_BitmapHeader.RedMask);
            m_BitmapData[Index].Green = ShiftRightByMask(Color, m_BitmapHeader.GreenMask);
            m_BitmapData[Index].Blue = ShiftRightByMask(Color, m_BitmapHeader.BlueMask);
            m_BitmapData[Index].Alpha = ShiftRightByMask(Color, m_BitmapHeader.AlphaMask);

            Index++;
          }
        }
      }
      return true;
    }

    void saveBitmapFileHeader(std::vector<unsigned char>& file, const BITMAP_FILEHEADER& bfh)
    {
      file.push_back(bfh.Signature[0]);
      file.push_back(bfh.Signature[1]);
      file.push_back(bfh.Size % 256);
      file.push_back((bfh.Size / 256) % 256);
      file.push_back((bfh.Size / (256*256)) % 256);
      file.push_back((bfh.Size / (256*256*256)) % 256);
      file.push_back(bfh.Reserved % 256);
      file.push_back((bfh.Reserved / 256) % 256);
      file.push_back((bfh.Reserved / (256*256)) % 256);
      file.push_back((bfh.Reserved / (256*256*256)) % 256);
      file.push_back(bfh.BitsOffset % 256);
      file.push_back((bfh.BitsOffset / 256) % 256);
      file.push_back((bfh.BitsOffset / (256*256)) % 256);
      file.push_back((bfh.BitsOffset / (256*256*256)) % 256);
    }

    void saveBitmapHeader(std::vector<unsigned char>& file, const BITMAP_HEADER& bh)
    {
      for(size_t i = 0; i < sizeof(BITMAP_HEADER); i++) file.push_back(((unsigned char*)(&bh))[i]);
    }

    //WARNING! I think I broke this function when converting to std::vector, the produced BMP file is not recognised by programs. But I now use "encodeBMP" function above instead of this class.
    bool Save(std::vector<unsigned char>& file, unsigned int BitCount = 32)
    {
      (void)BitCount;


      BITMAP_FILEHEADER bfh;
      BITMAP_HEADER bh;

      bfh.Signature[0] = BITMAP_SIGNATURE[0];
      bfh.Signature[1] = BITMAP_SIGNATURE[1];
      bfh.BitsOffset = sizeof(BITMAP_HEADER) + sizeof(BITMAP_FILEHEADER);
      bfh.Size = (GetWidth() * GetHeight() * BitCount) / 8 + bfh.BitsOffset;

      bh.HeaderSize = sizeof(BITMAP_HEADER);
      bh.BitCount = BitCount;

      if (BitCount == 32) {
        bh.Compression = 3; // BITFIELD
        bh.AlphaMask = 0xff000000;
        bh.BlueMask = 0x00ff0000;
        bh.GreenMask = 0x0000ff00;
        bh.RedMask = 0x000000ff;
      } else {
        bh.Compression = 0; // RGB
      }

      bh.Planes = 1;
      bh.Height = GetHeight();
      bh.Width = GetWidth();
      bh.SizeImage = (GetWidth() * GetHeight() * BitCount) / 8;
      bh.PelsPerMeterX = 3780;
      bh.PelsPerMeterY = 3780;

      if (BitCount == 32)
      {
        saveBitmapFileHeader(file, bfh);
        saveBitmapHeader(file, bh);
        for(size_t i = 0; i < bh.SizeImage; i++) file.push_back(((unsigned char*)(m_BitmapData))[i]);
      }
      else if (BitCount < 16)
      {
        unsigned char* Bitmap = new unsigned char[bh.SizeImage];

        BGRA *Palette = 0;
        unsigned int PaletteSize = 0;

        if (GetBitsWithPalette(Bitmap, bh.SizeImage, BitCount, Palette, PaletteSize)) {
          bfh.BitsOffset += PaletteSize * sizeof(BGRA);

          saveBitmapFileHeader(file, bfh);
          saveBitmapHeader(file, bh);
          for(size_t i = 0; i < PaletteSize * sizeof(BGRA); i++) file.push_back(((unsigned char*)(&Palette))[i]);
          for(size_t i = 0; i < bh.SizeImage; i++) file.push_back(((unsigned char*)(m_BitmapData))[i]);
        }
        delete [] Bitmap;
        delete [] Palette;
      }
      else
      {
        unsigned char* Bitmap = new unsigned char[bh.SizeImage];

        if (GetBits(Bitmap, bh.SizeImage, BitCount)) {
          saveBitmapFileHeader(file, bfh);
          saveBitmapHeader(file, bh);
          for(size_t i = 0; i < bh.SizeImage; i++) file.push_back(((unsigned char*)(m_BitmapData))[i]);
        }
        delete [] Bitmap;
      }

      return true;
    }

    unsigned int GetWidth() {
      return m_BitmapHeader.Width < 0 ? -m_BitmapHeader.Width : m_BitmapHeader.Width;
    }

    unsigned int GetHeight() {
      return m_BitmapHeader.Height < 0 ? -m_BitmapHeader.Height : m_BitmapHeader.Height;
    }

    bool isUpsideDown() { return m_BitmapHeader.Height > 0; }

    unsigned int GetBitCount() {
      return m_BitmapHeader.BitCount;
    }

    /* Copies internal RGBA buffer to user specified buffer */

    bool GetBits(void* Buffer, unsigned int& Size) {
      bool Result = false;
      if (Size == 0 || Buffer == 0) {
        Size = m_BitmapSize * sizeof(RGBA);
        Result = m_BitmapSize != 0;
      } else {
        for(size_t i = 0; i < Size; i++) ((unsigned char*)Buffer)[i] = ((unsigned char*)m_BitmapData)[i];
        Result = true;
      }
      return Result;
    }

    /* Returns internal RGBA buffer */

    void* GetBits() {
      return m_BitmapData;
    }

    /* Copies internal RGBA buffer to user specified buffer and convertes into destination bit format.
     * Supported Bit depths are: 16 (5-5-5), 24, 32
     */
    bool GetBits(void* Buffer, unsigned int& Size, unsigned int BitCount) {
      bool Result = false;
      if (Size == 0 || Buffer == 0) {
        Size = (m_BitmapSize * BitCount) / 8;
        return true;
      }

      if (BitCount > 32) {
        return false;
      }

      unsigned char* BufferPtr = (unsigned char*) Buffer;

      for (size_t i = 0; i < m_BitmapSize; i++) {
        if (BitCount == 16) {
          ((BGR16*) BufferPtr)->Blue = ShiftRightByMask(m_BitmapData[i].Blue, 0xff, 5);
          ((BGR16*) BufferPtr)->Green = ShiftRightByMask(m_BitmapData[i].Green, 0xff, 5);
          ((BGR16*) BufferPtr)->Red = ShiftRightByMask(m_BitmapData[i].Red, 0xff, 5);
          BufferPtr += 2;
        } else if (BitCount == 24) {
          ((BGR*) BufferPtr)->Blue = m_BitmapData[i].Blue;
          ((BGR*) BufferPtr)->Green = m_BitmapData[i].Green;
          ((BGR*) BufferPtr)->Red = m_BitmapData[i].Red;
          BufferPtr += 3;
        } else if (BitCount == 32) {
          ((BGRA*) BufferPtr)->Blue = m_BitmapData[i].Blue;
          ((BGRA*) BufferPtr)->Green = m_BitmapData[i].Green;
          ((BGRA*) BufferPtr)->Red = m_BitmapData[i].Red;
          ((BGRA*) BufferPtr)->Alpha = m_BitmapData[i].Alpha;
          BufferPtr += 4;
        }
      }

      Result = true;

      return Result;
    }

    /* See GetBits().
     * It creates a corresponding color table (palette) which have to be destroyed by the user after usage.
     * Supported Bit depths are: 4, 8
     * Todo: Optimize, use optimized palette, do ditehring
     */

    bool GetBitsWithPalette(void* Buffer, unsigned int &Size, unsigned int BitCount, BGRA* &Palette, unsigned int &PaletteSize, bool OptimalPalette = false)
    {
      (void)OptimalPalette;
      bool Result = false;

      if (BitCount > 16) {
        return false;
      }

      if (Size == 0 || Buffer == 0) {
        Size = (m_BitmapSize * BitCount) / 8;
        return true;
      }

      if (BitCount == 4) {
        PaletteSize = 16;
        Palette = new BGRA[PaletteSize];
        for (int r = 0; r < 4; r++) {
          for (int g = 0; g < 2; g++) {
            for (int b = 0; b < 2; b++) {
              Palette[r | g << 2 | b << 3].Red = (r << 6) | 0x35;
              Palette[r | g << 2 | b << 3].Green = (g << 7) | 0x55;
              Palette[r | g << 2 | b << 3].Blue = (b << 7) | 0x55;
              Palette[r | g << 2 | b << 3].Alpha = 0xff;
            }
          }
        }
      } else if (BitCount == 8) {
        PaletteSize = 256;
        Palette = new BGRA[PaletteSize];
        for (int r = 0; r < 8; r++) {
          for (int g = 0; g < 8; g++) {
            for (int b = 0; b < 4; b++) {
              Palette[r | g << 3 | b << 6].Red = (r << 5) | 0x15;
              Palette[r | g << 3 | b << 6].Green = (g << 5) | 0x15;
              Palette[r | g << 3 | b << 6].Blue = (b << 6) | 0x35;
              Palette[r | g << 3 | b << 6].Alpha = 0xff;
            }
          }
        }
      }

      unsigned char* BufferPtr = (unsigned char*) Buffer;

      for (size_t i = 0; i < m_BitmapSize; i++) {
        if (BitCount == 4) {
          *BufferPtr = (m_BitmapData[i].Red >> 6) | (m_BitmapData[i].Green >> 7) << 2 | (m_BitmapData[i].Blue >> 7) << 3;
          i++;
          *BufferPtr |= ((m_BitmapData[i].Red >> 6) | (m_BitmapData[i].Green >> 7) << 2 | (m_BitmapData[i].Blue >> 7) << 3) << 4;
          BufferPtr++;
        } else if (BitCount == 8) {
          *BufferPtr = (m_BitmapData[i].Red >> 5) | (m_BitmapData[i].Green >> 5) << 3 | (m_BitmapData[i].Blue >> 5) << 6;
          BufferPtr++;
        }
      }

      Result = true;

      return Result;
    }

    /* Set Bitmap Bits. Will be converted to RGBA internally */
    void SetBits(const void* Buffer, unsigned int Width, unsigned int Height, unsigned int RedMask, unsigned int GreenMask, unsigned int BlueMask, unsigned int AlphaMask = 0) {
      const unsigned char *BufferPtr = (const unsigned char*) Buffer;

      Dispose();

      m_BitmapHeader.Width = Width;
      m_BitmapHeader.Height = Height;
      m_BitmapHeader.BitCount = 32;
      m_BitmapHeader.Compression = 3;

      m_BitmapSize = GetWidth() * GetHeight();
      m_BitmapData = new RGBA[m_BitmapSize];

      /* Find BitCount by Mask, maximum is 32 Bit */

      unsigned int Test = 0x80000000;
      unsigned int BitCount = 32;

      while (BitCount) {
        if ((RedMask | GreenMask | BlueMask | AlphaMask) & Test) {
          break;
        }
        Test >>= 1;
        BitCount--;
      }
      for (size_t i = 0; i < m_BitmapSize; i++) {
        unsigned int Color = 0;
        if (BitCount <= 8) {
          Color = *((const unsigned char*) BufferPtr);
          BufferPtr += 1;
        } else if (BitCount <= 16) {
          Color = *((const unsigned short int*) BufferPtr);
          BufferPtr += 2;
        } else if (BitCount <= 24) {
          Color = *((const unsigned int*) BufferPtr);
          BufferPtr += 3;
        } else if (BitCount <= 32) {
          Color = *((const unsigned int*) BufferPtr);
          BufferPtr += 4;
        } else {
          /* unsupported */
          BufferPtr += 1;
        }
        m_BitmapData[i].Alpha = ShiftRightByMask(Color, AlphaMask);
        m_BitmapData[i].Red = ShiftRightByMask(Color, RedMask);
        m_BitmapData[i].Green = ShiftRightByMask(Color, GreenMask);
        m_BitmapData[i].Blue = ShiftRightByMask(Color, BlueMask);
      }
    }

    void SetAlphaBits(unsigned char Alpha) {
      for (size_t i = 0; i < m_BitmapSize; i++) {
        m_BitmapData[i].Alpha = Alpha;
      }
    }

};

bool decodeImageFile(std::string& error, std::vector<unsigned char>& image, int& w, int& h, const unsigned char* data, size_t datasize, ImageFormat format)
{
  if(format == IF_BMP)
  {
    CBitmap bitmap;
    if(!bitmap.Load(data, datasize))
    {
      error = "Error decoding BMP image";
      return false;
    }
    w = bitmap.GetWidth();
    h = bitmap.GetHeight();
    image.resize(w * h * 4);
    unsigned int datasizeuint = datasize;
    bitmap.GetBits(&image[0], datasizeuint, 32);
    if(bitmap.isUpsideDown())
    {
      for(int y = 0; y < h / 2; y++)
      for(int x = 0; x < w; x++)
      for(int c = 0; c < 4; c++)
      {
        size_t index = w * y * 4 + x * 4 + c;
        size_t index2 = w * (h - y - 1) * 4 + x * 4 + c;
        std::swap(image[index], image[index2]);
      }
    }
    //swap BGR order to RGB
    for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++)
    {
      size_t index = w * y * 4 + x * 4 ;
      std::swap(image[index], image[index + 2]);
    }

    return true;
  }
  else if(format == IF_PNG)
  {
    LodePNG::Decoder png_decoder;
    png_decoder.decode(image, data, datasize); //decode the image from PNG
    if(png_decoder.hasError())
    {
      std::stringstream ss;
      ss << "Error decoding PNG image because: \nLodePNG error number " << png_decoder.getError();
      error = ss.str();
      return false;
    }
    else
    {
      w = png_decoder.getWidth();
      h = png_decoder.getHeight();
      return true;
    }
  }
  else if(format == IF_JPG)
  {
    int comp; //number of components in source image (not needed by us)
    unsigned char* img = stbi_jpeg_load_from_memory(data, datasize, &w, &h, &comp, 4);
    if(img != 0)
    {
      image.resize(w * h * 4);
      for(int i = 0; i < w * h * 4; i++) image[i] = img[i];
      stbi_image_free(img);
      return true;
    }
    else
    {
      std::string failure_reason = stbi_failure_reason();
      error = "Error decoding JPEG image because: \n" + failure_reason;
      return false;
    }
  }
  else if(format == IF_TGA)
  {
    int comp; //number of components in source image (not needed by us)
    unsigned char* img = stbi_tga_load_from_memory(data, datasize, &w, &h, &comp, 4);
    if(img != 0)
    {
      image.resize(w * h * 4);
      for(int i = 0; i < w * h * 4; i++) image[i] = img[i];
      stbi_image_free(img);
      return true;
    }
    else
    {
      std::string failure_reason = stbi_failure_reason();
      error = "Error decoding TGA image because: \n" + failure_reason;
      return false;
    }
  }
  else
  {
    error = "Error decoding image because: \nUnknown image format";
    return false;
  }

  return false;
}

bool encodeImageFile(std::string& error, std::vector<unsigned char>& file, const unsigned char* image, int w, int h, ImageFormat format, const ImageEncodeOptions* p_options)
{
  ImageEncodeOptions options;
  if(p_options) options = *p_options;
  
  if(options.dataBitsPerChannel != 8
  || (options.dataColorType != 1 && options.dataColorType != 4)
  || options.formatBitsPerChannel != 8
  || (options.formatColorType != 1 && options.formatColorType != 4)
  || options.dataColorType != options.formatColorType
  || options.dataBitsPerChannel != options.formatBitsPerChannel)
  {
    error = "Unsupported encoding options";
    return false;
  }

  
  if(format == IF_BMP)
  {
    encodeBMP(file, image, w, h, options.dataColorType);
    return true;
  }
  if(format == IF_TGA)
  {
    encodeTGA(file, image, w, h, options.dataColorType);
    return true;
  }
  else if(format == IF_PNG)
  {
    LodePNG::Encoder png_encoder;
    if(options.dataColorType == 1)
    {
      png_encoder.getInfoPng().color.colorType = 0;
      png_encoder.getInfoRaw().color.colorType = 0;
    }
    png_encoder.encode(file, image, w, h);
    if(png_encoder.hasError())
    {
      std::stringstream ss;
      ss << "Error saving image: LodePNG error " << png_encoder.getError();
      error = ss.str();
      return false;
    }
    return true;
  }
  else
  {
    error = "Unknown image format for encoding";
    return false;
  }
}

bool supportsImageDecode(ImageFormat format)
{
  switch(format)
  {
    case IF_BMP:
      return true;
    case IF_PNG:
      return true;
    case IF_TGA:
      return true;
    case IF_JPG:
      return true;
    default:
      return false;
  }
}

bool supportsImageEncode(ImageFormat format)
{
  switch(format)
  {
    case IF_BMP:
      return true;
    case IF_PNG:
      return true;
    case IF_TGA:
      return true;
    default:
      return false;
  }
}

bool isOfFormat(const unsigned char* file, size_t filesize, ImageFormat format)
{
  if(format == IF_BMP)
  {
    //too small to have the smallest possible header
    if(filesize < 26) return false;
    //'BM' at the beginning
    if(file[0] != 'B' || file[1] != 'M') return false;
    //size of the header (40 bytes)
    if((int)file[15] != 0 || (int)file[16] != 0 || (int)file[17] != 0) return false; //the last 3 bytes of header size
    int headersize = (int)file[14]; //the first byte of headersize contains the actual value
    if(!(headersize == 12 || headersize == 40 || headersize == 56 || headersize == 108)) return false;
    return true;
  }
  else if(format == IF_PNG)
  {
    LodePNG::Decoder pngtest;
    pngtest.inspect(file, filesize);
    return !pngtest.hasError();
  }
  else if(format == IF_JPG)
  {
    int i = stbi_jpeg_test_memory(file, filesize);
    return i != 0;
  }
  else if(format == IF_TGA)
  {
    int i = stbi_tga_test_memory(file, filesize);
    return i != 0;
  }
  else
  {
    return false;
  }
}

ImageFormat findImageFormat(const unsigned char* file, size_t filesize)
{
  if(isOfFormat(file, filesize, IF_BMP)) return IF_BMP;
  else if(isOfFormat(file, filesize, IF_PNG)) return IF_PNG;
  else if(isOfFormat(file, filesize, IF_TGA)) return IF_TGA;
  else if(isOfFormat(file, filesize, IF_JPG)) return IF_JPG;
  else return IF_INVALID;
}

} // namespace lpi
