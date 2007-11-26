/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CRYPTO_H
#define CRYPTO_H

#include <vector>
#include <string>

#include <iostream>

namespace lpi
{

//v is the data, k the password, num_rounds is suggested to be 64, unsigned long is 32 bit, v is two unsigned longs, k is 128 bit

inline void xtea_encipher(unsigned int num_rounds, unsigned long* v, unsigned long* k)
{
  unsigned long v0 = v[0], v1 = v[1];
  unsigned long sum = 0, delta = 0x9E3779B9;
  for(unsigned int i = 0; i < num_rounds; i++)
  {
    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    sum += delta;
    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
  }
  v[0] = v0;
  v[1] = v1;
}

inline void xtea_decipher(unsigned int num_rounds, unsigned long* v, unsigned long* k)
{
  unsigned long v0 = v[0], v1 = v[1];
  unsigned long delta = 0x9E3779B9, sum = delta * num_rounds;
  for(unsigned int i = 0; i < num_rounds; i++)
  {
    v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
    sum -= delta;
    v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
  }
  v[0] = v0;
  v[1] = v1;
}

/*
xtea encryption/decription with following properties:
-it uses the xtea reference code to do the actual ciphering (so the encrypted data has an amount of bytes that is a multiple of 8)
-the last byte in the encrypted data indicates the amount of values (excluding this byte itself) at the end that have to be removed after deciphering (0-7) -> this indicator byte isn't encrypted itself so that people can't reverse engineer the encrypted content knowing that the last byte is 0-7
-the key is an ascii string (the password), which is converted to 128 bit (16 characters) by adding zeroes or removing the last part
-8 bytes of vector/string data make 2 32-bit values of the xtea code; the first 4 bytes are v0, the next 4 bytes are v1, in little endian byte order (little end first)
-similarly, the password bytes are used as little endian encoding of the 128 bit key (little end first)
*/
template<typename T>
void xtea_encipher(T& out, const T& in, const std::string& password)
{
  out = in;
  size_t extra_zeroes = (8 - (out.size() % 8)) % 8;
  out.resize(out.size() + extra_zeroes, 0); //add values until the size is a multiple of 8
  
  unsigned long k[4]; //128 bit key
  std::string pw16 = password; //password with 16 chars (=128 bit)
  pw16.resize(16, 0);
  typedef unsigned char uc;
  
  k[0] = ((uc)pw16[ 0]) | ((uc)pw16[ 1] << 8) | ((uc)pw16[ 2] << 16) | ((uc)pw16[ 3] << 24);
  k[1] = ((uc)pw16[ 4]) | ((uc)pw16[ 5] << 8) | ((uc)pw16[ 6] << 16) | ((uc)pw16[ 7] << 24);
  k[2] = ((uc)pw16[ 8]) | ((uc)pw16[ 9] << 8) | ((uc)pw16[10] << 16) | ((uc)pw16[11] << 24);
  k[3] = ((uc)pw16[12]) | ((uc)pw16[13] << 8) | ((uc)pw16[14] << 16) | ((uc)pw16[15] << 24);
  
  unsigned long v[2];
  
  for(size_t i = 0; i + 7 < out.size(); i += 8)
  {
    v[0] = ((uc)out[i + 0]) | ((uc)out[i + 1] << 8) | ((uc)out[i + 2] << 16) | ((uc)out[i + 3] << 24);
    v[1] = ((uc)out[i + 4]) | ((uc)out[i + 5] << 8) | ((uc)out[i + 6] << 16) | ((uc)out[i + 7] << 24);
    
    xtea_encipher(64, v, k);
    
    out[i + 0] = (v[0]) & 255; out[i + 1] = (v[0] >> 8) & 255; out[i + 2] = (v[0] >> 16) & 255; out[i + 3] = (v[0] >> 24) & 255;
    out[i + 4] = (v[1]) & 255; out[i + 5] = (v[1] >> 8) & 255; out[i + 6] = (v[1] >> 16) & 255; out[i + 7] = (v[1] >> 24) & 255;
  }

  out.push_back(extra_zeroes); //at the end, specify to the data how many zeroes can be removed after decoding
}

//returns false if everything went ok, returns true and does nothing if the amount of zeros is > 7 which means it wasn't an enciphered file OR the filesize modulo 8 of the input file isn't 1
template<typename T>
bool xtea_decipher(T& out, const T& in, const std::string& password)
{
  if((in.size() % 8) != 1) return true; //error, incorrect size
  
  size_t extra_zeroes = in[in.size() - 1];
  
  if(extra_zeroes >= 8) return true; //error, incorrect amount of extra zeroes indicated
  
  out = in;

  out.resize(out.size() - 1);
  
  
  
  unsigned long k[4]; //128 bit key
  std::string pw16 = password; //password with 16 chars (=128 bit)
  pw16.resize(16, 0);
  typedef unsigned char uc;
  k[0] = ((uc)pw16[ 0]) | ((uc)pw16[ 1] << 8) | ((uc)pw16[ 2] << 16) | ((uc)pw16[ 3] << 24);
  k[1] = ((uc)pw16[ 4]) | ((uc)pw16[ 5] << 8) | ((uc)pw16[ 6] << 16) | ((uc)pw16[ 7] << 24);
  k[2] = ((uc)pw16[ 8]) | ((uc)pw16[ 9] << 8) | ((uc)pw16[10] << 16) | ((uc)pw16[11] << 24);
  k[3] = ((uc)pw16[12]) | ((uc)pw16[13] << 8) | ((uc)pw16[14] << 16) | ((uc)pw16[15] << 24);
  
  unsigned long v[2];
  
  for(size_t i = 0; i + 7 < in.size(); i += 8)
  {
    v[0] = ((uc)out[i + 0]) | ((uc)out[i + 1] << 8) | ((uc)out[i + 2] << 16) | ((uc)out[i + 3] << 24);
    v[1] = ((uc)out[i + 4]) | ((uc)out[i + 5] << 8) | ((uc)out[i + 6] << 16) | ((uc)out[i + 7] << 24);
    
    xtea_decipher(64, v, k);
    
    out[i + 0] = (v[0]) & 255; out[i + 1] = (v[0] >> 8) & 255; out[i + 2] = (v[0] >> 16) & 255; out[i + 3] = (v[0] >> 24) & 255;
    out[i + 4] = (v[1]) & 255; out[i + 5] = (v[1] >> 8) & 255; out[i + 6] = (v[1] >> 16) & 255; out[i + 7] = (v[1] >> 24) & 255;
  }
  
  out.resize(out.size() - extra_zeroes);

  return false;
}

} //end of namespace lpi

#endif
