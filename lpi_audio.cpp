 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_audio.h"

#include <SDL/SDL.h>

namespace lpi
{

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

void audioSetBufferSamplesRange(size_t min_samples, size_t max_samples)
{
  audio_min_samples = min_samples;
  audio_max_samples = max_samples;
}

void audioSetMode(int mode) //0: silent, 1: full (no volume calculations ==> faster), 2: volume-controlled (= default value)
{
  audio_mode = mode;
}

void audioSetVolume(double volume) //multiplier used if mode is 2 (volume-controlled). Default value is 1.0.
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

size_t audioSamplesShortage() //returns value > 0 if the soundcard is consuming more samples than you're producing
{
  if(audio_data.size() < audio_min_samples) return audio_min_samples - audio_data.size();
  else return 0;
}

size_t audioSamplesOverflow() //returns value > 0 if you're producing more samples than the soundard is consuming - so take it easy a bit
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

int audioOpen(int samplerate, int framesize) //always 16-bit mono sound for now
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

void audioClose()
{
  SDL_CloseAudio();
}

int audioReOpen() //closes and opens again with same parameters
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
void audioPushSamples(const std::vector<double>& samples, size_t pos, size_t end)
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

void audioPlay(const std::vector<double>& samples)
{
  if(audio_mode == 0) return;
  
  Mutex mutex(audio_lock);
  
  //the *current* time is at the first sample of audio_data, the rest has been played through soundcard already
  
  if(samples.size() > audio_data.size()) audio_data.resize(samples.size(), 0.0);
  
  if(audio_mode == 1) for(size_t i = 0; i < samples.size(); i++) audio_data[i] += samples[i];
  else if(audio_mode == 2) for(size_t i = 0; i < samples.size(); i++) audio_data[i] += samples[i] * audio_volume;
}

} //namespace lpi



