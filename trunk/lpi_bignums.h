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

#ifndef LPI_BIGNUMS_H_INCLUDED
#define LPI_BIGNUMS_H_INCLUDED

#include <iostream>

/*
lpi::fixed96q32 is a 128-bit fixed point number with fractional bits
*) 32 bits represent the part after the point
*) 95 bits represent the part before the point
*) 1 bit (the MSB) represents the sign, but two's complement is used

lpi::ufixed96q32 is the unsigned version.

This allows nanometer precision over a distance of 39614081257132168796771975168
metres or 4187211692790 lightyears, which is ideal for my space game project.

For this to work properly, it's important that the type "unsigned int" is
32-bit, not more, not less. But the C++ standard doesn't properly assure such
thing for any type.
*/

/*
There are also template functions available that work on arbitrary uint32 arrays.

This is a work in progress, for example it's planned to make the sqrt and /= functions
also templated like already done with the add and multiply functions.
*/

namespace lpi
{

typedef unsigned int uint32; //the 32-bit unsigned type used by the big numbers, must be exactly 32-bit!

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
    if(sign<size>(a)) for(int i = 0; i < size; i++) a[i] = 0xFFFFFFFFU;
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
      a[size - 1] = sign<size>(a) ? 0xFFFFFFFFU : 0U;
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

template<int size>
void setMSB(uint32* a, bool bit)
{
  if(bit) a[size - 1] |= 2147483648U;
  else a[size - 1] &= 2147483647;
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
    if(a[i] != 0xFFFFFFFFU) break; //it only carries over if this uint32 became 0 due to overflow
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

template<int size>
bool isZero(const uint32* a)
{
  for(int i = 0; i < size; i++) if(a[i] != 0U) return false;
  return true;
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
  for(int i = sizeb; i < sizeo; i++) b_copy[i] = getMSB<sizeb>(b) ? 0xFFFFFFFFU : 0U;
  
  for(int j = 0; j < sizeo * 32; j++)
  {
    if(lpi::getBit_signed<sizea>(a, j)) add<sizeo, 0, sizeo, 0>(out, out, b_copy);
    leftshift<sizeo>(b_copy, 1U);
  }
}

template<int size>
bool greaterthan_unsigned(const uint32* a, const uint32* b) //returns a > b
{
  for(int i = size - 1; i >= 0; i--) if(a[i] != b[i]) return a[i] > b[i];
  return false; //they're equal
}

template<int sizea, int fraca, int sizeb, int fracb>
bool greaterthan_unsigned(const uint32* a, const uint32* b) //returns a > b
{
  int shift = fraca - fracb;
  if(sizea - fraca > sizeb - fracb) //a has bigger MSB
  {
    for(int i = sizea - 1; i >= 0; i--)
    {
      uint32 bi = (i - shift >= 0 && i - shift < sizeb) ? b[i - shift] : 0;
      if(a[i] != bi) return a[i] > bi;
    }
    //now there are maybe some b's left
    //if(shift < 0) for(int i = -shift - 1; i >= 0; i--) if(b[i] != 0) return false;
    return false; //they're equal OR b is bigger
  }
  else //b has bigger MSB
  {
    for(int i = sizeb - 1; i >= 0; i--)
    {
      uint32 ai = (i + shift >= 0 && i - shift < sizea) ? a[i + shift] : 0;
      if(b[i] != ai) return ai > b[i];
    }
    //now there are maybe some a's left
    if(shift > 0) for(int i = shift - 1; i >= 0; i--) if(a[i] != 0) return true;
    return false; //they're equal
  }
}

template<int size>
bool smallerthan_unsigned(const uint32* a, const uint32* b)
{
  return greaterthan_unsigned<size>(b, a);
}

template<int sizea, int fraca, int sizeb, int fracb>
bool smallerthan_unsigned(const uint32* a, const uint32* b)
{
  return greaterthan_unsigned<sizea, fraca, sizeb, fracb>(b, a);
}

template<int size>
bool greaterthan_signed(const uint32* a, const uint32* b)
{
  if(sign<size>(a) && !sign<size>(b)) return false;
  if(!sign<size>(a) && sign<size>(b)) return true;
  
  return(greaterthan_unsigned<size>(a, b));
}

template<int sizea, int fraca, int sizeb, int fracb>
bool greaterthan_signed(const uint32* a, const uint32* b)
{
  if(sign<sizea>(a) && !sign<sizeb>(b)) return false;
  if(!sign<sizea>(a) && sign<sizeb>(b)) return true;
  
  return greaterthan_unsigned<sizea, fraca, sizeb, fracb>(a, b);
}

template<int size>
bool smallerthan_signed(const uint32* a, const uint32* b)
{
  return greaterthan_signed<size>(b, a);
}

template<int sizea, int fraca, int sizeb, int fracb>
bool smallerthan_signed(const uint32* a, const uint32* b)
{
  return greaterthan_signed<sizea, fraca, sizeb, fracb>(b, a);
}

/*
If some of the uint32's are fractional:
interpret the resulting bits as fractional and non fractional bits correctly yourself.
sizeo must be at least sizea
in fact sizeb must also be as big as sizea, or the step "left shift dividor until it is greater than or equal to divident" doesn't work
so remove the sizes or make 1 input size
the inputs are not const and will be modified! make copy yourself if needed.
*/
template<int sizeo, int sizea, int sizeb>
void divide_unsigned(uint32* out, uint32* divident, uint32* dividor)
{
  //quotient = divident / dividor
  
  int shift = 0;
  
  makeZero<sizeo>(out);
  
  if(!isZero<sizeb>(dividor)) //avoid division through 0
  {
    /*left shift dividor until it is greater than or equal to divident
    the comparision includes checks for the extra MSBs*/
    bool lostbit = false;
    while(smallerthan_unsigned<sizeb, 0, sizea, 0>(dividor, divident))
    {
      lostbit = getMSB<sizeb>(dividor);
      leftshift<sizeb>(dividor, 1);
      shift++;
      if(shift >= sizeb * 32) break; //shifted too much?
    }

    //if the dividor is now larger than the divident, right shift it again once
    if(lostbit || greaterthan_unsigned<sizeb, 0, sizea, 0>(divident, dividor))
    {
      rightshift_unsigned<sizeb>(dividor, 1);
      setMSB<sizeb>(dividor, lostbit);
      shift--;
    }
    
    while(shift >= 0)
    {
      leftshift<sizeo>(out, 1);
      
      if(!greaterthan_unsigned<sizeb, 0, sizea, 0>(dividor, divident)) //if dividor <= divident...
      {
        subtract<sizea, 0, sizeb, 0>(divident, divident, dividor);
        addLSB<sizeo>(out); //increment the LSB from the quotient, carrying the bit if needed
      }
      
      rightshift_unsigned<sizeb>(dividor, 1);
      shift--;
    }
  }
}

// /*
// If some of the uint32's are fractional:
// interpret the resulting bits as fractional and non fractional bits correctly yourself.
// sizeo must be at least sizea
// */
// template<int sizeo, int sizea, int sizeb>
// void divide_unsigned(uint32* out, const uint32* a, const uint32* b)
// {
//   //quotient = divident / dividor
//   //     out =        a /       b
//   /*
//   Without precaution, the result will exclude the digits behind the point.
//   So to solve: the divident is multiplied with 4 billion compared to the dividor
//   */
//   
//   uint32 dividor[sizeb]; for(int i = 0; i < sizeb; i++) dividor[i] = b[i];
//   uint32 divident[sizea]; for(int i = 0; i < sizea; i++) divident[i] = a[i];
//   
//   int shift = 0;
//   
//   uint32 thisMSB = 0U; //extra uint32 for *this (extra MSB's)
//   makeZero<sizeo>(out);
//   
//   if(!isZero<sizeb>(dividor)) //avoid division through 0
//   {
//     /*left shift dividor until it is greater than or equal to divident
//     the comparision includes checks for the extra MSBs*/
//     //while(b[sizeb - 1] < a[sizea - 1] || (dividorMSB == dividentMSB && dividor < divident))
//     while(smallerthan_unsigned<sizeb, 0, sizea, 0>(dividor, divident))
//     {
//       leftshift<sizeb>(dividor, 1);
//       shift++;
//     }
// 
//     //if the dividor is now larger than the divident, right shift it again once
//     //if(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))
//     if(greaterthan_unsigned<sizeb, 0, sizea, 0>(divident, dividor))
//     {
//       rightshift_unsigned<sizeb>(dividor, 1);
//       shift--;
//     }
//     
//     while(shift >= 0)
//     {
//       //left shift quotient
//       leftshift<sizeo>(out, 1);
//       
//       //if(!(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))) //if dividor <= divident...
//       if(!greaterthan_unsigned<sizeb, 0, sizea, 0>(dividor, divident)) //if dividor <= divident...
//       {
//         //subtract dividor from divident
//         subtract<sizea, 0, sizeb, 0>(divident, divident, dividor);
// 
//         //increment the LSB from the quotient, carrying to the whole left if needed
//         addLSB<sizeo>(out);
//       }
//       
//       //right shift dividor 
//       rightshift_unsigned<sizeb>(dividor, 1);
// 
//       shift--;
//     }
//   }
// }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class fixed96q32;

class ufixed96q32 //unsigned version, used in some functions of the signed version
{
  public:
    
    
    //ctors
    ufixed96q32(){}
    ufixed96q32(const ufixed96q32& other) { *this = other; }
    ufixed96q32(double other) { *this = other; }
    ufixed96q32(uint32 other) { *this = other; }
    ufixed96q32(const fixed96q32& other) { *this = other; }
    
    //conversions
    operator double() const; //this only returns a correct result if the double can hold this value
    void operator=(double d);
    operator uint32() const;
    void operator=(uint32 d);
    void operator=(const fixed96q32& o);

    //member operators
    ufixed96q32& operator+=(const ufixed96q32& rhs);
    ufixed96q32& operator-=(const ufixed96q32& rhs);
    
    ufixed96q32& operator+=(uint32 rhs);
    ufixed96q32& operator-=(uint32 rhs);
    
    ufixed96q32& operator+=(double rhs);
    ufixed96q32& operator-=(double rhs);
    
    ufixed96q32& operator>>=(uint32 shift);
    ufixed96q32& operator<<=(uint32 shift);
    
    ufixed96q32& operator&=(const ufixed96q32& rhs);
    ufixed96q32& operator|=(const ufixed96q32& rhs);
    ufixed96q32& operator^=(const ufixed96q32& rhs);
    
    ufixed96q32& operator*=(uint32 rhs);
    ufixed96q32& operator*=(const ufixed96q32& rhs);
    ufixed96q32& operator*=(double rhs);
    
    ufixed96q32& operator/=(const ufixed96q32& rhs);
    
    ufixed96q32& operator++();
    ufixed96q32 operator++(int);
    ufixed96q32& operator--();
    ufixed96q32 operator--(int);
    
    bool operator==(const ufixed96q32& rhs) const;
    bool operator==(uint32 rhs) const;
    bool operator>(const ufixed96q32& rhs) const;
    bool operator<(const ufixed96q32& rhs) const;
    bool operator>=(const ufixed96q32& rhs) const;
    bool operator<=(const ufixed96q32& rhs) const;

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
    void crudeMultiply(const ufixed96q32& rhs); //not really useful function
    void takeSqrt();
    
    friend class fixed96q32;
    
    private:
    /*
    uint32 is assumed to be 32 bit, it won't work at all if it isn't, incorrect overflows happen then.
    data[0] are the bits behind the point
    the LSB of data[0] is also the LSB of this ufixed96q32
    the LSB of data[1] is the LSB of the integer part of this ufixed96q32
    */
    uint32 data[4];
};


//free operators
ufixed96q32 operator-(const ufixed96q32& a); //does to the bits what "would" be done if it were signed, but it isn't
ufixed96q32 operator~(const ufixed96q32& a);

ufixed96q32 operator+(const ufixed96q32& a, const ufixed96q32& b);
ufixed96q32 operator-(const ufixed96q32& a, const ufixed96q32& b);
ufixed96q32 operator+(const ufixed96q32& a, uint32& b);
ufixed96q32 operator+(uint32& a, const ufixed96q32& b);
ufixed96q32 operator-(const ufixed96q32& a, uint32& b);

ufixed96q32 operator>>(const ufixed96q32& a, uint32 b);
ufixed96q32 operator<<(const ufixed96q32& a, uint32 b);

ufixed96q32 operator&(const ufixed96q32& a, const ufixed96q32& b);
ufixed96q32 operator|(const ufixed96q32& a, const ufixed96q32& b);
ufixed96q32 operator^(const ufixed96q32& a, const ufixed96q32& b);

ufixed96q32 operator*(const ufixed96q32& a, const ufixed96q32& b);
ufixed96q32 operator*(const ufixed96q32& a, uint32 b);
ufixed96q32 operator*(uint32 a, const ufixed96q32& b);
ufixed96q32 operator*(const ufixed96q32& a, double b);
ufixed96q32 operator*(double a, const ufixed96q32& b);

ufixed96q32 operator/(const ufixed96q32& a, const ufixed96q32& b);

ufixed96q32 sqrt(const ufixed96q32& a);

bool operator==(uint32 a, const ufixed96q32& b); //the other operator=='s are member functions since access to private data is needed

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class fixed96q32
{
  public:
    //ctors
    fixed96q32(){}
    fixed96q32(const fixed96q32& other) { *this = other; }
    fixed96q32(double other) { *this = other; }
    fixed96q32(uint32 other) { *this = other; }
    fixed96q32(int other) { *this = other; }
    fixed96q32(const ufixed96q32& other) { *this = other; }
    
    //conversions
    operator double() const; //this only returns a correct result if the double can hold this value
    void operator=(double d);
    operator uint32() const;
    void operator=(uint32 d);
    operator int() const;
    void operator=(int d);
    void operator=(const ufixed96q32& o);

    //member operators
    fixed96q32& operator+=(const fixed96q32& rhs);
    fixed96q32& operator-=(const fixed96q32& rhs);
    
    fixed96q32& operator+=(uint32 rhs);
    fixed96q32& operator-=(uint32 rhs);
    
    fixed96q32& operator+=(double rhs);
    fixed96q32& operator-=(double rhs);
    
    fixed96q32& operator>>=(uint32 shift);
    fixed96q32& operator<<=(uint32 shift);
    
    fixed96q32& operator&=(const fixed96q32& rhs);
    fixed96q32& operator|=(const fixed96q32& rhs);
    fixed96q32& operator^=(const fixed96q32& rhs);
    
    fixed96q32& operator*=(uint32 rhs);
    fixed96q32& operator*=(const fixed96q32& rhs);
    fixed96q32& operator*=(double rhs);
    
    fixed96q32& operator/=(const fixed96q32& rhs);
    
    fixed96q32& operator++();
    fixed96q32 operator++(int);
    fixed96q32& operator--();
    fixed96q32 operator--(int);
    
    bool operator==(const fixed96q32& rhs) const;
    bool operator==(uint32 rhs) const;
    bool operator>(const fixed96q32& rhs) const;
    bool operator<(const fixed96q32& rhs) const;
    bool operator>=(const fixed96q32& rhs) const;
    bool operator<=(const fixed96q32& rhs) const;

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
    void crudeMultiply(const fixed96q32& rhs); //not really useful function
    void takeSqrt();
    
    friend class ufixed96q32;
    
    private:
    /*
    uint32 is assumed to be 32 bit, it won't work at all if it isn't, incorrect overflows happen then.
    data[0] are the bits behind the point
    the LSB of data[0] is also the LSB of this fixed96q32
    the LSB of data[1] is the LSB of the integer part of this fixed96q32
    the MSB of data[3] is the sign (note that two's complement notation is used)
    */
    uint32 data[4];
};


//free operators
fixed96q32 operator-(const fixed96q32& a);
fixed96q32 operator~(const fixed96q32& a);

fixed96q32 operator+(const fixed96q32& a, const fixed96q32& b);
fixed96q32 operator-(const fixed96q32& a, const fixed96q32& b);
fixed96q32 operator+(const fixed96q32& a, uint32& b);
fixed96q32 operator+(uint32& a, const fixed96q32& b);
fixed96q32 operator-(const fixed96q32& a, uint32& b);

fixed96q32 operator>>(const fixed96q32& a, uint32 b);
fixed96q32 operator<<(const fixed96q32& a, uint32 b);

fixed96q32 operator&(const fixed96q32& a, const fixed96q32& b);
fixed96q32 operator|(const fixed96q32& a, const fixed96q32& b);
fixed96q32 operator^(const fixed96q32& a, const fixed96q32& b);

fixed96q32 operator*(const fixed96q32& a, const fixed96q32& b);
fixed96q32 operator*(const fixed96q32& a, uint32 b);
fixed96q32 operator*(uint32 a, const fixed96q32& b);
fixed96q32 operator*(const fixed96q32& a, double b);
fixed96q32 operator*(double a, const fixed96q32& b);

fixed96q32 operator/(const fixed96q32& a, const fixed96q32& b);

fixed96q32 sqrt(const fixed96q32& a);

bool sign(const fixed96q32& a);
fixed96q32 abs(const fixed96q32& a);

bool operator==(uint32 a, const fixed96q32& b); //the other operator=='s are member functions since access to private data is needed

} //namespace lpi


#endif

