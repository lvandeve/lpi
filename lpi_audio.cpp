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

#include "lpi_audio.h"

#include <SDL/SDL.h>
#include <iostream>

namespace lpi
{

AudioSDL::AudioSDL()
{
  if(SDL_Init(SDL_INIT_AUDIO) < 0)
  {
    std::cout << "Unable to init audio: " << SDL_GetError() << std::endl;
  }
}

AudioSDL::~AudioSDL()
{
}

////////////////////////////////////////////////////////////////////////////////
//Multithreading helper functions///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//SDL's C functions don't have destructors and such so therefore this here is needed

//currently only needed for audio, therefor it's not in a different cpp file.

//this creates SDL mutexes and makes sure that they're destroyed at the end. MutexFactory does the deletion!
struct MutexFactory
{
  SDL_mutex* createMutex()
  {
    mutexes.push_back(SDL_CreateMutex());
    return mutexes.back();
  }
  
  ~MutexFactory()
  {
    for(size_t i = 0; i < mutexes.size(); i++)
      SDL_DestroyMutex(mutexes[i]);
  }
  
  private:
  
  std::vector<SDL_mutex*> mutexes;
};

MutexFactory mutexFactory;

//this does SDL_mutexP in the ctor and SDL_mutexV in the dtor so no matter where you leave a function, SDL_mutexV is called
struct Mutex
{
  SDL_mutex** m;
  
  Mutex(SDL_mutex*& mutex)
  {
    m = &mutex;
    SDL_mutexP(*m);
  }
  
  ~Mutex()
  {
    SDL_mutexV(*m);
  }
};

////////////////////////////////////////////////////////////////////////////////
//Soundcard functions///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

size_t audio_min_samples = 4096; //safety buffer to avoid clicks
size_t audio_max_samples = 8192; //avoid too long queue

double audio_volume = 1.0;
int audio_mode = 2; //0=off, 1=full (volume ignored), 2=volume-controlled

void AudioSDL::audioSetBufferSamplesRange(size_t min_samples, size_t max_samples)
{
  audio_min_samples = min_samples;
  audio_max_samples = max_samples;
}

void AudioSDL::audioSetMode(int mode) //0: silent, 1: full (no volume calculations ==> faster), 2: volume-controlled (= default value)
{
  audio_mode = mode;
}

void AudioSDL::audioSetVolume(double volume) //multiplier used if mode is 2 (volume-controlled). Default value is 1.0.
{
  audio_volume = volume;
}

/*
Avoid the callback function and pushSamples function to be called at the same time,
or the std::vector can be invalid as two threads at the same time change it.
This SDL_mutex helps eliminate that problem.
*/
SDL_mutex* audio_lock = mutexFactory.createMutex();

std::vector<double> audio_data(audio_min_samples, 0);

SDL_AudioSpec audiospec_wanted, audiospec_obtained;

size_t AudioSDL::audioSamplesShortage() //returns value > 0 if the soundcard is consuming more samples than you're producing
{
  if(audio_data.size() < audio_min_samples) return audio_min_samples - audio_data.size();
  else return 0;
}

size_t AudioSDL::audioSamplesOverflow() //returns value > 0 if you're producing more samples than the soundard is consuming - so take it easy a bit
{
  if(audio_data.size() > audio_max_samples) return audio_data.size() - audio_max_samples;
  else return 0;
}

void audioCallback(void* /*userdata*/, Uint8* stream, int len)
{
  Mutex mutex(audio_lock);
  
  int dataLengthLeft = audio_data.size();

  //only play if we have data left
  if(dataLengthLeft <= 0) return;
  
  int nsamples = len / 2; //always 16-bit, so always 2 bytes per sample, hence the amount of samples being len / 2
  int fill_len = (nsamples < dataLengthLeft ? nsamples : dataLengthLeft);
  
  for(int i = 0; i < nsamples; i++)
  {
    if(i < fill_len)
    {
      int s = int(audio_data[i] * 32768);
      if(s < -32768) s = -32768;
      if(s > 32767) s = 32767;
    
      stream[i * 2 + 0] = Uint8(s % 256);
      stream[i * 2 + 1] = Uint8(s / 256);
    }
    else stream[i * 2 + 0] = stream[i * 2 + 1] = 0;
  }
  
  audio_data.erase(audio_data.begin(), audio_data.begin() + fill_len);
}

int AudioSDL::audioOpen(int samplerate, int framesize) //always 16-bit mono sound for now
{
  //set the audio format
  audiospec_wanted.freq = samplerate;
  audiospec_wanted.format = AUDIO_S16;
  audiospec_wanted.channels = 1;  //1 = mono, 2 = stereo
  audiospec_wanted.samples = framesize;
  audiospec_wanted.callback = audioCallback;
  audiospec_wanted.userdata = NULL;
  
  /*
  when using alsa and 44100 samples/second, then the framesize (samples)
  will be 940 instead of 1024. Resampled to 48000Hz, this gives back 1024.
  */

  //open the audio device, forcing the wanted format
  if(SDL_OpenAudio(&audiospec_wanted, &audiospec_obtained) < 0)
  {
    return 1;
  }
  
  SDL_PauseAudio(0);
  
  return 0;
}

void AudioSDL::audioClose()
{
  SDL_CloseAudio();
}

int AudioSDL::audioReOpen() //closes and opens again with same parameters
{
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  if ( SDL_OpenAudio(&audiospec_wanted, &audiospec_obtained) < 0 ) 
  {
    return 1;
  }
  SDL_PauseAudio(0);
  
  return 0;
}


//only works correct for 16 bit audio currently
void AudioSDL::audioPushSamples(const std::vector<double>& samples, size_t pos, size_t end)
{
  if(audio_mode == 0) return;
  
  Mutex mutex(audio_lock);
  
  if(audio_mode == 1)
  {
    audio_data.insert(audio_data.end(), samples.begin() + pos, samples.begin() + end);
  }
  else if(audio_mode == 2)
  {
    size_t j = audio_data.size();
    audio_data.resize(j + samples.size());
    for(size_t i = 0; i < samples.size(); i++)
    {
      audio_data[j + i] = samples[i] * audio_volume;
    }
  }
}

void AudioSDL::audioPlay(const std::vector<double>& samples)
{
  if(audio_mode == 0) return;
  
  Mutex mutex(audio_lock);
  
  //the *current* time is at the first sample of audio_data, the rest has been played through soundcard already
  
  if(samples.size() > audio_data.size()) audio_data.resize(samples.size(), 0.0);
  
  if(audio_mode == 1) for(size_t i = 0; i < samples.size(); i++) audio_data[i] += samples[i];
  else if(audio_mode == 2) for(size_t i = 0; i < samples.size(); i++) audio_data[i] += samples[i] * audio_volume;
}

} //namespace lpi



