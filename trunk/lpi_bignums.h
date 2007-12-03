/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LPI_BIGNUMS_H_INCLUDED
#define LPI_BIGNUMS_H_INCLUDED

#include <iostream>

/*
lpi::fixed128 is a 128-bit fixed point number
*) 32 bits represent the part after the point
*) 95 bits represent the part before the point
*) 1 bit (the MSB) represents the sign, but two's complement is used

lpi::ufixed128 is the unsigned version.

This allows nanometer precision over a distance of 39614081257132168796771975168
metres or 4187211692790 lightyears, which is ideal for my space game project.

For this to work properly, it's important that the type "unsigned" is 32-bit,
not more, not less.
But the C++ standard doesn't properly assure such thing for any type.
*/

/*
There are also template functions available that work on arbitrary uint32 arrays.

This is a work in progress, for example it's planned to make the sqrt and /= functions
also templated like already done with the add and multiply functions.
*/

namespace lpi
{

typedef unsigned uint32; //the 32-bit unsigned type used by the big numbers, must be exactly 32-bit!

/*
these are generalized functions that operate on arrays of uint32's
size is the number of uint32's that make up the number
frac is the number of those that represent bits behind the point
for some is a signed and unsigned version, for most it doesn't matter
the LSB's are in index[0] of arrays
*/

template<int size>
uint32 add(uint32* out, const uint32* a, const uint32* b) //return value is carry bit, out may be the same as a or b
{
  uint32 carry = 0U;
  for(int i = 0; i < size; i++)
  {
    out[i] = a[i] + b[i] + carry;
    if(!carry && out[i] < b[i]) carry = 1U; //if it has overflown
    else if(carry && out[i] <= b[i]) carry = 1U;
    else carry = 0U;
  }
  return carry;
}

template<int sizea, int fraca, int sizeb, int fracb>
uint32 add(uint32* out, const uint32* a, const uint32* b) //size of out will be the same as a. the number b must be completely inside the scope of a (ie no larger size, no lesser or more significant bits than a)
{
  /*
  preconditions:
  fracb <= fraca
  sizeb <= sizea
  (sizeb - fracb) <= (sizea - fraca)
  */
  uint32 carry = 0U;
  for(int i = 0; i < sizea; i++)
  {
    int j = i - (fraca - fracb);
    uint32 b_value = j >= 0 && j < sizeb ? b[j] : 0U;
    out[i] = a[i] + b_value + carry;
    if(!carry && out[i] < b_value) carry = 1U; //if it has overflown
    else if(carry && out[i] <= b_value) carry = 1U;
    else carry = 0U;
  }
  return carry;
}

template<int size>
bool subtract(uint32* out, const uint32* a, const uint32* b) //return value is borrow bit, out may be the same as a or b
{
  bool borrow = false;
  for(int i = 0; i < size; i++)
  {
    if(borrow)
    {
      borrow = b[i] >= a[i];
      out[i] -= b[i] + 1U;
    }
    else
    {
      borrow = b[i] > a[i];
      out[i] -= b[i];
    }
  }
  return borrow;
}

template<int sizea, int fraca, int sizeb, int fracb>
bool subtract(uint32* out, const uint32* a, const uint32* b) //size of out will be the same as a. the number b must be completely inside the scope of a (ie no larger size, no lesser or more significant bits than a)
{
  /*
  preconditions:
  fracb <= fraca
  sizeb <= sizea
  (sizeb - fracb) <= (sizea - fraca)
  */
  bool borrow = false;
  for(int i = 0; i < sizea; i++)
  {
    int j = i - (fraca - fracb);
    uint32 b_value = j >= 0 && j < sizeb ? b[j] : 0U;
    if(borrow)
    {
      borrow = b_value >= a[i];
      out[i] -= b_value + 1U;
    }
    else
    {
      borrow = b_value > a[i];
      out[i] -= b_value;
    }
  }
  return borrow;
}

template<int size>
bool sign(const uint32* a)
{
  return a[size - 1] >= 2147483648U;
}

template<int size>
void leftshift(uint32* a, uint32 shift)
{
  //will make this more efficient if needed
  if(shift >= uint32(size * 32U))
  {
    for(int i = 0; i < size; i++) a[i] = 0U;
  }
  else
  {
    while(shift >= 32)
    {
      shift -= 32;
      for(int i = size - 1; i > 0; i--) a[i] = a[i - 1];
      a[0] = 0;
    }
    while(shift >= 8)
    {
      void* v = &a[0];
      unsigned char* c = (unsigned char*)v;
      shift -= 8;
      for(int i = 0; i < (size * 4 - 1); i++) c[i + 1] = c[i]; //the 4 is from 32 / 8 (uint32 bits / char bits)
      c[0] = 0;
    }
    while(shift != 0)
    {
      shift--;
      for(int i = size - 1; i > 0; i--)
      {
        a[i] <<= 1U;
        if(a[i - 1] & 2147483648U) a[i] |= 1U;
        else a[i] &= 4294967294U;
      }
      a[0] <<= 1U;
    }
  }
}

template<int size>
void rightshift_unsigned(uint32* a, uint32 shift)
{
  //will make this more efficient if needed
  if(shift >= uint32(size * 32))
  {
    for(int i = 0; i < size; i++) a[i] = 0U;
  }
  else
  {
    while(shift >= 32)
    {
      shift -= 32;
      for(int i = 0; i < size - 1; i++)
      {
        a[i] = a[i + 1];
      }
      a[size - 1] = 0U;
    }
    while(shift >= 8)
    {
      void* v = &a[0];
      unsigned char* c = (unsigned char*)v;
      shift -= 8;
      for(int i = 0; i < (size * 4 - 1); i++) c[i] = c[i + 1]; //the 4 is from 32 / 8 (uint32 bits / char bits)
      c[size * 4 - 1] = 0U;
    }
    while(shift != 0)
    {
      shift--;
      for(int i = 0; i < size - 1; i++)
      {
        a[i] >>= 1U;
        if(a[i + 1] & 1U) a[i] |= 2147483648U;
        else a[i] &= 2147483647U;
      }
      a[size - 1] >>= 1;
      a[size - 1] &= 2147483647U;
    }
  }
}

template<int size>
void rightshift_signed(uint32* a, uint32 shift)
{
  //will make this more efficient if needed
  if(shift >= 128)
  {
    if(sign<size>(a)) for(int i = 0; i < size; i++) a[i] = 4294967295U;
    else for(int i = 0; i < size; i++) a[i] = 0U;
  }
  else
  {
    while(shift >= 32)
    {
      shift -= 32;
      for(int i = 0; i < size - 1; i++)
      {
        a[i] = a[i + 1];
      }
      a[size - 1] = sign<size>(a) ? 4294967295U : 0U;
    }
    while(shift >= 8)
    {
      void* v = &a[0];
      unsigned char* c = (unsigned char*)v;
      shift -= 8;
      for(int i = 0; i < (size * 4 - 1); i++) c[i] = c[i + 1]; //the 4 is from 32 / 8 (uint32 bits / char bits)
      c[size * 4 - 1] = sign<size>(a) ? 255U : 0U;
    }
    while(shift != 0)
    {
      bool negative = sign<size>(a);
      shift--;
      for(int i = 0; i < size - 1; i++)
      {
        a[i] >>= 1U;
        if(a[i + 1] & 1U) a[i] |= 2147483648U;
        else a[i] &= 2147483647U;
      }
      a[size - 1] >>= 1;
      if(negative) a[size - 1] |= 2147483648U;
      else a[size - 1] &= 2147483647U;
    }
  }
}

inline bool getLSB(const uint32* a)
{
  return a[0] & 1;
}

template<int size>
bool getMSB(const uint32* a)
{
  return a[size - 1] > 2147483647;
}

/*
getBit functions:
gets a bit from the given bignum.
Bit 0 is the lsb, bit 1 the more significant one, etc...
If you enter a negative number, it returns 0.
If you enter a number larger than size * 4:
-unsigned version will return 0
-signed version will return the MSB
*/

template<int size>
bool getBit_unsigned(const uint32* a, int bit)
{
  if(bit >= 0 && bit < size * 32)
  {
    int i = bit % 32;
    int j = bit / 32;
    
    return (a[j] >> i) & 1;
  }
  else if(bit < 0) return false;
  else return false;
}

template<int size>
bool getBit_signed(const uint32* a, int bit)
{
  if(bit >= 0 && bit < size * 32)
  {
    int i = bit % 32;
    int j = bit / 32;
    
    return (a[j] >> i) & 1;
  }
  else if(bit < 0) return false;
  else return getMSB<size>(a);
}


template<int size>
void addLSB(uint32* a) //increments the LSB and the other bits if it carries over
{
  for(int i = 0; i < size; i++)
  {
    a[i]++;
    if(a[i] != 0U) break; //it only carries over if this uint32 became 0 due to overflow
  }
}

template<int size>
void subtractLSB(uint32* a) //increments the LSB and the other bits if it carries over
{
  for(int i = 0; i < size; i++)
  {
    a[i]--;
    if(a[i] != 4294967295U) break; //it only carries over if this uint32 became 0 due to overflow
  }
}

template<int size>
void bit_invert(uint32* a)
{
  for(int i = 0; i < size; i++) a[i] = ~a[i];
}

template<int size>
void negate(uint32* a)
{
  bit_invert<size>(a);
  addLSB<size>(a);
}

template<int size>
void makeZero(uint32* a)
{
  for(int i = 0; i < size; i++) a[i] = 0U;
}

/*
sizeo = size of output in uint32's
sizea and sizeb = size of inputs
If the numbers have fractional bits, reinterpret them correctly after the multiplication.
It's just a shift over uint32's.
sizeo must be >= sizea and >= sizeb
the maximum useful sizeo is 2 * sizea or 2 * sizeb
*/
template<int sizeo, int sizea, int sizeb>
void multiply_signed(uint32* out, const uint32* a, const uint32* b)
{
  makeZero<sizeo>(out);
  
  //the copy is needed because add doesn't support bitshifted input, and the sign bits must be correct for more uints
  uint32 b_copy[sizeo];
  for(int i = 0; i < sizeb; i++) b_copy[i] = b[i];
  for(int i = sizeb; i < sizeo; i++) b_copy[i] = getMSB<sizeb>(b) ? 4294967295U : 0U;
  
  for(int j = 0; j < sizeo * 32; j++)
  {
    if(lpi::getBit_signed<sizea>(a, j)) add<sizeo, 0, sizeo, 0>(out, out, b_copy);
    leftshift<sizeo>(b_copy, 1U);
  }
}

template<int sizeo, int sizea, int sizeb>
void multiply_unsigned(uint32* out, const uint32* a, const uint32* b)
{
  makeZero<sizeo>(out);
  
  //the copy is needed because add doesn't support bitshifted input, and the sign bits must be correct for more uints
  uint32 b_copy[sizeo];
  for(int i = 0; i < sizeb; i++) b_copy[i] = b[i];
  for(int i = sizeb; i < sizeo; i++) b_copy[i] = 0U;
  
  for(int j = 0; j < sizeo * 32; j++)
  {
    if(lpi::getBit_unsigned<sizea>(a, j)) add<sizeo, 0, sizeo, 0>(out, out, b_copy);
    leftshift<sizeo>(b_copy, 1U);
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class fixed128;

class ufixed128 //unsigned version, used in some functions of the signed version
{
  public:
    
    
    //ctors
    ufixed128(){}
    ufixed128(const ufixed128& other) { *this = other; }
    ufixed128(double other) { *this = other; }
    ufixed128(uint32 other) { *this = other; }
    ufixed128(const fixed128& other) { *this = other; }
    
    //conversions
    operator double() const; //this only returns a correct result if the double can hold this value
    void operator=(double d);
    operator uint32() const;
    void operator=(uint32 d);
    void operator=(const fixed128& o);

    //member operators
    ufixed128& operator+=(const ufixed128& rhs);
    ufixed128& operator-=(const ufixed128& rhs);
    
    ufixed128& operator+=(uint32 rhs);
    ufixed128& operator-=(uint32 rhs);
    
    ufixed128& operator+=(double rhs);
    ufixed128& operator-=(double rhs);
    
    ufixed128& operator>>=(uint32 shift);
    ufixed128& operator<<=(uint32 shift);
    
    ufixed128& operator&=(const ufixed128& rhs);
    ufixed128& operator|=(const ufixed128& rhs);
    ufixed128& operator^=(const ufixed128& rhs);
    
    ufixed128& operator*=(uint32 rhs);
    ufixed128& operator*=(const ufixed128& rhs);
    ufixed128& operator*=(double rhs);
    
    ufixed128& operator/=(const ufixed128& rhs);
    
    ufixed128& operator++();
    ufixed128 operator++(int);
    ufixed128& operator--();
    ufixed128 operator--(int);
    
    bool operator==(const ufixed128& rhs) const;
    bool operator==(uint32 rhs) const;
    bool operator>(const ufixed128& rhs) const;
    bool operator<(const ufixed128& rhs) const;
    bool operator>=(const ufixed128& rhs) const;
    bool operator<=(const ufixed128& rhs) const;

    //helper functions
    void makeZero();
    bool isZero() const { return data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0; }
    void addLSB();
    void subtractLSB();
    bool getLSB() { return data[0] & 1; }
    bool getMSB() { return data[3] > 2147483647; }
    void bit_invert();
    void negate(); //does to the bits what "would" be done if it were signed, but it isn't
    bool sign() const; //does to the bits what "would" be done if it were signed, but it isn't
    void round() { data[0] = 0; } //this removes the fractional part (32 bits are set to 0, that's all this does)
    uint32 getFractional() const { return data[0]; } //returns the 32 fractional bits
    void setFractional(uint32 frac) { data[0] = frac; } //set the fractional part to that integer (divided through roughly 4 billion)
    void addFractional(uint32 rhs); //add integer to the fractional part
    void subtractFractional(uint32 rhs); //subtract integer from the fractional part
    void crudeMultiply(const ufixed128& rhs); //not really useful function
    void takeSqrt();
    
    friend class fixed128;
    
    private:
    /*
    uint32 is assumed to be 32 bit, it won't work at all if it isn't, incorrect overflows happen then.
    data[0] are the bits behind the point
    the LSB of data[0] is also the LSB of this ufixed128
    the LSB of data[1] is the LSB of the integer part of this ufixed128
    */
    uint32 data[4];
};


//free operators
ufixed128 operator-(const ufixed128& a); //does to the bits what "would" be done if it were signed, but it isn't
ufixed128 operator~(const ufixed128& a);

ufixed128 operator+(const ufixed128& a, const ufixed128& b);
ufixed128 operator-(const ufixed128& a, const ufixed128& b);
ufixed128 operator+(const ufixed128& a, uint32& b);
ufixed128 operator+(uint32& a, const ufixed128& b);
ufixed128 operator-(const ufixed128& a, uint32& b);

ufixed128 operator>>(const ufixed128& a, uint32 b);
ufixed128 operator<<(const ufixed128& a, uint32 b);

ufixed128 operator&(const ufixed128& a, const ufixed128& b);
ufixed128 operator|(const ufixed128& a, const ufixed128& b);
ufixed128 operator^(const ufixed128& a, const ufixed128& b);

ufixed128 operator*(const ufixed128& a, const ufixed128& b);
ufixed128 operator*(const ufixed128& a, uint32 b);
ufixed128 operator*(uint32 a, const ufixed128& b);
ufixed128 operator*(const ufixed128& a, double b);
ufixed128 operator*(double a, const ufixed128& b);

ufixed128 operator/(const ufixed128& a, const ufixed128& b);

ufixed128 sqrt(const ufixed128& a);

bool operator==(uint32 a, const ufixed128& b); //the other operator=='s are member functions since access to private data is needed

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class fixed128
{
  public:
    //ctors
    fixed128(){}
    fixed128(const fixed128& other) { *this = other; }
    fixed128(double other) { *this = other; }
    fixed128(uint32 other) { *this = other; }
    fixed128(const ufixed128& other) { *this = other; }
    
    //conversions
    operator double() const; //this only returns a correct result if the double can hold this value
    void operator=(double d);
    operator uint32() const;
    void operator=(uint32 d);
    void operator=(const ufixed128& o);

    //member operators
    fixed128& operator+=(const fixed128& rhs);
    fixed128& operator-=(const fixed128& rhs);
    
    fixed128& operator+=(uint32 rhs);
    fixed128& operator-=(uint32 rhs);
    
    fixed128& operator+=(double rhs);
    fixed128& operator-=(double rhs);
    
    fixed128& operator>>=(uint32 shift);
    fixed128& operator<<=(uint32 shift);
    
    fixed128& operator&=(const fixed128& rhs);
    fixed128& operator|=(const fixed128& rhs);
    fixed128& operator^=(const fixed128& rhs);
    
    fixed128& operator*=(uint32 rhs);
    fixed128& operator*=(const fixed128& rhs);
    fixed128& operator*=(double rhs);
    
    fixed128& operator/=(const fixed128& rhs);
    
    fixed128& operator++();
    fixed128 operator++(int);
    fixed128& operator--();
    fixed128 operator--(int);
    
    bool operator==(const fixed128& rhs) const;
    bool operator==(uint32 rhs) const;
    bool operator>(const fixed128& rhs) const;
    bool operator<(const fixed128& rhs) const;
    bool operator>=(const fixed128& rhs) const;
    bool operator<=(const fixed128& rhs) const;

    //helper functions
    void makeZero();
    bool isZero() const { return data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0; }
    void addLSB();
    void subtractLSB();
    bool getLSB() { return data[0] & 1; }
    bool getMSB() { return data[3] > 2147483647; }
    void bit_invert();
    void negate();
    bool sign() const;
    void round() { data[0] = 0; } //this removes the fractional part (32 bits are set to 0, that's all this does)
    uint32 getFractional() const { return data[0]; } //returns the 32 fractional bits
    void setFractional(uint32 frac) { data[0] = frac; } //set the fractional part to that integer (divided through roughly 4 billion)
    void addFractional(uint32 rhs); //add integer to the fractional part
    void subtractFractional(uint32 rhs); //subtract integer from the fractional part
    void crudeMultiply(const fixed128& rhs); //not really useful function
    void takeSqrt();
    
    friend class ufixed128;
    
    private:
    /*
    uint32 is assumed to be 32 bit, it won't work at all if it isn't, incorrect overflows happen then.
    data[0] are the bits behind the point
    the LSB of data[0] is also the LSB of this fixed128
    the LSB of data[1] is the LSB of the integer part of this fixed128
    the MSB of data[3] is the sign (note that two's complement notation is used)
    */
    uint32 data[4];
};


//free operators
fixed128 operator-(const fixed128& a);
fixed128 operator~(const fixed128& a);

fixed128 operator+(const fixed128& a, const fixed128& b);
fixed128 operator-(const fixed128& a, const fixed128& b);
fixed128 operator+(const fixed128& a, uint32& b);
fixed128 operator+(uint32& a, const fixed128& b);
fixed128 operator-(const fixed128& a, uint32& b);

fixed128 operator>>(const fixed128& a, uint32 b);
fixed128 operator<<(const fixed128& a, uint32 b);

fixed128 operator&(const fixed128& a, const fixed128& b);
fixed128 operator|(const fixed128& a, const fixed128& b);
fixed128 operator^(const fixed128& a, const fixed128& b);

fixed128 operator*(const fixed128& a, const fixed128& b);
fixed128 operator*(const fixed128& a, uint32 b);
fixed128 operator*(uint32 a, const fixed128& b);
fixed128 operator*(const fixed128& a, double b);
fixed128 operator*(double a, const fixed128& b);

fixed128 operator/(const fixed128& a, const fixed128& b);

fixed128 sqrt(const fixed128& a);

bool sign(const fixed128& a);
fixed128 abs(const fixed128& a);

bool operator==(uint32 a, const fixed128& b); //the other operator=='s are member functions since access to private data is needed

} //namespace lpi


#endif

