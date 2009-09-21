/*
LodeWAV version 20071117

Copyright (c) 2007 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "lodewav.h"

#include <vector>


namespace LodeWAV
{

void encode(std::vector<unsigned char>& out, const std::vector<std::vector<double> >& in, const InfoWAV& info)
{
  if(info.channels < 1) return; //nothing to encode
  
  //convert the input data to unsigned char data with channels interleaved
  std::vector<unsigned char> data;
  
  long range = 1 << info.bitsPerSample; //the range of values for the integers of samples in the wav file
  unsigned long halfrange = range / 2;
  
  for(size_t x = 0; x < in[0].size(); x++)
  {
    for(int j = 0; j < info.channels; j++)
    {
      double v = in[j][x];
      unsigned long vn = (unsigned long)((1 << info.bitsPerSample) * ((v + 1.0) / 2.0)); //normalized
      
      //now make the number two's complement, center level is integer value 0 in the wav
      
      if(vn >= halfrange)
      {
        vn -= halfrange;
      }
      else
      {
        vn += halfrange;
      }
      
      for(int i = 0; i < info.bitsPerSample / 8; i++)
      {
        unsigned char c = ((unsigned long)(vn) >> (i * 8)) & 255;
        data.push_back(c);
      }
    }
  }
    
  //calculate parameters for WAV file
  int blockAlign = (info.channels * info.bitsPerSample) / 8;
  int byteRate = info.samplerate * blockAlign;
  
  int filesize = 44 + data.size();
  out.clear();

  //== RIFF header ==
  //"RIFF"
  out.push_back('R');
  out.push_back('I');
  out.push_back('F');
  out.push_back('F');
  //size of the file minus 8 (= number of bytes that comes after these 4 bytes)
  out.push_back( (filesize - 8)             % 256);
  out.push_back(((filesize - 8) /      256) % 256);
  out.push_back(((filesize - 8) /    65536) % 256);
  out.push_back(((filesize - 8) / 16777216) % 256);
  //"WAVE"
  out.push_back('W');
  out.push_back('A');
  out.push_back('V');
  out.push_back('E');
  
  //== FORMAT chunk ==
  //ID (4 bytes)
  out.push_back('f');
  out.push_back('m');
  out.push_back('t');
  out.push_back(' ');
  
  //chunkSize (excluding the 8 bytes for ID and chunkSize)
  out.push_back(16);
  out.push_back(0);
  out.push_back(0);
  out.push_back(0);
  
  //wFormatTag
  out.push_back(1); //1 = no compression
  out.push_back(0);
  
  //wChannels
  out.push_back(info.channels);
  out.push_back(0);
  
  //dwSamplesPerSec
  out.push_back( (info.samplerate)             % 256);
  out.push_back(((info.samplerate) /      256) % 256);
  out.push_back(((info.samplerate) /    65536) % 256);
  out.push_back(((info.samplerate) / 16777216) % 256);
  
  //dwAvgBytesPerSec
  out.push_back( (byteRate)             % 256);
  out.push_back(((byteRate) /      256) % 256);
  out.push_back(((byteRate) /    65536) % 256);
  out.push_back(((byteRate) / 16777216) % 256);
  
  //wblockAlign
  out.push_back(blockAlign);
  out.push_back(0);
  
  //wBitsPerSample
  out.push_back(info.bitsPerSample);
  out.push_back(0);
  
  //== DATA chunk ==
  //ID (4 bytes)
  out.push_back('d');
  out.push_back('a');
  out.push_back('t');
  out.push_back('a');
  
  //chunkSize (excluding the 8 bytes for ID and chunkSize)
  out.push_back( data.size()             % 256);
  out.push_back((data.size() /      256) % 256);
  out.push_back((data.size() /    65536) % 256);
  out.push_back((data.size() / 16777216) % 256);
  
  //the data
  for(size_t x = 0; x < data.size(); x++)
  {
    out.push_back(data[x]);
  }
}

int decode(std::vector<std::vector<double> >& out, InfoWAV& info, const std::vector<unsigned char>& in)
{
  std::vector<unsigned char> data;
  
  size_t pos = 0;
  //== RIFF header ==
  if(in.size() < 44) return 10; //error: file too small
  //"RIFF"
  if(in[0] != 'R' || in[1] != 'I' || in[2] != 'F' || in[3] != 'F') return 11; //error: no RIFF
  //size of the file minus 8 (= number of bytes that comes after these 4 bytes)
  //not of interest, we already know the file size
  //"WAVE"
  if(in[8] != 'W' || in[9] != 'A' || in[10] != 'V' || in[11] != 'E') return 12; //error: no WAVE
  
  pos += 12;
  
  //chunks
  bool format_chunk_read = false;
  bool done = false;

  while(!done)
  {
    if(pos >= in.size() - 8) return 16; //error: size too short for next block
    
    if(in[pos] == 'f' && in[pos + 1] == 'm' && in[pos + 2] == 't' && in[pos + 3] == ' ')
    {
      pos += 4;
      
      format_chunk_read = true;
      int chunkSize = in[pos] + 256 * in[pos + 1] + 65536 * in[pos + 2] + 16777216 * in[pos + 3];
      pos += 4;
      if(pos + chunkSize >= in.size()) return 17;
      if(chunkSize < 16) return 13; //fmt block must have this size
      
      short wFormatTag = in[pos] + 256 * in[pos + 1];
      pos += 2;
      unsigned short wChannels = in[pos] + 256 * in[pos + 1];
      pos += 2;
      unsigned long dwSamplesPerSec = in[pos] + 256 * in[pos + 1] + 65536 * in[pos + 2] + 16777216 * in[pos + 3];
      pos += 4;
      //unsigned long dwAvgBytesPerSec = in[pos] + 256 * in[pos + 1] + 65536 * in[pos + 2] + 16777216 * in[pos + 3];
      pos += 4;
      //unsigned short wblockAlign = in[pos] + 256 * in[pos + 1];
      pos += 2;
      unsigned short wBitsPerSample = in[pos] + 256 * in[pos + 1];
      pos += 2;
      
      //there can be two more bytes here that specify the length of extra format bytes. However this doesn't occur if wFormatTag is 1 (no compression)
      
      if(wFormatTag != 1) return 14; //error: unsupported format
      
      info.channels = wChannels;
      info.samplerate = dwSamplesPerSec;
      info.bitsPerSample = wBitsPerSample;
    }
    else if(in[pos] == 'd' && in[pos + 1] == 'a' && in[pos + 2] == 't' && in[pos + 3] == 'a')
    {
      pos += 4;
      
      if(!format_chunk_read) return 15; //error: fmt chunk must come before this!
      
      size_t chunkSize = in[pos] + 256 * in[pos + 1] + 65536 * in[pos + 2] + 16777216 * in[pos + 3];
      pos += 4;

      if(pos + chunkSize > in.size()) return 17;
      
      data.clear();
      
      data.insert(data.begin(), in.begin() + pos, in.begin() + pos + chunkSize);
      pos += chunkSize;
    }
    else //unknown chunk type, skip over it
    {
      pos += 4;
      
      size_t chunkSize = in[pos] + 256 * in[pos + 1] + 65536 * in[pos + 2] + 16777216 * in[pos + 3];
      pos += 4;
      //if(pos + chunkSize >= in.size()) return 17;
      pos += chunkSize;
    }
    
    if(pos >= in.size())
    {
      done = true;
    }
  }
  
  //convert the input data to unsigned char data with channels interleaved (= as they are, so we just convert samples in the same order)
  out.clear();
  out.resize(info.channels);
  
  int bytesPerSample = info.bitsPerSample / 8;
  
  long range = 1 << info.bitsPerSample; //the range of values for the integers of samples in the wav file
  long halfrange = range / 2;
  double factor = 1.0 / halfrange; //factor to convert to format where the audio is in the range -32768 - +32767
  
  for(size_t x = 0; x < data.size(); x += bytesPerSample * info.channels)
  {
    for(int c = 0; c < info.channels; c++)
    {
      size_t index = x + c * bytesPerSample;
      long v = 0;
      for(int i = 0; i < bytesPerSample; i++)
      {
        v += (data[index + i] << (i * 8));
      }
      
      //in the wav, the value 0 is the center (zero level), values up to range / 2 are positive, values higher than that are negative (where range is (1 << info.bitsPerSample))
      //now I shift this so that 0 is the lowest negative level, range / 2 the zero level and range - 1 the maximum positive level
      if(v < halfrange)
      {
        v += halfrange;
      }
      else
      {
        v -= halfrange;
      }
      
      double vn = (v * factor) - 1.0;
      out[c].push_back(vn);
    }
  }
  
  return 0;
}

void encode(std::vector<unsigned char>& out, const std::vector<std::vector<int> >& in, const InfoWAV& info)
{
  std::vector<std::vector<double> > d(in.size());
  for(size_t i = 0; i < in.size(); i++)
  {
    d[i].resize(in[i].size());
    for(size_t j = 0; j < in[i].size(); j++)
    {
      d[i][j] = in[i][j] / 32768.0;
    }
  }
  encode(out, d, info);
}

int decode(std::vector<std::vector<int> >& out, InfoWAV& info, const std::vector<unsigned char>& in)
{
  std::vector<std::vector<double> > d;
  int result = decode(d, info, in);
  
  if(result == 0)
  {
    out.resize(d.size());
    for(size_t i = 0; i < d.size(); i++)
    {
      out[i].resize(d[i].size());
      for(size_t j = 0; j < d[i].size(); j++)
      {
        out[i][j] = int(d[i][j] * 32768.0);
      }
    }
  }
  
  return result;
}

} //namespace LodeWAV


