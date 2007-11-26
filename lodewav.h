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

#ifndef lodewav_h
#define lodewav_h

#include <vector>

//g++ *.cpp -Wall -Wextra -pedantic -ansi -O3

namespace LodeWAV
{

struct InfoWAV
{
  int channels; //amount of channels
  int samplerate; //samples per second
  int bitsPerSample; //bits per sample in the output wav
  
  InfoWAV(int channels, int samplerate, int bitsPerSample)
  {
    this->channels = channels;
    this->samplerate = samplerate;
    this->bitsPerSample = bitsPerSample;
  }
  
  InfoWAV() {}
};

//the input is always assumed to be audio data with an amplitude from -1.0 to +1.0
void encode(std::vector<unsigned char>& out, const std::vector<std::vector<double> >& in, const InfoWAV& info);

//same, but the value is stored in integers. Samples should be in the range -32768 to +32767
void encode(std::vector<unsigned char>& out, const std::vector<std::vector<int> >& in, const InfoWAV& info);
/*
return value is error code
0: no error
10: file too small to contain RIFF header and minimum required chunks
11: no RIFF header
12: no WAVE in RIFF header
13: too small format chunk
14: compression not supported, only wFormatTag 1 (PCM) is supported
15: there must come a format chunk before the data chunk
16: not enough bytes to read chunk type & size of next chunk
17: not enough bytes to read for the full chunksize
*/
//in the output, +1.0 is max amplitude, -1.0 is min amplitude
int decode(std::vector<std::vector<double> >& out, InfoWAV& info, const std::vector<unsigned char>& in);

//same, but the value is stored in integers. Samples will be in the range -32768 to +32767
int decode(std::vector<std::vector<int> >& out, InfoWAV& info, const std::vector<unsigned char>& in);

} //end of namespace LodeWAV

#endif //lodewav_h
