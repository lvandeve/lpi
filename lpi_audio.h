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

#ifndef LPI_AUDIO_H_INCLUDED
#define LPI_AUDIO_H_INCLUDED

#include <vector>

namespace lpi
{

class AudioSDL //may have only one instance!! This is just put in a class to group it somewhat better together, no proper interface or usage of ctor and dtor yet.
{
  public:
    AudioSDL();
    ~AudioSDL();
    
    int audioOpen(int samplerate, int framesize); //always 16-bit mono sound for now; returns 0 if no error happened. Pick for example samplerate 44100, framesize 1024
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

};


} //namespace lpi

#endif
