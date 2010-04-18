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

#pragma once

#include <vector>
#include <string>

/*
This code tries to combine various sources to allow loading and saving of various image formats.
But there are restrictions to what kind of sources are supported:
  -no sources that require external dependencies (such as zlib or libjpeg)
  -no sources that require lpi to add or change its license or acknowledgements
  -only C and C++ code
  

Currently the sources are:
  -LodePNG (for PNG r/w)
  -stb_image.c from nothings.org (for JPEG r, TGA r, ...)
  -bitmap.h from www.kalytta.com, heavily modified (for BMP r)
  -extra code in lpi_imageformats.cpp (for BMP w, TGA w)
  
The best of the sources is combined, e.g. stb_image.c and LodePNG both support
PNG, but LodePNG supports interlaced PNGs and both loading and saving of PNGs,
so for PNG LodePNG is used, but on the other hand stb_image.c supports loading
of JPEGs so for that stb_image.c is used.

All encoding and decoding is done through memory (with std::vector<unsigned char>),
not with a FILE.
*/

namespace lpi
{

enum ImageFormat
{
  IF_PNG,
  IF_BMP,
  IF_TGA,
  IF_JPG,

  IF_INVALID //use if unknown
};

//these options are followed only loosely by the encoders, depending on what they support and/or what is relevant to the format
struct ImageEncodeOptions
{
  /*
  Type of the raw data. Currently, only 8 bits per channel, and only greyscale or RGBA is supported.
  Also, only having the data and format types be exactly the same is supported.
  In other words, only an RGBA and a greyscale mode with matching raw data are supported currently.
  This might get more possibilities later.
  */
  int dataBitsPerChannel;
  int dataColorType; //1 = Greyscale, 4 = RGBA
  
  int formatBitsPerChannel;
  int formatColorType; //1 = Greyscale, 4 = RGBA
  
  ImageEncodeOptions();
};

/*
decodeImageFile: decodes image to 32-bit RGBA buffer
returns true if ok, false if error happened
*/
bool decodeImageFile(std::string& error, std::vector<unsigned char>& image, int& w, int& h, const unsigned char* data, size_t datasize, ImageFormat format);
bool isOfFormat(const unsigned char* file, size_t filesize, ImageFormat format);
/*
encodeImageFile: encodes image from 32-bit RGBA buffer, to the closest to RGBA that the image format can offer (some can't save alpha)
TODO: add some generic options that some image formats support, such as: quality vs compression, number of channels, bit depth, interlace
*/
bool encodeImageFile(std::string& error, std::vector<unsigned char>& file, const unsigned char* image, int w, int h, ImageFormat format, const ImageEncodeOptions* options = 0);

bool supportsImageDecode(ImageFormat format);
bool supportsImageEncode(ImageFormat format);


ImageFormat findImageFormat(const unsigned char* file, size_t filesize); //returns IF_INVALID if no supported format found

} // namespace lpi
