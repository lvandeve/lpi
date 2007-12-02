 
/*
Copyright (c) 2005-2007 Lode Vandevenne
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of Lode Vandevenne nor the names of his contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "lpi_bignums.h"

namespace lpi
{

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


ufixed128& ufixed128::operator+=(const ufixed128& rhs)
{
  add<4>(data, data, rhs.data);
  return *this;
}

ufixed128& ufixed128::operator-=(const ufixed128& rhs)
{
  subtract<4>(data, data, rhs.data);
  return *this;
}

ufixed128& ufixed128::operator+=(uint32 rhs)
{
  add<4,1,1,0>(data, data, &rhs);
  return *this;
}

ufixed128& ufixed128::operator-=(uint32 rhs)
{
  subtract<4,1,1,0>(data, data, &rhs);
  return *this;
}

ufixed128& ufixed128::operator+=(double rhs)
{
  return (*this)+= ufixed128(rhs);
}

ufixed128& ufixed128::operator-=(double rhs)
{
  return (*this)-= ufixed128(rhs);
}

void ufixed128::addFractional(uint32 rhs)
{
  data[0] += rhs; //data[0], the fractional part, is left untouched
  if(data[0] < rhs) //if it has overflown
  for(int i = 1; i < 4; i++)
  {
    data[i]++;
    if(data[i] != 0U) break; //only if data[i] is 0 (4294967295 + 1), it has overflown and we should continue
  }
}

void ufixed128::subtractFractional(uint32 rhs)
{
  if(rhs <= data[0])
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
  }
  else //overflow case
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
    for(int i = 1; i < 4; i++)
    {
      data[i]--;
      if(data[i] != 4294967295U) break; //only if data[i] is 4294967295 (0 - 1), it has overflown and we should continue
    }
  }
}

ufixed128::operator double() const //this only returns a correct result if the double can hold this value
{
  double result = 0.0;
  
  double multiplier = 1.0 / 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    result += multiplier * data[i];
    multiplier *= 4294967296.0;
  }
  return result;
}

void ufixed128::operator=(double d)
{
  double multiplier = 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    data[i] = uint32(d * multiplier);
    multiplier /= 4294967296.0;
  }
}

void ufixed128::operator=(const fixed128& o)
{
  data[0] = o.data[0];
  data[1] = o.data[1];
  data[2] = o.data[2];
  data[3] = o.data[3];
}

ufixed128::operator uint32() const //this only returns a correct result if the double can hold this value
{
  return data[1];
}

void ufixed128::operator=(uint32 d)
{
  data[0] = 0;
  data[1] = d;
  data[2] = 0;
  data[3] = 0;
}

ufixed128& ufixed128::operator++()
{
  data[1]++;
  if(data[1] == 0)
  {
    data[2]++;
    if(data[2] == 0)
    {
      data[3]++;
    }
  }
  return *this;
}

ufixed128 ufixed128::operator++(int)
{
  ufixed128 result = *this;
  (*this)++;
  return result;
}

ufixed128& ufixed128::operator--()
{
  data[1]--;
  if(data[1] == 4294967295U)
  {
    data[2]--;
    if(data[2] == 4294967295U)
    {
      data[3]--;
    }
  }
  return *this;
}

ufixed128 ufixed128::operator--(int)
{
  ufixed128 result = *this;
  (*this)--;
  return result;
}

void ufixed128::addLSB()
{
  lpi::addLSB<4>(data);
}

void ufixed128::subtractLSB()
{
  lpi::subtractLSB<4>(data);
}

void ufixed128::bit_invert()
{
  lpi::bit_invert<4>(data);
}

void ufixed128::negate()
{
  lpi::negate<4>(data);
}

bool ufixed128::sign() const
{
  return data[3] > 2147483647U;
}

void ufixed128::takeSqrt()
{
  if(isZero()) return;

  ufixed128 result = *this;
  static const ufixed128 mask = ~(ufixed128(1U) >> 32U); //only the LSB is 0

  uint32 i;
  for(i = 0; result.isZero(); i++) result >>= 1U;
  result = (*this) >> (2 * i);
  for(;;)
  {
    ufixed128 temp = ufixed128((*this)>>1U) / result - (result>>1U);
    if((temp & mask).isZero()) break;
    else result += temp;
  }
  
  *this = result;
}

////////////////////////////////////////////////////////////////////////////////

ufixed128& ufixed128::operator/=(const ufixed128& rhs)
{
  //divident / dividor = quotient (= *this)
  /*
  Without precaution, the result will exclude the digits behind the point.
  So to solve: the divident is multiplied with 4 billion compared to the dividor
  */
  ufixed128 dividor = rhs;
  uint32 dividorMSB = 0U;
  uint32 dividentMSB = data[3]; //MSB's for the divident
  ufixed128 divident = ((*this) << 32U);
  
  int shift = 0;
  
  uint32 thisMSB = 0U; //extra uint32 for *this (extra MSB's)
  makeZero();
  
  if(!dividor.isZero()) //avoid division through 0
  {
    /*left shift dividor until it is greater than or equal to divident
    the comparision includes checks for the extra MSBs*/
    while(dividorMSB < dividentMSB || (dividorMSB == dividentMSB && dividor < divident))
    {
      dividorMSB <<= 1; if(dividor.data[3] & 2147483648U) dividorMSB |= 1; else dividorMSB &= 4294967294U;
      dividor <<= 1;
      shift++;
    }

    //if the dividor is now larger than the divident, right shift it again once
    if(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))
    {
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0
      
      shift--;
    }
    
    while(shift >= 0)
    {
      //left shift quotient
      thisMSB <<= 1; if(data[3] & 2147483648U) thisMSB |= 1; else thisMSB &= 4294967294U;
      (*this) <<= 1;
      
      if(!(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))) //if dividor <= divident...
      {
        //subtract dividor from divident, operator- implemented here including the extra MSBs
        {
          bool borrow = false;
          for(int i = 0; i < 4; i++)
          {
            if(borrow)
            {
              borrow = dividor.data[i] >= divident.data[i];
              divident.data[i] -= (dividor.data[i] + 1U);
            }
            else
            {
              borrow = dividor.data[i] > divident.data[i] ;
              divident.data[i] -= dividor.data[i];
            }
          }
          //the MSBs from divident
          if(borrow) dividentMSB -= (dividorMSB + 1U);
          else dividentMSB -= dividorMSB;
        }

        //increment the LSB from the quotient, carrying to the whole left if needed
        addLSB();
        if(isZero()) thisMSB++;
      }
      
      //right shift dividor 
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0

      shift--;
    }
  }
  
  return *this;
}

ufixed128& ufixed128::operator>>=(uint32 shift)
{
  rightshift_unsigned<4>(data, shift);
  return *this;
}

ufixed128& ufixed128::operator<<=(uint32 shift)
{
  leftshift<4>(data, shift);
  return *this;
}

void ufixed128::makeZero()
{
  data[0] = data[1] = data[2] = data[3] = 0U;
}

ufixed128& ufixed128::operator*=(uint32 rhs)
{
  ufixed128 a = *this;
  
  makeZero();
  
  for(int i = 0; i < 32; i++)
  {
    if((rhs >> i) & 1) (*this) += a;
    a <<= 1;
  }

  return *this;
}

ufixed128& ufixed128::operator*=(const ufixed128& rhs)
{
  //160 bit precision for multiplication: one 32-bit group extra, because at the end we need to do a right shift
  uint32 a[5] = { data[0], data[1], data[2], data[3], sign() ? 4294967295U : 0U };
  uint32 extra = 0U; //similar: 5th 32-bit group for *this, representing "data[4]"
  
  ufixed128 b = rhs;
  
  makeZero(); //result will be stored in *this, start at zero
  
  for(int j = 0; j < 128; j++)
  {
    if(b.getLSB()) //the code from the += operator implemented again, to fill the 5th uint32 (*this += a)
    {
      uint32 carry = 0U;
      for(int i = 0; i < 4; i++)
      {
        data[i] += a[i] + carry;
        if(!carry && data[i] < a[i]) carry = 1U; //if it has overflown
        else if(carry && data[i] <= a[i]) carry = 1U;
        else carry = 0U;
      }
      extra += a[4] + carry;
    }
    leftshift<5>(a, 1U);
      
    b >>= 1;
  }
  
  data[0] = data[1];
  data[1] = data[2];
  data[2] = data[3];
  data[3] = extra;

  return *this;
}

ufixed128& ufixed128::operator*=(double rhs)
{
  return operator*=((ufixed128)rhs);
}

//this one loses the leftmost 32 bits, but shows a simpler version of the multiplication implementation for documentation
void ufixed128::crudeMultiply(const ufixed128& rhs)
{
  ufixed128 a = *this;
  ufixed128 b = rhs;
  makeZero();
  for(int i = 0; i < 128; i++)
  {
    if(b.getLSB()) (*this) += a;
    a <<= 1;
    b >>= 1;
  }
  
  (*this) >>= 32; //the part behind the comma is smaller
}

bool ufixed128::operator==(const ufixed128& rhs) const
{
  return data[0] == rhs.data[0]
      && data[1] == rhs.data[1]
      && data[2] == rhs.data[2]
      && data[3] == rhs.data[3];
}

bool ufixed128::operator==(uint32 rhs) const
{
  return data[0] == 0U
      && data[1] == rhs
      && data[2] == 0U
      && data[3] == 0U;
}

bool ufixed128::operator>(const ufixed128& rhs) const
{
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] > rhs.data[0];
      }
      else return data[1] > rhs.data[1];
    }
    else return data[2] > rhs.data[2];
  }
  else return data[3] > rhs.data[3];
}

bool ufixed128::operator<(const ufixed128& rhs) const
{
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] < rhs.data[0];
      }
      else return data[1] < rhs.data[1];
    }
    else return data[2] < rhs.data[2];
  }
  else return data[3] < rhs.data[3];
}

bool ufixed128::operator>=(const ufixed128& rhs) const
{
  return !((*this) > rhs);
}

bool ufixed128::operator<=(const ufixed128& rhs) const
{
  return !((*this) > rhs);
}

ufixed128& ufixed128::operator&=(const ufixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] &= rhs.data[i];
  }
  
  return *this;
}

ufixed128& ufixed128::operator|=(const ufixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] |= rhs.data[i];
  }
  
  return *this;
}

ufixed128& ufixed128::operator^=(const ufixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] ^= rhs.data[i];
  }
  
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ufixed128 operator-(const ufixed128& a)
{
  ufixed128 result = a;
  result.negate();
  return result;
}


ufixed128 operator~(const ufixed128& a)
{
  ufixed128 result = a;
  result.bit_invert();
  return result;
}


ufixed128 operator+(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result += b;
  return result;
}

ufixed128 operator-(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result -= b;
  return result;
}

ufixed128 operator+(const ufixed128& a, uint32& b)
{
  ufixed128 result = a;
  result += b;
  return result;
}

ufixed128 operator+(uint32& a, const ufixed128& b)
{
  ufixed128 result = b;
  result += a;
  return result;
}

ufixed128 operator-(const ufixed128& a, uint32& b)
{
  ufixed128 result = a;
  result -= b;
  return result;
}

ufixed128 operator>>(const ufixed128& a, uint32 b)
{
  ufixed128 result = a;
  result >>= b;
  return result;
}

ufixed128 operator<<(const ufixed128& a, uint32 b)
{
  ufixed128 result = a;
  result <<= b;
  return result;
}

ufixed128 operator&(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result &= b;
  return result;
}

ufixed128 operator|(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result |= b;
  return result;
}

ufixed128 operator^(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result ^= b;
  return result;
}

ufixed128 operator*(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result *= b;
  return result;
}

ufixed128 operator*(const ufixed128& a, uint32 b)
{
  ufixed128 result = a;
  result *= b;
  return result;
}

ufixed128 operator*(uint32 a, const ufixed128& b)
{
  ufixed128 result = b;
  result *= a;
  return result;
}


ufixed128 operator*(const ufixed128& a, double b)
{
  ufixed128 result = a;
  result *= b;
  return result;
}

ufixed128 operator*(double a, const ufixed128& b)
{
  ufixed128 result = b;
  result *= a;
  return result;
}

ufixed128 operator/(const ufixed128& a, const ufixed128& b)
{
  ufixed128 result = a;
  result /= b;
  return result;
}

bool operator==(uint32 a, const ufixed128& b)
{
  return b == a;
}

ufixed128 sqrt(const ufixed128& a)
{
  ufixed128 result = a;
  result.takeSqrt();
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

fixed128& fixed128::operator+=(const fixed128& rhs)
{
  add<4>(data, data, rhs.data);
  return *this;
}

fixed128& fixed128::operator-=(const fixed128& rhs)
{
  subtract<4>(data, data, rhs.data);
  return *this;
}

fixed128& fixed128::operator+=(uint32 rhs)
{
  add<4,1,1,0>(data, data, &rhs);
  return *this;
}

fixed128& fixed128::operator-=(uint32 rhs)
{
  subtract<4,1,1,0>(data, data, &rhs);
  return *this;
}

fixed128& fixed128::operator+=(double rhs)
{
  return (*this)+= fixed128(rhs);
}

fixed128& fixed128::operator-=(double rhs)
{
  return (*this)-= fixed128(rhs);
}

void fixed128::addFractional(uint32 rhs)
{
  data[0] += rhs; //data[0], the fractional part, is left untouched
  if(data[0] < rhs) //if it has overflown
  for(int i = 1; i < 4; i++)
  {
    data[i]++;
    if(data[i] != 0U) break; //only if data[i] is 0 (4294967295 + 1), it has overflown and we should continue
  }
}

void fixed128::subtractFractional(uint32 rhs)
{
  if(rhs <= data[0])
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
  }
  else //overflow case
  {
    data[0] -= rhs; //data[0], the fractional part, is left untouched
    for(int i = 1; i < 4; i++)
    {
      data[i]--;
      if(data[i] != 4294967295U) break; //only if data[i] is 4294967295 (0 - 1), it has overflown and we should continue
    }
  }
}

fixed128::operator double() const //this only returns a correct result if the double can hold this value
{
  double result = 0.0;
  
  if(sign())
  {
    double multiplier = 1.0 / 4294967296.0;
    for(int i = 0; i < 4; i++)
    {
      result += multiplier * (~data[i]);
      multiplier *= 4294967296.0;
    }
    return -result - 1.0 / 4294967296.0;
  }
  else
  {
    double multiplier = 1.0 / 4294967296.0;
    for(int i = 0; i < 4; i++)
    {
      result += multiplier * data[i];
      multiplier *= 4294967296.0;
    }
    return result;
  }
}

void fixed128::operator=(double d)
{
  bool negative = d < 0.0;
  if(negative) d = -d;
  double multiplier = 4294967296.0;
  for(int i = 0; i < 4; i++)
  {
    data[i] = uint32(d * multiplier);
    multiplier /= 4294967296.0;
  }
  if(negative) negate();
}

void fixed128::operator=(const ufixed128& o)
{
  data[0] = o.data[0];
  data[1] = o.data[1];
  data[2] = o.data[2];
  data[3] = o.data[3];
}

fixed128::operator uint32() const //this only returns a correct result if the double can hold this value
{
  return data[1];
}

void fixed128::operator=(uint32 d)
{
  data[0] = 0;
  data[1] = d;
  data[2] = 0;
  data[3] = 0;
}

fixed128& fixed128::operator++()
{
  data[1]++;
  if(data[1] == 0)
  {
    data[2]++;
    if(data[2] == 0)
    {
      data[3]++;
    }
  }
  return *this;
}

fixed128 fixed128::operator++(int)
{
  fixed128 result = *this;
  (*this)++;
  return result;
}

fixed128& fixed128::operator--()
{
  data[1]--;
  if(data[1] == 4294967295U)
  {
    data[2]--;
    if(data[2] == 4294967295U)
    {
      data[3]--;
    }
  }
  return *this;
}

fixed128 fixed128::operator--(int)
{
  fixed128 result = *this;
  (*this)--;
  return result;
}

void fixed128::addLSB()
{
  lpi::addLSB<4>(data);
}

void fixed128::subtractLSB()
{
  lpi::subtractLSB<4>(data);
}

void fixed128::bit_invert()
{
  lpi::bit_invert<4>(data);
}

void fixed128::negate()
{
  lpi::negate<4>(data);
}

bool fixed128::sign() const
{
  return data[3] > 2147483647U;
}

void fixed128::takeSqrt()
{
  if(sign() || isZero())
  {
    makeZero();
    return;
  }

  ufixed128 result = *this;
  static const ufixed128 mask = ~(ufixed128(1U) >> 32U); //only the LSB is 0

  uint32 i;
  for(i = 0; result.isZero(); i++) result >>= 1U;
  result = (*this) >> (2 * i);
  for(;;)
  {
    ufixed128 temp = ufixed128((*this)>>1U) / result - (result>>1U);
    if((temp & mask).isZero()) break;
    else result += temp;
  }
  
  *this = result;
}

////////////////////////////////////////////////////////////////////////////////

fixed128& fixed128::operator/=(const fixed128& rhs)
{
  //divident / dividor = quotient (= *this)
  /*
  Without precaution, the result will exclude the digits behind the point.
  So to solve: the divident is multiplied with 4 billion compared to the dividor
  */
  bool negative = false; //sign
  if(sign()) { negative = !negative; negate(); }
  ufixed128 dividor = rhs;
  if(dividor.sign()) { negative = !negative; dividor.negate(); }
  uint32 dividorMSB = 0U;
  uint32 dividentMSB = data[3]; //MSB's for the divident
  ufixed128 divident = ((*this) << 32U);
  
  int shift = 0;
  
  uint32 thisMSB = 0U; //extra uint32 for *this (extra MSB's)
  makeZero();
  
  if(!dividor.isZero()) //avoid division through 0
  {
    /*left shift dividor until it is greater than or equal to divident
    the comparision includes checks for the extra MSBs*/
    while(dividorMSB < dividentMSB || (dividorMSB == dividentMSB && dividor < divident))
    {
      dividorMSB <<= 1; if(dividor.data[3] & 2147483648U) dividorMSB |= 1; else dividorMSB &= 4294967294U;
      dividor <<= 1;
      shift++;
    }

    //if the dividor is now larger than the divident, right shift it again once
    if(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))
    {
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0
      
      shift--;
    }
    
    while(shift >= 0)
    {
      //left shift quotient
      thisMSB <<= 1; if(data[3] & 2147483648U) thisMSB |= 1; else thisMSB &= 4294967294U;
      (*this) <<= 1;
      
      if(!(dividorMSB > dividentMSB || (dividorMSB == dividentMSB && dividor > divident))) //if dividor <= divident...
      {
        //subtract dividor from divident, operator- implemented here including the extra MSBs
        {
          bool borrow = false;
          for(int i = 0; i < 4; i++)
          {
            if(borrow)
            {
              borrow = dividor.data[i] >= divident.data[i];
              divident.data[i] -= (dividor.data[i] + 1U);
            }
            else
            {
              borrow = dividor.data[i] > divident.data[i] ;
              divident.data[i] -= dividor.data[i];
            }
          }
          //the MSBs from divident
          if(borrow) dividentMSB -= (dividorMSB + 1U);
          else dividentMSB -= dividorMSB;
        }

        //increment the LSB from the quotient, carrying to the whole left if needed
        addLSB();
        if(isZero()) thisMSB++;
      }
      
      //right shift dividor 
      dividor >>= 1;
      if(dividorMSB & 1U) dividor.data[3] |= 2147483648U; else dividor.data[3] &= 2147483647U;
      dividorMSB >>= 1; dividorMSB &= 2147483647U; //make extra sure the left bit is 0

      shift--;
    }

  if(negative) negate();
  }
  
  return *this;
}

fixed128& fixed128::operator>>=(uint32 shift)
{
  rightshift_signed<4>(data, shift);
  return *this;
}

fixed128& fixed128::operator<<=(uint32 shift)
{
  leftshift<4>(data, shift);
  return *this;
}

void fixed128::makeZero()
{
  data[0] = data[1] = data[2] = data[3] = 0U;
}

fixed128& fixed128::operator*=(uint32 rhs)
{
  fixed128 a = *this;
  
  makeZero();
  
  for(int i = 0; i < 32; i++)
  {
    if((rhs >> i) & 1) (*this) += a;
    a <<= 1;
  }

  return *this;
}

fixed128& fixed128::operator*=(const fixed128& rhs)
{
  //160 bit precision for multiplication: one 32-bit group extra, because at the end we need to do a right shift
  uint32 a[5] = { data[0], data[1], data[2], data[3], sign() ? 4294967295U : 0U };
  uint32 extra = 0U; //similar: 5th 32-bit group for *this, representing "data[4]"
  
  uint32 b[5] = { rhs.data[0], rhs.data[1], rhs.data[2], rhs.data[3], rhs.sign() ? 4294967295U : 0U };
  
  makeZero(); //result will be stored in *this, start at zero
  
  for(int j = 0; j < 160; j++)
  {
    if(lpi::getLSB(b)) //the code from the += operator implemented again, to fill the 5th uint32 (*this += a)
    {
      uint32 carry = 0U;
      for(int i = 0; i < 4; i++)
      {
        data[i] += a[i] + carry;
        if(!carry && data[i] < a[i]) carry = 1U; //if it has overflown
        else if(carry && data[i] <= a[i]) carry = 1U;
        else carry = 0U;
      }
      extra += a[4] + carry;
    }
    leftshift<5>(a, 1U);
    
    rightshift_signed<5>(b, 1U);
  }
  
  data[0] = data[1];
  data[1] = data[2];
  data[2] = data[3];
  data[3] = extra;

  return *this;
}

fixed128& fixed128::operator*=(double rhs)
{
  return operator*=((fixed128)rhs);
}

//this one loses the leftmost 32 bits, but shows a simpler version of the multiplication implementation for documentation
void fixed128::crudeMultiply(const fixed128& rhs)
{
  fixed128 a = *this;
  fixed128 b = rhs;
  makeZero();
  for(int i = 0; i < 128; i++)
  {
    if(b.getLSB()) (*this) += a;
    a <<= 1;
    b >>= 1;
  }
  
  (*this) >>= 32; //the part behind the comma is smaller
}

bool fixed128::operator==(const fixed128& rhs) const
{
  return data[0] == rhs.data[0]
      && data[1] == rhs.data[1]
      && data[2] == rhs.data[2]
      && data[3] == rhs.data[3];
}

bool fixed128::operator==(uint32 rhs) const
{
  return data[0] == 0U
      && data[1] == rhs
      && data[2] == 0U
      && data[3] == 0U;
}

bool fixed128::operator>(const fixed128& rhs) const
{
  if(sign() && !rhs.sign()) return false;
  if(!sign() && rhs.sign()) return true;
  
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] > rhs.data[0];
      }
      else return data[1] > rhs.data[1];
    }
    else return data[2] > rhs.data[2];
  }
  else return data[3] > rhs.data[3];
}

bool fixed128::operator<(const fixed128& rhs) const
{
  if(sign() && !rhs.sign()) return true;
  if(!sign() && rhs.sign()) return false;
  
  if(data[3] == rhs.data[3])
  {
    if(data[2] == rhs.data[2])
    {
      if(data[1] == rhs.data[1])
      {
        return data[0] < rhs.data[0];
      }
      else return data[1] < rhs.data[1];
    }
    else return data[2] < rhs.data[2];
  }
  else return data[3] < rhs.data[3];
}

bool fixed128::operator>=(const fixed128& rhs) const
{
  return !((*this) > rhs);
}

bool fixed128::operator<=(const fixed128& rhs) const
{
  return !((*this) > rhs);
}

fixed128& fixed128::operator&=(const fixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] &= rhs.data[i];
  }
  
  return *this;
}

fixed128& fixed128::operator|=(const fixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] |= rhs.data[i];
  }
  
  return *this;
}

fixed128& fixed128::operator^=(const fixed128& rhs)
{
  for(int i = 0; i < 4; i++)
  {
    data[i] ^= rhs.data[i];
  }
  
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

fixed128 operator-(const fixed128& a)
{
  fixed128 result = a;
  result.negate();
  return result;
}

fixed128 operator~(const fixed128& a)
{
  fixed128 result = a;
  result.bit_invert();
  return result;
}


fixed128 operator+(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result += b;
  return result;
}

fixed128 operator-(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result -= b;
  return result;
}

fixed128 operator+(const fixed128& a, uint32& b)
{
  fixed128 result = a;
  result += b;
  return result;
}

fixed128 operator+(uint32& a, const fixed128& b)
{
  fixed128 result = b;
  result += a;
  return result;
}

fixed128 operator-(const fixed128& a, uint32& b)
{
  fixed128 result = a;
  result -= b;
  return result;
}

fixed128 operator>>(const fixed128& a, uint32 b)
{
  fixed128 result = a;
  result >>= b;
  return result;
}

fixed128 operator<<(const fixed128& a, uint32 b)
{
  fixed128 result = a;
  result <<= b;
  return result;
}

fixed128 operator&(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result &= b;
  return result;
}

fixed128 operator|(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result |= b;
  return result;
}

fixed128 operator^(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result ^= b;
  return result;
}

fixed128 operator*(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result *= b;
  return result;
}

fixed128 operator*(const fixed128& a, uint32 b)
{
  fixed128 result = a;
  result *= b;
  return result;
}

fixed128 operator*(uint32 a, const fixed128& b)
{
  fixed128 result = b;
  result *= a;
  return result;
}


fixed128 operator*(const fixed128& a, double b)
{
  fixed128 result = a;
  result *= b;
  return result;
}

fixed128 operator*(double a, const fixed128& b)
{
  fixed128 result = b;
  result *= a;
  return result;
}

fixed128 operator/(const fixed128& a, const fixed128& b)
{
  fixed128 result = a;
  result /= b;
  return result;
}
bool operator==(uint32 a, const fixed128& b)
{
  return b == a;
}

fixed128 sqrt(const fixed128& a)
{
  fixed128 result = a;
  result.takeSqrt();
  return result;
}

bool sign(const fixed128& a)
{
  return a.sign();
}

fixed128 abs(const fixed128& a)
{
  if(a.sign()) return -a;
  else return a;
}


} //namespace lpi

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//test
#if 1
#include <iostream>

int main(){}

class Testfixed128
{
  public:
  
  void testaddsub()
  {
    using namespace lpi;
    std::cout.precision(20);
    
    std::cout << "testing addition and subtraction" << std::endl;
    
    fixed128 n = -5.0;
    fixed128 p = +5.0;
    
    std::cout<<(double)(p + p)<<std::endl;
    std::cout<<(double)(p + n)<<std::endl;
    std::cout<<(double)(n + p)<<std::endl;
    std::cout<<(double)(n + n)<<std::endl;
    std::cout<<(double)(p - p)<<std::endl;
    std::cout<<(double)(p - n)<<std::endl;
    std::cout<<(double)(n - p)<<std::endl;
    std::cout<<(double)(n - n)<<std::endl;
    
    std::cout << std::endl;
    
    fixed128 f = 5.5;
    fixed128 g = -5.5;
    uint32 i = 5;
    uint32 j = 6;

    std::cout<<(double)(f + i)<<std::endl;
    std::cout<<(double)(f - i)<<std::endl;
    std::cout<<(double)(g + i)<<std::endl;
    std::cout<<(double)(g - i)<<std::endl;
    std::cout<<(double)(f + j)<<std::endl;
    std::cout<<(double)(f - j)<<std::endl;
    std::cout<<(double)(g + j)<<std::endl;
    std::cout<<(double)(g - j)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testsmallmul()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing small multiplication" << std::endl;
    
    fixed128 a = 0.01;
    fixed128 b = 0.3;
    fixed128 c = -0.01;
    fixed128 d = -0.3;
    
    std::cout<<(double)(a*a)<<std::endl;
    std::cout<<(double)(b*b)<<std::endl;
    std::cout<<(double)(c*c)<<std::endl;
    std::cout<<(double)(d*d)<<std::endl;
    std::cout<<std::endl;
    std::cout<<(double)(a*b)<<std::endl;
    std::cout<<(double)(a*d)<<std::endl;
    std::cout<<(double)(c*b)<<std::endl;
    std::cout<<(double)(c*d)<<std::endl;
    std::cout<<std::endl;
    std::cout<<(double)(b*a)<<std::endl;
    std::cout<<(double)(d*a)<<std::endl;
    std::cout<<(double)(b*c)<<std::endl;
    std::cout<<(double)(d*c)<<std::endl;
    
    std::cout<<std::endl;
    
  }
  
  void testlargemul()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing large multiplication" << std::endl;
    
    fixed128 a = 8000000000.5;
    fixed128 b = 9000000000.5;
    fixed128 c = -8000000000.5;
    fixed128 d = -9000000000.5;
    
    std::cout<<(double)(a*b)<<std::endl;
    std::cout<<(double)(a*d)<<std::endl;
    std::cout<<(double)(c*b)<<std::endl;
    std::cout<<(double)(c*d)<<std::endl;
    
    fixed128 e = -614.488;
    std::cout<<(double)(e*e)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testintmul()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing multiplication with integer" << std::endl;
    
    fixed128 a = 8000000000.0;
    uint32 b = 5;
    
    std::cout<<(double)a<<" "<<b<<std::endl;
    
    a *= b;
    
    std::cout<<(double)a<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testdoublemul()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing multiplication with doubles" << std::endl;
    
    fixed128 a = 8000000000.0;
    
    std::cout<<(double)(a * 0.5)<<std::endl;
    std::cout<<(double)(a * -1.5)<<std::endl;
    std::cout<<(double)(a * 1000.0)<<std::endl;
    std::cout<<(double)(a * -1.0)<<std::endl;
    std::cout<<(double)(a * 0.0)<<std::endl;
    
    std::cout << std::endl;
    fixed128 b = -8000000000.0;
    
    std::cout<<(double)(b * 0.5)<<std::endl;
    std::cout<<(double)(b * -1.5)<<std::endl;
    std::cout<<(double)(b * 1000.0)<<std::endl;
    std::cout<<(double)(b * -1.0)<<std::endl;
    std::cout<<(double)(b * 0.0)<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testltgt()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing lesser than and greater than" << std::endl;
    
    fixed128 a = +5.0;
    fixed128 b = +6.0;
    fixed128 c = -5.0;
    fixed128 d = -6.0;
    
    std::cout<<(a < b)<<std::endl;
    std::cout<<(a < c)<<std::endl;
    std::cout<<(c < d)<<std::endl;
    std::cout<<(b < a)<<std::endl;
    std::cout<<(c < a)<<std::endl;
    std::cout<<(d < c)<<std::endl;
    std::cout << std::endl;
    std::cout<<(a > b)<<std::endl;
    std::cout<<(a > c)<<std::endl;
    std::cout<<(c > d)<<std::endl;
    std::cout<<(b > a)<<std::endl;
    std::cout<<(c > a)<<std::endl;
    std::cout<<(d > c)<<std::endl;
    std::cout<<std::endl;
  }
  
  void testsqrt()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing square root" << std::endl;
    
    fixed128 a = 1600000000.0;
    a.takeSqrt();
    std::cout<<(double)a<<std::endl;
    
    fixed128 b = 9.0;
    b.takeSqrt();
    std::cout<<(double)b<<std::endl;
    
    fixed128 c = 2.0;
    c.takeSqrt();
    std::cout<<(double)c<<std::endl;
    
    std::cout<<std::endl;
  }
  
  void testdiv()
  {
    using namespace lpi;
    std::cout.precision(30);
    
    std::cout << "testing division" << std::endl;
    
    fixed128 a = 3.0;
    a /= 2.0;
    std::cout<<(double)a<<std::endl;
    
    fixed128 b = -16000000000000.0;
    b /= 2.0;
    std::cout<<(double)b<<std::endl;
    
    fixed128 c = 0.005;
    c /= 2.0;
    std::cout<<(double)c<<std::endl;
    
    
    fixed128 e = 4U;
    e <<= 32U;
    e /= 2.0;
    e >>= 32U;
    std::cout<<(double)e<<std::endl;
    
    fixed128 d = 4U;
    d <<= 64U;
    d /= 2.0;
    d >>= 64U;
    std::cout<<(double)d<<std::endl;
    
    std::cout<<std::endl;
  }
  
  Testfixed128()
  {
    testaddsub();
    testsmallmul();
    testlargemul();
    testintmul();
    testdoublemul();
    testltgt();
    testsqrt();
    testdiv();
  }
} testfixed128;

#endif

