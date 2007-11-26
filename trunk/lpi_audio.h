/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LPI_AUDIO_H_INCLUDED
#define LPI_AUDIO_H_INCLUDED

#include <vector>

namespace lpi
{

int audioOpen(int samplerate, int framesize); //always 16-bit mono sound for now; returns 0 if no error happened
void audioClose();
int audioReOpen(); //closes and opens again with same parameters

/*
push samples to the soundcard, making sure not to cause shortage or overflow
pos and end are the range in the samples vector that you want to push to the audio card
*/
void audioPushSamples(const std::vector<double>& samples, size_t pos, size_t end);

size_t audioSamplesShortage(); //returns value > 0 if the soundcard is consuming more samples than you're producing
size_t audioSamplesOverflow(); //returns value > 0 if you're producing more samples than the soundard is consuming - so take it easy a bit
void audioSetBufferSamplesRange(size_t min_samples, size_t max_samples); //set shortage and overflow values. E.g. 4096 and 8192.

/*
This plays the sound starting at this time, until it's done
The difference with audioPushSamples is:
audioPlay allows playing multiple sounds at the same time: it doesn't push at the end,
but elementwise-adds or pushes back samples if needed.
The duration depends on samplerate, make sure the samples in the vector have the correct samplerate.
*/
void audioPlay(const std::vector<double>& samples);

void audioSetMode(int mode); //0: silent, 1: full (no volume calculations ==> faster), 2: volume-controlled (= default value)
void audioSetVolume(double volume); //multiplier used if mode is 2 (volume-controlled). Default value is 1.0.

} //namespace lpi

#endif
