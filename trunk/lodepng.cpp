/*
LodePNG version 20071228

Copyright (c) 2005-2007 Lode Vandevenne

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

//The manual and changelog can be found in the header file "lodepng.h"

#include "lodepng.h"

#include <vector>
#include <string>
#include <fstream>

#define VERSION_STRING "20071228"

namespace LodeFlate //Deflate
{

////////////////////////////////////////////////////////////////////////////////
// ** Shared functions and data for Deflate compression and decompression **  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Reading and writing single bits and bytes from/to stream                   //
////////////////////////////////////////////////////////////////////////////////

void addBitToStream(size_t& bitpointer, std::vector<unsigned char>& bitstream, unsigned long bit)
{
  if(bitpointer % 8 == 0) bitstream.push_back(0); //add a new byte at the end
  bitstream.back() |=  (bit << (bitpointer & 0x7)); //earlier bit of huffman code is in a lesser significant bit of an earlier byte
  bitpointer++;
}

void addBitsToStream(size_t& bitpointer, std::vector<unsigned char>& bitstream, unsigned long value, size_t nbits)
{
  for(size_t i = 0; i < nbits; i++) addBitToStream(bitpointer, bitstream, (value >> i) & 1);
}

void addBitsToStreamReversed(size_t& bitpointer, std::vector<unsigned char>& bitstream, unsigned long value, size_t nbits)
{
  for(size_t i = 0; i < nbits; i++) addBitToStream(bitpointer, bitstream, (value >> (nbits - 1 - i)) & 1);
}

void setBitOfStream(size_t& bitpointer, unsigned char* bitstream, unsigned long bit)
{
  bitstream[bitpointer >> 3] |=  (bit << (bitpointer & 0x7)); //earlier bit of huffman code is in a lesser significant bit of an earlier byte
  bitpointer++;
}

unsigned long readBitFromStream(size_t& bitpointer, const unsigned char* bitstream)
{
  unsigned long result = (bitstream[bitpointer >> 3] >> (bitpointer & 0x7)) & 1;
  bitpointer++;
  return result;
}

unsigned long readBitsFromStream(size_t& bitpointer, const unsigned char* bitstream, size_t nbits)
{
  unsigned long result = 0;
  for(size_t i = 0; i < nbits; i++) result += (readBitFromStream(bitpointer, bitstream)) << i;
  return result;
}

const size_t FIRST_LENGTH_CODE_INDEX = 257;
const size_t LAST_LENGTH_CODE_INDEX = 285;
const size_t NUM_DEFLATE_CODE_SYMBOLS = 288; //256 literals, the end code, some length codes, and 2 unused codes
const size_t NUM_DISTANCE_SYMBOLS = 32; //the distance codes have their own symbols, 30 used, 2 unused
const size_t NUM_CODE_LENGTH_CODES = 19; //the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros

static const unsigned long lengthbase[29] //the base lengths represented by codes 257-285
  = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
static const unsigned long lengthextra[29] //the extra bits used by codes 257-285 (added to base length)
  = {0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,   4,   5,   5,   5,   5,   0};
static const unsigned long distancebase[30] //the base backwards distances (the bits of distance codes appear after length codes and use their own huffman tree)
  = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const unsigned long distanceextra[30] //the extra bits of backwards distances (added to base)
  = {0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,   6,   6,   7,   7,   8,   8,    9,    9,   10,   10,   11,   11,   12,    12,    13,    13};
static const unsigned long clcl[NUM_CODE_LENGTH_CODES] //the order in which "code length alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated
  = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

class HuffmanTree
{
  public:
  
  //given the code lengths (as stored in the PNG file), generate the tree as defined by Deflate. maxbitlen is the maximum bits that a code in the tree can have. return value is error.
  int makeFromLengths(const std::vector<unsigned long>& bitlen, unsigned long maxbitlen)
  {
    lengths = bitlen;
    numcodes = (unsigned long)bitlen.size(); //number of symbols
    this->maxbitlen = maxbitlen;
    return makeFromLengths();
  }
  
  //terminology used for the package-merge algorithm and the coin collector's problem
  struct Coin //a coin can be multiple coins (when they're merged)
  {
    std::vector<unsigned long> symbols;
    float weight; //the sum of all weights in this coin

    void operator+=(const Coin& c)
    {
      for(size_t i = 0; i < c.symbols.size(); i++) symbols.push_back(c.symbols[i]);
      weight += c.weight;
    }
  };
  
  void fillInCoins(std::vector<Coin>& coins, const std::vector<unsigned long>& frequencies, float sum)
  {
    for(unsigned long i = 0; i < frequencies.size(); i++)
    {
      if(frequencies[i] == 0) continue; //it's important to exclude symbols that aren't present
      Coin c;
      c.weight = frequencies[i] / sum;
      c.symbols.push_back(i);
      coins.push_back(c);
    }
    if(coins.size()) sort(&coins[0], coins.size());
  }
  
  /*
  This uses the package-merge algorithm to generate length-limited symbols for the Huffman tree
  the frequencies are the number of occurances of each symbol (by normalizing  you get the weight of each symbol)
  */
  void makeFromFrequencies(const std::vector<unsigned long>& frequencies, unsigned long maxbitlen)
  {
    numcodes = (unsigned long)frequencies.size(); //number of symbols
    this->maxbitlen = maxbitlen;
    
    size_t numpresent = 0; //the number of symbols that is present (has non-zero frequency)
    for(size_t i = 0; i < numcodes; i++) if(frequencies[i] > 0) numpresent++;
    
    if(numpresent == 0) //there are no symbols at all, bring the tree in a stable and safe state and then stop
    {
      lengths.resize(numcodes, 0);
      makeFromLengths();
      return;
    }
    
    float sum = 0.0f;
    for(size_t i = 0; i < numcodes; i++) sum += frequencies[i];
    
    std::vector<Coin> prev_row; //the previous row of coins
    std::vector<Coin> coins; //the coins of the currently calculated row

    //first row, lowest denominator
    fillInCoins(coins, frequencies, sum);

    for(size_t j = 1; j <= maxbitlen; j++) //each of the remaining rows
    {
      prev_row.swap(coins);
      coins.clear();

      for(size_t i = 0; i + 1 < prev_row.size(); i += 2)
      {
        coins.push_back(prev_row[i]);
        coins.back() += prev_row[i + 1];
      }
      if(j < maxbitlen)
      {
        fillInCoins(coins, frequencies, sum);
      }
    }
    
    //keep the coins with lowest weight, so that they add up to the amount of symbols - 1
    coins.resize(numpresent - 1);
    
    //calculate the lenghts of each symbol, as the amount of times a coin of each symbol is used
    lengths.clear();
    lengths.resize(numcodes, 0);
    for(size_t i = 0; i < coins.size(); i++)
    {
      Coin& c = coins[i];
      for(size_t j = 0; j < c.symbols.size(); j++) lengths[c.symbols[j]]++;
    }
    
    makeFromLengths();
  }
  
  /*Decodes a symbol from the tree
  if decoded is true, then result contains the symbol, otherwise it contains something unspecified (because the symbol isn't fully decoded yet)
  bit is the bit that you just read from the stream
  you have to decode a full symbol (let the decode function return true) before you can try to decode another one, otherwise the state isn't reset
  return value is error.*/
  int decode(bool& decoded, unsigned long& result, size_t& treepos, unsigned long bit) const
  {
    if(treepos >= numcodes) return 11; //error: you appeared outside the codetree
    
    result = tree2d[2 * treepos + bit];
    decoded = (result < numcodes);
    
    if(decoded) treepos = 0;
    else treepos = result - numcodes;
    
    return 0;
  }
  
  unsigned long getCode(size_t index) const { return tree1d[index]; }
  unsigned long getLength(size_t index) const { return lengths[index]; }
  unsigned long size() const { return numcodes; }
  
  private:
  /*tree2d: 2D representation of a huffman tree. The one dimension is "0" or "1", the other dimension
  contains all nodes and leaves of the tree.
  If a value is < NUMCODES, this node is a leaf and the value is the symbol
  If a value is >= NUMCODES, then this is a node and this represents the address in the vector of the next node
  (If a value is 32767, then it means that this value isn't filled in yet)
  This 2D representation is useful for the decoder*/
  std::vector<unsigned long> tree2d;
  
  //the tree representation used by the decoder. return value is error
  int make2DTree()
  {
    tree2d.resize(numcodes * 2);
    //convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means uninited, a value >= numcodes is an address to another bit, a value < numcodes is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as many columns as codes - 1
    //a good huffmann tree has N * 2 - 1 nodes, of which N - 1 are internal nodes. Here, the internal nodes are stored (what their 0 and 1 option point to). There is only memory for such good tree currently, if there are more nodes (due to too long length codes), error 55 will happen
    for(unsigned long n = 0;  n < numcodes * 2; n++) tree2d[n] = 32767; //32767 here means the tree2d isn't filled there yet
    unsigned long nodefilled = 0; //up to which node it is filled
    unsigned long treepos = 0; //position in the tree (1 of the numcodes columns)
    
    for(unsigned long n = 0; n < numcodes; n++) //the codes
    for(unsigned long i = 0; i < lengths[n]; i++) //the bits for this code
    {
      unsigned char bit = (unsigned char)((tree1d[n] >> (lengths[n] - i - 1)) & 1);
      if(treepos > numcodes - 2) return 55; //error 55: see description in header
      if(tree2d[2 * treepos + bit] == 32767) //not yet filled in
      {
        if(i + 1 == lengths[n]) //last bit
        {
          tree2d[2 * treepos + bit] = n; //put the current code in it
          treepos = 0;
        }
        else //put address of the next step in here, first that address has to be found of course (it's just nodefilled + 1)...
        {
          nodefilled++;
          tree2d[2 * treepos + bit] = nodefilled + numcodes; //addresses encoded with numcodes added to it
          treepos = nodefilled;
        }
      }
      else treepos = tree2d[2 * treepos + bit] - numcodes;
    }
    
    return 0;
  }
  
  void sort(Coin* data, size_t amount) //combsort
  {
    size_t gap = amount;
    bool swapped = false;
    while(gap > 1 || swapped)
    {
      gap = (gap * 10) / 13; //shrink factor 1.3
      if(gap == 9 || gap == 10) gap = 11; //combsort11
      if(gap < 1) gap = 1;
      swapped = false;
      for(size_t i = 0; i < amount - gap; i++)
      {
        size_t j = i + gap;
        if(data[j].weight < data[i].weight)
        {
          std::swap(data[i].weight, data[j].weight);
          data[i].symbols.swap(data[j].symbols);
          swapped = true;
        }
      }
    }
  }
  
  int makeFromLengths() //given that numcodes, lengths and maxbitlen are already filled in correctly. return value is error.
  {
    tree1d.resize(numcodes);
    std::vector<unsigned long> blcount(maxbitlen + 1, 0);
    std::vector<unsigned long> nextcode(maxbitlen + 1, 0);
    
    //step 1: count number of instances of each code length
    for(size_t bits = 0; bits < numcodes; bits++) blcount[lengths[bits]]++;
    //step 2: generate the nextcode values
    for(size_t bits = 1; bits <= maxbitlen; bits++) nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
    //step 3: generate all the codes
    for(size_t n = 0; n < numcodes; n++) if(lengths[n] != 0) tree1d[n] = nextcode[lengths[n]]++;
    return make2DTree();
  }
  
  /*tree1d: 1D representation of the huffman tree. The different codes of the tree as integers (length in bits is gotten
  from the lengths vector). This 1d representation is useful for the encoder*/
  std::vector<unsigned long> tree1d;
  
  std::vector<unsigned long> lengths; //the lengths of the codes of the 1d-tree
  
  unsigned long maxbitlen; //maximum number of bits a single code can get
  unsigned long numcodes; //number of symbols in the alphabet = number of codes
};

//get the tree of a deflated block with fixed tree, as specified in the deflate specification
int generateFixedTree(HuffmanTree& tree)
{
  std::vector<unsigned long> bitlen(NUM_DEFLATE_CODE_SYMBOLS);
    
  //288 possible codes: 0-255=literals, 256=endcode, 257-285=lengthcodes, 286-287=unused
  for(size_t i =   0; i <= 143; i++) bitlen[i] = 8;
  for(size_t i = 144; i <= 255; i++) bitlen[i] = 9;
  for(size_t i = 256; i <= 279; i++) bitlen[i] = 7;
  for(size_t i = 280; i <= 287; i++) bitlen[i] = 8;
  
  return tree.makeFromLengths(bitlen, 15);
}

int generateDistanceTree(HuffmanTree& tree)
{
  std::vector<unsigned long> bitlen(NUM_DISTANCE_SYMBOLS);
  
  //there are 32 distance codes, but 30-31 are unused
  for(size_t i = 0; i < NUM_DISTANCE_SYMBOLS; i++) bitlen[i] = 5;
  
  return tree.makeFromLengths(bitlen, 15);
}

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for Deflate decompression **                         //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Inflator                                                                   //
////////////////////////////////////////////////////////////////////////////////

class Inflator
{
  private:
  
  int error;
  
  //Huffman trees
  HuffmanTree codetree; //287 //the code tree for Huffman codes
  HuffmanTree codetreeD; //31 //the code tree for distance codes
  HuffmanTree codelengthcodetree; //18 //the code tree for code length codes

  //decode a single symbol from given list of bits with given code tree. return value is the symbol
  unsigned long huffmanDecodeSymbol(const unsigned char* in, size_t& bp, const HuffmanTree& codetree, size_t inlength)
  {
    size_t treepos = 0;
    bool decoded;
    unsigned long ct;
    for(;;)
    {
      if((bp & 0x07) == 0 && (bp >> 3) > inlength) { error = 10; return 0; } //error: end of input memory reached without endcode
      unsigned long bit = readBitFromStream(bp, in);
      error = codetree.decode(decoded, ct, treepos, bit);
      if(error) return 0; //stop, an error happened
      if(decoded) return ct;
    }
  }
  
  //get the tree of a deflated block with fixed tree, as specified in the deflate specification
  void getTreeInflateFixed(HuffmanTree& tree, HuffmanTree& treeD)
  {
    //error checking not done, this is fixed stuff, it works, it doesn't depend on the image
    generateFixedTree(tree);
    generateDistanceTree(treeD);
  }
  
  //get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
  void getTreeInflateDynamic(HuffmanTree& tree, HuffmanTree& treeD, const unsigned char* in, size_t& bp, size_t inlength)
  {
    //make sure that length values that aren't filled in will be 0, or a wrong tree will be generated
    std::vector<unsigned long> bitlen(NUM_DEFLATE_CODE_SYMBOLS, 0);
    std::vector<unsigned long> bitlenD(NUM_DISTANCE_SYMBOLS, 0);
    
    if(bp >> 3 >= inlength - 2) { error = 49; return; } //the bit pointer is or will go past the memory

    size_t HLIT =  readBitsFromStream(bp, in, 5) + FIRST_LENGTH_CODE_INDEX; //number of literal/length codes + 257. Unlike the spec, the value 257 is added to it here already
    size_t HDIST = readBitsFromStream(bp, in, 5) + 1; //number of distance codes. Unlike the spec, the value 1 is added to it here already
    size_t HCLEN = readBitsFromStream(bp, in, 4) + 4; //number of code length codes. Unlike the spec, the value 4 is added to it here already
    
    //read the code length codes out of 3 * (amount of code length codes) bits
    std::vector<unsigned long> codelengthcode(NUM_CODE_LENGTH_CODES); //the lengths of the tree used to decode the lengths of the dynamic tree
    for(size_t i = 0; i < NUM_CODE_LENGTH_CODES; i++)
    {
      if(i < HCLEN) codelengthcode[clcl[i]] = readBitsFromStream(bp, in, 3);
      else codelengthcode[clcl[i]]  = 0; //if not, it must stay 0
    }
    
    error = codelengthcodetree.makeFromLengths(codelengthcode, 7);
    if(hasError()) return;
    
    //now we can use this tree to read the lengths for the tree that this function will return
    size_t i = 0;
    while(i < HLIT + HDIST) //i is the current symbol we're reading in the part that contains the code lengths of lit/len codes and dist codes
    {
      unsigned long code = huffmanDecodeSymbol(in, bp, codelengthcodetree, inlength);
      if(hasError()) return;
      
      if(code <= 15) //a length code
      {
        if(i < HLIT) bitlen[i] = code;
        else bitlenD[i - HLIT] = code;
        i++;
      }
      else if(code == 16) //repeat previous
      {
        size_t replength = 3; //read in the 2 bits that indicate repeat length (3-6)
        if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
        
        replength += readBitsFromStream(bp, in, 2);
        
        unsigned long value; //set value to the previous code
        if((i - 1) < HLIT) value = bitlen[i - 1];
        else value = bitlenD[i - HLIT - 1];
        //repeat this value in the next lengths
        for(size_t n = 0; n < replength; n++)
        {
          if(i >= HLIT + HDIST) { error = 13; return; } //error: i is larger than the amount of codes
          if(i < HLIT) bitlen[i] = value;
          else bitlenD[i - HLIT] = value;
          i++;
        }
      }
      else if(code == 17) //repeat "0" 3-10 times
      {
        size_t replength = 3; //read in the bits that indicate repeat length
        if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
  
        replength += readBitsFromStream(bp, in, 3);
        
        //repeat this value in the next lengths
        for(size_t n = 0; n < replength; n++)
        {
          if(i >= HLIT + HDIST) { error = 14; return; } //error: i is larger than the amount of codes
          if(i < HLIT) bitlen[i] = 0;
          else bitlenD[i - HLIT] = 0;
          i++;
        }
      }
      else if(code == 18) //repeat "0" 11-138 times
      {
        size_t replength = 11; //read in the bits that indicate repeat length
  
        if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
  
        replength += readBitsFromStream(bp, in, 7);
        
        //repeat this value in the next lengths
        for(size_t n = 0; n < replength; n++)
        {
          if(i >= HLIT + HDIST) { error = 15; return; } //error: i is larger than the amount of codes
          if(i < HLIT) bitlen[i] = 0;
          else bitlenD[i - HLIT] = 0;
          i++;
        }
      }
      else { error = 16; return; } //error: somehow an unexisting code appeared. This can never happen.
    }
    
    if(bitlen[256] == 0) { error = 64; return; } //the length of the end code 256 must be larger than 0
    
    //now we've finally got HLIT and HDIST, so generate the code trees, and the function is done
    error = tree.makeFromLengths(bitlen, 15); if(hasError()) return;
    error = treeD.makeFromLengths(bitlenD, 15); if(hasError()) return;
  }
  
  //inflate a block with dynamic of fixed Huffman tree
  void inflateHuffmanBlock(std::vector<unsigned char>& out, const unsigned char* in, size_t& bp, size_t& pos, size_t inlength, unsigned long btype) 
  {
    if(btype == 1) getTreeInflateFixed(codetree, codetreeD);
    else if(btype == 2) getTreeInflateDynamic(codetree, codetreeD, in, bp, inlength);
    if(hasError()) return;
    
    bool endreached = false;
    while(!endreached)
    {
      unsigned long code = huffmanDecodeSymbol(in, bp, codetree, inlength);
      if(hasError()) return; //some error happened in the above function
      if(code == 256) //end code
      {
        endreached = true;
      }
      else if(code <= 255) //literal symbol
      {
        if(pos >= out.size())
        {
          out.resize((pos + 1) * 2); //reserve more room at once (std::vectors do this on their own too but this is more explicit)
        }
        out[pos] = (unsigned char)(code);
        pos++;
      }
      else if(code >= FIRST_LENGTH_CODE_INDEX && code <= LAST_LENGTH_CODE_INDEX) //length code
      {
        //part 1: get length base
        size_t length = lengthbase[code - FIRST_LENGTH_CODE_INDEX];
        
        //part 2: get extra bits and add the value of that to length
        size_t numextrabits = lengthextra[code - FIRST_LENGTH_CODE_INDEX];
        if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
        length += readBitsFromStream(bp, in, numextrabits);
        
        //part 3: get distance code
        unsigned long codeD = huffmanDecodeSymbol(in, bp, codetreeD, inlength);
        if(hasError()) return;
        if(codeD > 29) { error = 18; return; } //error: invalid distance code (30-31 are never used)
        unsigned long distance = distancebase[codeD];
        
        //part 4: get extra bits from distance
        unsigned long numextrabitsD = distanceextra[codeD];
  
        if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
        distance += readBitsFromStream(bp, in, numextrabitsD);
        
        //part 5: fill in all the out[n] values based on the length and dist
        size_t start = pos;
        size_t backward = start - distance;
        
        if(pos + length >= out.size())
        {
          out.resize((pos + length) * 2); //reserve more room at once (std::vectors do this on their own too but this is more explicit)
        }
        
        for(size_t forward = 0; forward < length; forward++)
        {
          out[pos] = out[backward];
          pos++;
          backward++;
          if(backward >= start) backward = start - distance;
        }
      }
    }
  }
  
  void inflateNoCompression(std::vector<unsigned char>& out, const unsigned char* in, size_t& bp, size_t& pos, size_t inlength)
  {
    //go to first boundary of byte
    while((bp & 0x7) != 0) bp++;
    size_t p = bp / 8; //byte position
    
    //read LEN (2 bytes) and NLEN (2 bytes)
    if(p >= inlength - 4) { error = 52; return; } //error, bit pointer will jump past memory
    unsigned long LEN = in[p] + 256 * in[p + 1]; p += 2;
    unsigned long NLEN = in[p] + 256 * in[p + 1]; p += 2;
    
    //check if 16-bit NLEN is really the one's complement of LEN
    if(LEN + NLEN != 65535) { error = 21; return; } //error: NLEN is not one's complement of LEN

    if(pos + LEN >= out.size()) out.resize(pos + LEN);
    
    //read the literal data: LEN bytes are now stored in the out buffer
    if(p + LEN > inlength) { error = 23; return; } //error: reading outside of in buffer
    for(unsigned long n = 0; n < LEN; n++) out[pos++] = in[p++];
    
    bp = p * 8;
  }
  
  public:
  
  int getError() { return error; }
  bool hasError() { return error != 0; }
  
  //inflate the deflated data (cfr. deflate spec)
  void inflate(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, size_t inpos = 0)
  {
    error = 0;
    
    size_t bp = 0; //bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte)
    unsigned long BFINAL = 0;
    size_t pos = 0; //byte position in the out buffer
    
    while(!BFINAL)
    {
      if(bp >> 3 >= in.size()) { error = 52; return; } //error, bit pointer will jump past memory
      BFINAL = readBitFromStream(bp, &in[inpos]);
      unsigned long BTYPE = 1 * readBitFromStream(bp, &in[inpos]); BTYPE += 2 * readBitFromStream(bp, &in[inpos]);
  
      if(BTYPE == 3) { error = 20; return; } //error: invalid BTYPE
      else if(BTYPE == 0) //no compression
      {
        inflateNoCompression(out, &in[inpos], bp, pos, in.size());
      }
      else //compression, BTYPE 01 or 10
      {
        inflateHuffmanBlock(out, &in[inpos], bp, pos, in.size(), BTYPE);
      }
      if(hasError()) return;
    }
    
    out.resize(pos); //Only now we know the true size of out, resize it to that
  }
};

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for Deflate compression **                           //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Deflator                                                                    //
////////////////////////////////////////////////////////////////////////////////

const size_t MAX_SUPPORTED_DEFLATE_LENGTH = 258;

//bitlen is the size in bits of the code
void addHuffmanSymbol(size_t& bp, std::vector<unsigned char>& compressed, unsigned long code, unsigned long bitlen)
{
  addBitsToStreamReversed(bp, compressed, code, bitlen);
}

//search the index in the array, that has the largest value smaller than or equal to the given value, given array must be sorted (if no value is smaller, it returns the size of the given array)
size_t searchCodeIndex(const long unsigned* array, size_t array_size, size_t value)
{
  //linear search implementation
  /*for(size_t i = 1; i < array_size; i++) if(array[i] > value) return i - 1;
  return array_size - 1;*/
  
  //binary search implementation (not that much faster) (precondition: array_size > 0)
  size_t left  = 1;
  size_t right = array_size - 1;
  while(left <= right)
  {
    size_t mid = (left + right) / 2;
    if(array[mid] <= value) left = mid + 1; //the value to find is more to the right
    else if(array[mid - 1] > value) right = mid - 1; //the value to find is more to the left
    else return mid - 1;
  }
  return array_size - 1;
}

void addLengthDistance(std::vector<int>& values, size_t length, size_t distance)
{
  /*values in encoded vector are those used by deflate:
  0-255: literal bytes
  256: end
  257-285: length/distance pair (length code, followed by extra length bits, distance code, extra distance bits)
  286-287: invalid*/

  //the length
  int length_code = (int)searchCodeIndex(lengthbase, 29, length);
  int extra_length = (int)(length - lengthbase[length_code]);
  values.push_back(length_code + FIRST_LENGTH_CODE_INDEX);
  values.push_back(extra_length);
  
  //the distance
  int dist_code = (int)searchCodeIndex(distancebase, 30, distance);
  int extra_distance = (int)(distance - distancebase[dist_code]);
  values.push_back(dist_code);
  values.push_back(extra_distance);
}

//the "brute force" version of the encodeLZ7 algorithm, not used anymore, kept here for reference
void encodeLZ77_brute(std::vector<int>& out, const unsigned char* in, size_t size, unsigned long windowSize)
{
  //using pointer instead of vector for input makes it faster when NOT using optimization when compiling; no influence if optimization is used
  for(size_t pos = 0; pos < size; pos++)
  {
    size_t length = 0, offset = 0; //the length and offset found for the current position
    size_t max_offset = pos < windowSize ? pos : windowSize; //how far back to test
  
    ///search for the longest string
    for(size_t current_offset = 1; current_offset < max_offset; current_offset++) //search backwards through all possible distances (=offsets)
    {
      size_t backpos = pos - current_offset;
      if(in[backpos] == in[pos])
      {
        //test the next characters
        size_t current_length = 1;
        size_t backtest = backpos + 1;
        size_t foretest = pos + 1;
        while(foretest < size && in[backtest] == in[foretest] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH) //maximum supporte length by deflate is max length
        {
          if(backpos >= pos) backpos -= current_offset; //continue as if we work on the decoded bytes after pos by jumping back before pos
          current_length++;
          backtest++;
          foretest++;
        }
        if(current_length > length)
        {
          length = current_length; //the longest length
          offset = current_offset; //the offset that is related to this longest length
          if(current_length == MAX_SUPPORTED_DEFLATE_LENGTH) break; //you can jump out of this for loop once a length of max length is found (gives significant speed gain)
        }
      }
    }
    
    ///encode it as length/distance pair or literal value
    if(length < 3) //only lengths of 3 or higher are supported as length/distance pair
    {
      out.push_back(in[pos]);
    }
    else
    {
      addLengthDistance(out, length, offset);
      pos += (length - 1);
    }
  } //end of the loop through each character of input
}

const unsigned long HASH_NUM_VALUES = 65536;
const unsigned long HASH_NUM_CHARACTERS = 6;
const unsigned long HASH_SHIFT = 2;
/*
Good and fast values: HASH_NUM_VALUES=65536, HASH_NUM_CHARACTERS=6, HASH_SHIFT=2
making HASH_NUM_CHARACTERS larger (like 8), makes the file size larger but is a bit faster
making HASH_NUM_CHARACTERS smaller (like 3), makes the file size smaller but is slower
*/

unsigned int getHash(const unsigned char* data, size_t size, size_t pos)
{
  unsigned int result = 0;
  if(pos >= size) return 0;
  size_t amount = HASH_NUM_CHARACTERS; if(pos + amount >= size) amount = size - pos;
  for(size_t i = 0; i < amount; i++) result ^= (data[pos + i] << (i * HASH_SHIFT));
  return result % HASH_NUM_VALUES;
}

//LZ77-encode the data using a hash table technique to let it encode faster.
void encodeLZ77(std::vector<int>& out, const unsigned char* in, size_t size, unsigned long windowSize)
{
  ///generate hash table
  std::vector<std::vector<size_t> > table(HASH_NUM_VALUES); //HASH_NUM_VALUES vectors
  
  //remember start and end positions in the tables to searching in
  std::vector<size_t> tablepos1(HASH_NUM_VALUES, 0);
  std::vector<size_t> tablepos2(HASH_NUM_VALUES, 0);
  
  //using pointer instead of vector for input makes it faster when NOT using optimization when compiling; no influence if optimization is used
  for(size_t pos = 0; pos < size; pos++)
  {
    size_t length = 0, offset = 0; //the length and offset found for the current position
    size_t max_offset = pos < windowSize ? pos : windowSize; //how far back to test
  
    ///search for the longest string
    //first find out where in the table to start (the first value that is in the range from "pos - max_offset" to "pos")
    unsigned int hash = getHash(in, size, pos);
    table[hash].push_back(pos);
    
    while(table[hash][tablepos1[hash]] < pos - max_offset) tablepos1[hash]++; //it now points to the first value in the table for which the index is larger than or equal to pos - max_offset
    while(table[hash][tablepos2[hash]] < pos) tablepos2[hash]++; //it now points to the first value in the table for which the index is larger than or equal to pos

    for(size_t tablepos = tablepos2[hash] - 1; tablepos >= tablepos1[hash] && tablepos < tablepos2[hash]; tablepos--)
    {
      size_t backpos = table[hash][tablepos];
      size_t current_offset = pos - backpos;

      //test the next characters
      size_t current_length = 0;
      size_t backtest = backpos;
      size_t foretest = pos;
      while(foretest < size && in[backtest] == in[foretest] && current_length < MAX_SUPPORTED_DEFLATE_LENGTH) //maximum supporte length by deflate is max length
      {
        if(backpos >= pos) backpos -= current_offset; //continue as if we work on the decoded bytes after pos by jumping back before pos
        current_length++;
        backtest++;
        foretest++;
      }
      if(current_length > length)
      {
        length = current_length; //the longest length
        offset = current_offset; //the offset that is related to this longest length
        if(current_length == MAX_SUPPORTED_DEFLATE_LENGTH) break; //you can jump out of this for loop once a length of max length is found (gives significant speed gain)
      }
    }
    
    ///encode it as length/distance pair or literal value
    if(length < 3) //only lengths of 3 or higher are supported as length/distance pair
    {
      out.push_back(in[pos]);
    }
    else
    {
      addLengthDistance(out, length, offset);
      //pos += (length - 1);
      for(size_t j = 0; j < length - 1; j++)
      {
        pos++;
        table[getHash(in, size, pos)].push_back(pos);
      }
    }
  } //end of the loop through each character of input
}

class Deflator
{
  private:
  
  unsigned long windowSize;
  bool useLZ77;
  
  void deflateNoCompression(std::vector<unsigned char>& out, const std::vector<unsigned char>& data)
  {
    //non compressed deflate block data: 1 bit BFINAL,2 bits BTYPE,(5 bits): it jumps to start of next byte, 2 bytes LEN, 2 bytes NLEN, LEN bytes literal DATA
    
    size_t numdeflateblocks = data.size() / 65536 + 1;
    unsigned long datapos = 0;
    for(size_t i = 0; i < numdeflateblocks; i++)
    {
      int BFINAL = (i == numdeflateblocks - 1);
      int BTYPE = 0;
      
      unsigned char firstbyte = (unsigned char)(BFINAL + ((BTYPE & 1) << 1) + ((BTYPE & 2) << 1));
      out.push_back(firstbyte);
      
      unsigned long LEN = 65535;
      if(data.size() - datapos < 65535) LEN = (unsigned long)data.size() - datapos;
      unsigned long NLEN = 65535 - LEN;
  
      out.push_back((unsigned char)(LEN % 256));
      out.push_back((unsigned char)(LEN / 256));
      out.push_back((unsigned char)(NLEN % 256));
      out.push_back((unsigned char)(NLEN / 256));
      
      //Decompressed data
      for(size_t j = 0; j < 65535 && datapos < data.size(); j++)
      {
        out.push_back(data[datapos++]);
      }
    }
  }
  
  //write the encoded data, using lit/len as well as distance codes
  void writeLZ77data(size_t& bp, std::vector<unsigned char>& out, const std::vector<int>& lz77_encoded, const HuffmanTree& codes, const HuffmanTree& codesD)
  {
    for(size_t i = 0; i < lz77_encoded.size(); i++)
    {
      int val = lz77_encoded[i];
      addHuffmanSymbol(bp, out, codes.getCode(val), codes.getLength(val));
      if(val > 256) //for a length code, 3 more things have to be added
      {
        int length_index = val - FIRST_LENGTH_CODE_INDEX;
        int n_length_extra_bits = lengthextra[length_index];
        int length_extra_bits = lz77_encoded[++i];
        addBitsToStream(bp, out, length_extra_bits, n_length_extra_bits);
        
        int distance_code = lz77_encoded[++i];
        addHuffmanSymbol(bp, out, codesD.getCode(distance_code), codesD.getLength(distance_code));
        
        int distance_index = distance_code;
        int n_distance_extra_bits = distanceextra[distance_index];
        int distance_extra_bits = lz77_encoded[++i];
        addBitsToStream(bp, out, distance_extra_bits, n_distance_extra_bits);
      }
    }
  }
  
  void deflateDynamic(std::vector<unsigned char>& out, const std::vector<unsigned char>& data)
  {
    /*
    after the BFINAL and BTYPE, the dynamic block consists out of the following:
    - 5 bits HLIT, 5 bits HDIST, 4 bits HCLEN
    - (HCLEN+4)*3 bits code lengths of code length alphabet
    - HLIT + 257 code lenghts of lit/length alphabet (encoded using the code length alphabet, + possible repetition codes 16, 17, 18)
    - HDIST + 1 code lengths of distance alphabet (encoded using the code length alphabet, + possible repetition codes 16, 17, 18)
    - compressed data
    - 256 (end code)
    */
    
    std::vector<int> lz77_encoded;
    if(useLZ77) encodeLZ77(lz77_encoded, data.empty() ? 0 : &data[0], data.size(), windowSize); //LZ77 encoded
    else for(size_t i = 0; i < data.size(); i++) lz77_encoded[i] = data[i]; //no LZ77, but still will be Huffman compressed

    
    std::vector<unsigned long> frequencies(286, 0);
    std::vector<unsigned long> frequenciesD(30, 0);
    for(size_t i = 0; i < lz77_encoded.size(); i++)
    {
      int symbol = lz77_encoded[i];
      frequencies[symbol]++;
      if(symbol > 256)
      {
        int dist = lz77_encoded[i + 2];
        frequenciesD[dist]++;
        i += 3;
      }
    }
    frequencies[256] = 1; //there will be exactly 1 end code, at the end of the block

    HuffmanTree codes; //tree for literal values and length codes
    codes.makeFromFrequencies(frequencies, 15);
    
    HuffmanTree codesD; //tree for distance codes
    codesD.makeFromFrequencies(frequenciesD, 15);
    
    bool BFINAL = 1; //make only one block... the first and final one
    size_t bp = 0; //the bit pointer
    
    addBitToStream(bp, out, BFINAL);
    addBitToStream(bp, out, 0); //first bit of BTYPE "dynamic"
    addBitToStream(bp, out, 1); //second bit of BTYPE "dynamic"
    std::vector<unsigned long> lldll; //lit/len & dist code lenghts
    unsigned long numcodes = (unsigned long)codes.size();
    if(numcodes > 286) numcodes = 286;
    unsigned long numcodesD = (unsigned long)codesD.size();
    if(numcodesD > 30) numcodesD = 30;
    for(unsigned long i = 0; i < numcodes; i++) lldll.push_back(codes.getLength(i));
    for(unsigned long i = 0; i < numcodesD; i++) lldll.push_back(codesD.getLength(i));
    
    //make lldl smaller by using repeat codes 16 (copy length 3-6 times), 17 (3-10 zeroes), 18 (11-138 zeroes)
    std::vector<unsigned long> lldl;
    for(unsigned long i = 0; i < (unsigned long)lldll.size(); i++)
    {
      unsigned long j = 0;
      while(i + j + 1 < (unsigned long)lldll.size() && lldll[i + j + 1] == lldll[i]) j++;
      
      if(lldll[i] == 0 && j >= 2)
      {
        j++; //include the first zero
        if(j <= 10) { lldl.push_back(17); lldl.push_back(j - 3); }
        else
        {
          if(j > 138) j = 138;
          lldl.push_back(18); lldl.push_back(j - 11);
        }
        i += (j - 1);
      }
      else if(j >= 3)
      {
        lldl.push_back(lldll[i]);
        unsigned long num = j / 6, rest = j % 6;
        for(size_t k = 0; k < num; k++) { lldl.push_back(16); lldl.push_back(   6 - 3); }
        if(rest >= 3)                   { lldl.push_back(16); lldl.push_back(rest - 3); }
        else j -= rest;
        i += j;
      }
      else lldl.push_back(lldll[i]);
    }
    //huffman tree voor de length codes van lit/len en dist codes genereren
    HuffmanTree codelengthcodes;
    std::vector<unsigned long> amounts; //the amounts in the "normal" order
    amounts.resize(19, 0); //16 possible lengths (0-15) and 3 repeat codes (16, 17 and 18)
    for(size_t i = 0; i < lldl.size(); i++)
    {
      amounts[lldl[i]]++;
      if(lldl[i] >= 16) i++; //after a repeat code come the bits that specify the amount, those don't need to be in the amounts calculation
    }
    
    codelengthcodes.makeFromFrequencies(amounts, 7);
    
    std::vector<unsigned long> clcls(19);
    for(size_t i = 0; i < 19; i++) clcls[i] = codelengthcodes.getLength(clcl[i]); //lenghts of code length tree is in the order as specified by deflate
    while(clcls[clcls.size() - 1] == 0 && clcls.size() > 0) clcls.pop_back(); //remove zeros at the end
    
    //write the HLIT, HDIST and HCLEN values
    unsigned long HLIT = numcodes - 257;
    unsigned long HDIST = numcodesD - 1;
    unsigned long HCLEN = (unsigned long)clcls.size() - 4;
    addBitsToStream(bp, out, HLIT, 5);
    addBitsToStream(bp, out, HDIST, 5);
    addBitsToStream(bp, out, HCLEN, 4);
    
    //write the code lenghts of the code length alphabet
    for(size_t i = 0; i < HCLEN + 4; i++) addBitsToStream(bp, out, clcls[i], 3);

    //write the lenghts of the lit/len AND the dist alphabet
    for(size_t i = 0; i < lldl.size(); i++)
    {
      addHuffmanSymbol(bp, out, codelengthcodes.getCode(lldl[i]), codelengthcodes.getLength(lldl[i]));
      //extra bits of repeat codes
      if(lldl[i] == 16) addBitsToStream(bp, out, lldl[++i], 2);
      else if(lldl[i] == 17) addBitsToStream(bp, out, lldl[++i], 3);
      else if(lldl[i] == 18) addBitsToStream(bp, out, lldl[++i], 7);
    }

    //write the compressed data symbols
    writeLZ77data(bp, out, lz77_encoded, codes, codesD);
    
    addHuffmanSymbol(bp, out, codes.getCode(256), codes.getLength(256)); //"end" code
  }
  
  void deflateFixed(std::vector<unsigned char>& out, const std::vector<unsigned char>& data)
  {
    HuffmanTree codes; //tree for literal values and length codes
    generateFixedTree(codes);
    
    HuffmanTree codesD; //tree for distance codes
    generateDistanceTree(codesD);
  
    bool BFINAL = 1; //make only one block... the first and final one
    size_t bp = 0; //the bit pointer
    
    addBitToStream(bp, out, BFINAL);
    addBitToStream(bp, out, 1); //first bit of BTYPE
    addBitToStream(bp, out, 0); //second bit of BTYPE
    
    if(useLZ77) //LZ77 encoded
    {
      std::vector<int> lz77_encoded;
      encodeLZ77(lz77_encoded, data.empty() ? 0 : &data[0], data.size(), windowSize);
      writeLZ77data(bp, out, lz77_encoded, codes, codesD);
    }
    else //no LZ77, but still will be Huffman compressed
    {
      for(size_t i = 0; i < data.size(); i++) addHuffmanSymbol(bp, out, codes.getCode(data[i]), codes.getLength(data[i]));
    }
    
    addHuffmanSymbol(bp, out, codes.getCode(256), codes.getLength(256)); //"end" code
  }
  
  public:
  
  void deflate(std::vector<unsigned char>& out, const std::vector<unsigned char>& data, unsigned long btype, unsigned long i_windowSize, bool i_useLZ77)
  {
    windowSize = i_windowSize;
    useLZ77 = i_useLZ77;
    if(btype == 0) deflateNoCompression(out, data);
    else if(btype == 1) deflateFixed(out, data);
    else if(btype == 2) deflateDynamic(out, data);
  }
};

} //end of namespace LodeFlate

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace LodeZlib //Zlib is deflate with extra header and ADLER32 checksum added
{

////////////////////////////////////////////////////////////////////////////////
// ** Shared functions and data for Zlib compression and decompression    **  //
////////////////////////////////////////////////////////////////////////////////

void add32bitInt(std::vector<unsigned char>& buffer, unsigned long value)
{
  buffer.push_back((unsigned char)((value >> 24) & 0xff));
  buffer.push_back((unsigned char)((value >> 16) & 0xff));
  buffer.push_back((unsigned char)((value >>  8) & 0xff));
  buffer.push_back((unsigned char)((value      ) & 0xff));
}

unsigned long read32bitInt(const unsigned char* buffer)
{
  return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

////////////////////////////////////////////////////////////////////////////////
// Adler32                                                                    //
////////////////////////////////////////////////////////////////////////////////

class Adler32
{
  public:
  unsigned long adler32(const unsigned char* data, unsigned long len) const;
  unsigned long update_adler32(unsigned long adler, const unsigned char* data, unsigned long len) const;
  
  private:
  static const unsigned long BASE = 65521; //largest prime smaller than 65536
};

unsigned long Adler32::update_adler32(unsigned long adler, const unsigned char* data, unsigned long len) const
{
   unsigned long s1 = adler & 0xffff;
   unsigned long s2 = (adler >> 16) & 0xffff;
   
  while(len > 0)
  {
    //at least 5550 sums can be done before the sums overflow, saving us from a lot of module divisions
    unsigned long amount = len > 5550 ? 5550 : len;
    len -= amount;
    while(amount > 0)
    {
      s1 = (s1 + *data++);
      s2 = (s2 + s1);
      amount--;
    }
    s1 %= BASE;
    s2 %= BASE;
  }
  
  return (s2 << 16) | s1;
}

//Return the adler32 of the bytes data[0..len-1]
unsigned long Adler32::adler32(const unsigned char* data, unsigned long len) const
{
  return update_adler32(1L, data, len);
}

Adler32 adler32;

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for Zlib decompression **                            //
////////////////////////////////////////////////////////////////////////////////

DecompressSettings::DecompressSettings()
{
  ignoreAdler32 = false;
}

//returns error value
int decompress(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, const DecompressSettings& settings)
{
  LodeFlate::Inflator inflator;
  
  if(in.size() < 2) { return 53; } //error, size of zlib data too small
  //read information from zlib header
  if((in[0] * 256 + in[1]) % 31 != 0) { return 24; } //error: 256 * in[0] + in[1] must be a multiple of 31, the FCHECK value is supposed to be made that way

  unsigned long CM = in[0] & 15;
  unsigned long CINFO = (in[0] >> 4) & 15;
  //unsigned long FCHECK = in[1] & 31; //FCHECK is already tested above
  unsigned long FDICT = (in[1] >> 5) & 1;
  //unsigned long FLEVEL = (in[1] >> 6) & 3; //not really important, all it does it to give a compiler warning about unused variable, we don't care what encoding setting the encoder used
  
  if(CM != 8 || CINFO > 7) { return 25; } //error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec
  if(FDICT != 0) { return 26; } //error: the specification of PNG says about the zlib stream: "The additional flags shall not specify a preset dictionary."
  
  inflator.inflate(out, in, 2);
  int error = inflator.getError();
  if(error != 0) return error;
  
  if(!settings.ignoreAdler32)
  {
    unsigned long ADLER32 = read32bitInt(&in[in.size() - 4]);
    unsigned long checksum = adler32.adler32(out.empty() ? 0 : &out[0], (unsigned long)out.size());
    if(checksum != ADLER32) return 58;
  }
  
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for Zlib compression    **                           //
////////////////////////////////////////////////////////////////////////////////

CompressSettings::CompressSettings()
{
  windowSize = 2048; //this is a good tradeoff between speed and compression ratio
  btype = 2;
  useLZ77 = true;
}

void compress(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, const CompressSettings& settings)
{
  LodeFlate::Deflator deflator;
  
  //zlib data: 1 byte CMF (CM+CINFO), 1 byte FLG, deflate data, 4 byte ADLER32 checksum of the Decompressed data
  int CMF = 120; //0b01111000: CM 8, CINFO 7. With CINFO 7, any window size up to 32768 can be used.
  int FLEVEL = 0;
  int FDICT = 0;
  int CMFFLG = 256 * CMF + FDICT * 32 + FLEVEL * 64;
  int FCHECK = 31 - CMFFLG % 31;
  CMFFLG += FCHECK;
  out.push_back((unsigned char)(CMFFLG / 256));
  out.push_back((unsigned char)(CMFFLG % 256));
  
  std::vector<unsigned char> deflatedata;
  deflator.deflate(deflatedata, in, settings.btype, settings.windowSize, settings.useLZ77);
  
  unsigned long ADLER32 = adler32.adler32(in.empty() ? 0 : &in[0], (unsigned long)in.size());
  for(size_t i = 0; i < deflatedata.size(); i++) out.push_back(deflatedata[i]);
  
  add32bitInt(out, ADLER32);
}

} //end of namespace LodeZlib

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace LodePNG
{

////////////////////////////////////////////////////////////////////////////////
// ** Shared functions and data for PNG encoding and decoding **              //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Reading and writing single bits and bytes from/to stream                   //
////////////////////////////////////////////////////////////////////////////////

unsigned long readBitFromReversedStream(size_t& bitpointer, const unsigned char* bitstream)
{
  unsigned long result = (bitstream[bitpointer >> 3] >> (7 - bitpointer & 0x7)) & 1;
  bitpointer++;
  return result;
}

unsigned long readBitsFromReversedStream(size_t& bitpointer, const unsigned char* bitstream, size_t nbits)
{
  unsigned long result = 0;
  for(size_t i = nbits - 1; i < nbits; i--) result += ((readBitFromReversedStream(bitpointer, bitstream)) << i);
  return result;
}

void setBitOfReversedStream(size_t& bitpointer, unsigned char* bitstream, unsigned long bit)
{
  bitstream[bitpointer >> 3] |=  (bit << (7 - bitpointer & 0x7)); //earlier bit of huffman code is in a lesser significant bit of an earlier byte
  bitpointer++;
}

unsigned long read32bitInt(const unsigned char* buffer)
{
  return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

void add32bitInt(std::vector<unsigned char>& buffer, unsigned long value)
{
  buffer.push_back((unsigned char)((value >> 24) & 0xff));
  buffer.push_back((unsigned char)((value >> 16) & 0xff));
  buffer.push_back((unsigned char)((value >>  8) & 0xff));
  buffer.push_back((unsigned char)((value      ) & 0xff));
}

////////////////////////////////////////////////////////////////////////////////
// PNG color types                                                            //
////////////////////////////////////////////////////////////////////////////////

//return type is a LodePNG error code
int checkColorValidity(unsigned long colorType, unsigned long bitDepth)
{
  const unsigned long& bd = bitDepth; //shorter variable name makes code below more easily readable
  switch(colorType)
  {
    case 0: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16)) return 37; break; //grey
    case 2: if(!(                                 bd == 8 || bd == 16)) return 37; break; //RGB
    case 3: if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8            )) return 37; break; //palette
    case 4: if(!(                                 bd == 8 || bd == 16)) return 37; break; //grey + alpha
    case 6: if(!(                                 bd == 8 || bd == 16)) return 37; break; //RGBA
    default: return 31;
  }
  return 0; //allowed color type / bits combination
}

unsigned long getNumColorChannels(unsigned long colorType, unsigned long /*bitDepth*/)
{
  switch(colorType)
  {
    case 0: return 1; //grey
    case 2: return 3; //RGB
    case 3: return 1; //palette
    case 4: return 2; //grey + alpha
    case 6: return 4; //RGBA
  }
  return 0; //unexisting color type
}

unsigned long getBpp(unsigned long colorType, unsigned long bitDepth)
{
  return getNumColorChannels(colorType, bitDepth) * bitDepth; //bits per pixel is amount of channels * bits per channel
}

////////////////////////////////////////////////////////////////////////////////
//Info, InfoPNG and InfoRaw                                                   //
////////////////////////////////////////////////////////////////////////////////

Info::Info()
{
  key_defined = false;
  colorType = 6;
  bitDepth = 8;
}

unsigned long Info::getBpp() const { return LodePNG::getBpp(colorType, bitDepth); }
unsigned long Info::getChannels() const { return getNumColorChannels(colorType, bitDepth); }
bool Info::isGreyscaleType() const { return colorType == 0 || colorType == 4; }
bool Info::isAlphaType() const { return (colorType & 4) != 0; }

InfoPng::InfoPng()
{
  background_defined = false;
}

void InfoPng::addText(const char* key, const char* str)
{
  text_keys.push_back(key);
  text_strings.push_back(str);
}

bool operator==(const Info& info1, const Info& info2)
{
  return info1.colorType == info2.colorType
      && info1.bitDepth  == info2.bitDepth; //palette and color key not compared
}

bool operator!=(const Info& info1, const Info& info2)
{
  return !(info1 == info2);
}

////////////////////////////////////////////////////////////////////////////////
//Convert                                                                     //
////////////////////////////////////////////////////////////////////////////////

//converts from any color type to 24-bit or 32-bit (later maybe more supported). return value = LodePNG error code
int convert(std::vector<unsigned char>& out, const unsigned char* in, Info& infoOut, Info& infoIn, unsigned long w, unsigned long h)
{
  out.clear();
  
  //cases where in and out already have the same format
  if(infoIn == infoOut)
  {
    unsigned long size = (w * h * infoIn.getBpp() + 7) / 8;
    out.insert(out.end(), &in[0], &in[size]);
    return 0;
  }

  const size_t numpixels = w * h; //amount of pixels
  const unsigned long OUT_BYTES = infoOut.getBpp() / 8; //bytes per pixel in the output image
  const bool          OUT_ALPHA = infoOut.isAlphaType();//use 8-bit alpha channel
  out.resize(numpixels * OUT_BYTES);
  
  unsigned char* out_ = out.empty() ? 0: &out[0]; //faster if compiled without optimization
  size_t bp = 0; //used by less-than-8-bit color types
    
  if((infoOut.colorType == 2 || infoOut.colorType == 6) && infoOut.bitDepth == 8)
  {
    if(infoIn.bitDepth == 8)
    {
      switch(infoIn.colorType)
      {
        case 0: //greyscale color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            out_[OUT_BYTES * i + 0] = out_[OUT_BYTES * i + 1] = out_[OUT_BYTES * i + 2] = in[i];
            if(OUT_ALPHA && infoIn.key_defined && in[i] == infoIn.key_r) out_[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 2: //RGB color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            for(size_t c = 0; c < 3; c++) out_[OUT_BYTES * i + c] = in[3 * i + c];
            if(OUT_ALPHA && infoIn.key_defined == 1 && in[3 * i + 0] == infoIn.key_r && in[3 * i + 1] == infoIn.key_g && in[3 * i + 2] == infoIn.key_b) out_[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 3: //indexed color (palette)
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            if(4U * in[i] >= infoIn.palette.size()) return 46;
            for(size_t c = 0; c < OUT_BYTES; c++) out_[OUT_BYTES * i + c] = infoIn.palette[4 * in[i] + c]; //get rgb colors from the palette
          }
        break;
        case 4: //greyscale with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            out_[OUT_BYTES * i + 0] = out_[OUT_BYTES * i + 1] = out_[OUT_BYTES * i + 2] = in[2 * i + 0];
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = in[2 * i + 1];
          }
        break;
        case 6: //RGB with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            for(size_t c = 0; c < OUT_BYTES; c++) out_[OUT_BYTES * i + c] = in[4 * i + c];
          }
        break;
        default: break;
      }
    }
    else if(infoIn.bitDepth == 16)
    {
      switch(infoIn.colorType)
      {
        case 0: //greyscale color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            out_[OUT_BYTES * i + 0] = out_[OUT_BYTES * i + 1] = out_[OUT_BYTES * i + 2] = in[2 * i];
            if(OUT_ALPHA && infoIn.key_defined && 256U * in[i] + in[i + 1] == infoIn.key_r) out_[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 2: //RGB color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            for(size_t c = 0; c < 3; c++) out_[OUT_BYTES * i + c] = in[6 * i + 2 * c];
            if(OUT_ALPHA && infoIn.key_defined && 256U * in[6 * i + 0] + in[6 * i + 1] == infoIn.key_r && 256U * in[6 * i + 2] + in[6 * i + 3] == infoIn.key_g && 256U * in[6 * i + 4] + in[6 * i + 5] == infoIn.key_b) out_[OUT_BYTES * i + 3] = 0;
          }
        break;
        case 4: //greyscale with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            out_[OUT_BYTES * i + 0] = out_[OUT_BYTES * i + 1] = out_[OUT_BYTES * i + 2] = in[4 * i]; //most significant byte
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = in[4 * i + 2];
          }
        break;
        case 6: //RGB with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            for(size_t c = 0; c < OUT_BYTES; c++) out_[OUT_BYTES * i + c] = in[8 * i + 2 * c];
          }
          break;
        default: break;
      }
    }
    else //infoIn.bitDepth is less than 8 bit per channel
    {
      switch(infoIn.colorType)
      {
        case 0: //greyscale color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            unsigned long value = readBitsFromReversedStream(bp, in, infoIn.bitDepth);
            if(OUT_ALPHA && infoIn.key_defined && value && ((1U << infoIn.bitDepth) - 1U) == infoIn.key_r && ((1U << infoIn.bitDepth) - 1U)) out_[OUT_BYTES * i + 3] = 0;
            value = (value * 255) / ((1 << infoIn.bitDepth) - 1); //scale value from 0 to 255
            out_[OUT_BYTES * i + 0] = out_[OUT_BYTES * i + 1] = out_[OUT_BYTES * i + 2] = (unsigned char)(value);
          }
        break;
        case 3: //indexed color (palette)
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 3] = 255;
            unsigned long value = readBitsFromReversedStream(bp, in, infoIn.bitDepth);
            if(4 * value >= infoIn.palette.size()) return 47;
            for(size_t c = 0; c < OUT_BYTES; c++) out_[OUT_BYTES * i + c] = infoIn.palette[4 * value + c]; //get rgb colors from the palette
          }
        break;
        default: break;
      }
    }
  }
  else if(infoOut.isGreyscaleType() && infoOut.bitDepth == 8) //conversion from greyscale to greyscale
  {
    if(!infoIn.isGreyscaleType()) return 62;
    if(infoIn.bitDepth == 8)
    {
      switch(infoIn.colorType)
      {
        case 0: //greyscale color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 1] = 255;
            out_[OUT_BYTES * i] = in[i];
            if(OUT_ALPHA && infoIn.key_defined && in[i] == infoIn.key_r) out_[OUT_BYTES * i + 1] = 0;
          }
        break;
        case 4: //greyscale with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            out_[OUT_BYTES * i + 0] = in[2 * i + 0];
            if(OUT_ALPHA) out_[OUT_BYTES * i + 1] = in[2 * i + 1];
          }
        break;
        default: return 31;
      }
    }
    else if(infoIn.bitDepth == 16)
    {
      switch(infoIn.colorType)
      {
        case 0: //greyscale color
          for(size_t i = 0; i < numpixels; i++)
          {
            if(OUT_ALPHA) out_[OUT_BYTES * i + 1] = 255;
            out_[OUT_BYTES * i] = in[2 * i];
            if(OUT_ALPHA && infoIn.key_defined && 256U * in[i] + in[i + 1] == infoIn.key_r) out_[OUT_BYTES * i + 1] = 0;
          }
        break;
        case 4: //greyscale with alpha
          for(size_t i = 0; i < numpixels; i++)
          {
            out_[OUT_BYTES * i] = in[4 * i]; //most significant byte
            if(OUT_ALPHA) out_[OUT_BYTES * i + 1] = in[4 * i + 2]; //most significant byte
          }
        break;
        default: return 31;
      }
    }
    else //infoIn.bitDepth is less than 8 bit per channel
    {
      if(infoIn.colorType != 0) return 31; //colorType 0 is the only greyscale type with < 8 bits per channel

      for(size_t i = 0; i < numpixels; i++)
      {
        if(OUT_ALPHA) out_[OUT_BYTES * i + 1] = 255;
        unsigned long value = readBitsFromReversedStream(bp, in, infoIn.bitDepth);
        if(OUT_ALPHA && infoIn.key_defined && value && ((1U << infoIn.bitDepth) - 1U) == infoIn.key_r && ((1U << infoIn.bitDepth) - 1U)) out_[OUT_BYTES * i + 1] = 0;
        value = (value * 255) / ((1 << infoIn.bitDepth) - 1); //scale value from 0 to 255
        out_[OUT_BYTES * i] = (unsigned char)(value);
      }
    }
  }
  else return 59;
  
  return 0;
}

//Paeth predicter, used by PNG filter type 4
long paethPredictor(long a, long b, long c)
{
  long p = a + b - c;
  long pa = p > a ? p - a : a - p;
  long pb = p > b ? p - b : b - p;
  long pc = p > c ? p - c : c - p;
  
  if(pa <= pb && pa <= pc) return a;
  else if(pb <= pc) return b;
  else return c;
}

////////////////////////////////////////////////////////////////////////////////
// CRC32                                                                      //
////////////////////////////////////////////////////////////////////////////////

class Crc32
{
  public:
  unsigned long update_crc(const unsigned char* buf, unsigned long crc, size_t len);
  unsigned long crc(const unsigned char* buf, size_t len);

  private:
  static unsigned long crc_table[256]; //Table of CRCs of all 8-bit messages.
  static bool crc_table_computed; //Flag: has the table been computed? Initially false.
  static void make_crc_table(void); //Make the table for a fast CRC.
};

bool Crc32::crc_table_computed = false;
unsigned long Crc32::crc_table[256];

//Make the table for a fast CRC.
void Crc32::make_crc_table(void)
{
  for(unsigned long n = 0; n < 256; n++)
  {
    unsigned long c = n;
    for(unsigned long k = 0; k < 8; k++)
    {
      if(c & 1) c = 0xedb88320L ^ (c >> 1);
      else c = c >> 1;
    }
    crc_table[n] = c;
  }
  crc_table_computed = true;
}

/*Update a running CRC with the bytes buf[0..len-1]--the CRC should be 
initialized to all 1's, and the transmitted value is the 1's complement of the
final running CRC (see the crc() routine below).*/
unsigned long Crc32::update_crc(const unsigned char* buf, unsigned long crc, size_t len)
{
  unsigned long c = crc;
  size_t n;

  if(!crc_table_computed) make_crc_table();
  for(n = 0; n < len; n++)
  {
    c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
  }
  return c;
}

//Return the CRC of the bytes buf[0..len-1].
unsigned long Crc32::crc(const unsigned char* buf, size_t len)
{
  return update_crc(buf, 0xffffffffL, len) ^ 0xffffffffL;
}

Crc32 crc32;

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for PNG decoding **                                  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PNG Decoder                                                                //
////////////////////////////////////////////////////////////////////////////////

//read the information from the header and store it in the Info
void Decoder::readPngHeader(const unsigned char* in, size_t inlength)
{
  if(inlength < 29) { error = 27; return; } //error: the data length is smaller than the length of the header

  if(in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71 || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10) { error = 28; return; } //error: the first 8 bytes are not the correct PNG signature
  if(in[12] != 'I' || in[13] != 'H' || in[14] != 'D' || in[15] != 'R') { error = 29; return; } //error: it doesn't start with a IHDR chunk!
  
  //read the values given in the header
  infoPng.width = read32bitInt(&in[16]);
  infoPng.height = read32bitInt(&in[20]);
  infoPng.bitDepth = in[24];
  infoPng.colorType = in[25];
  infoPng.compressionMethod = in[26];
  infoPng.filterMethod = in[27];
  infoPng.interlaceMethod = in[28];

  if(!settings.ignoreCrc)
  {
    unsigned long CRC = read32bitInt(&in[29]);
    unsigned long checksum = crc32.crc(&in[12], 17);
    if(CRC != checksum) { error = 57; return; }
  }
  
  if(infoPng.compressionMethod != 0) { error = 32; return; } //error: only compression method 0 is allowed in the specification
  if(infoPng.filterMethod != 0)      { error = 33; return; } //error: only filter method 0 is allowed in the specification
  if(infoPng.interlaceMethod > 1)    { error = 34; return; } //error: only interlace methods 0 and 1 exist in the specification
  
  error = checkColorValidity(infoPng.colorType, infoPng.bitDepth);
  if(error) return;
}

//filter a PNG image scanline by scanline. when the pixels are smaller than 1 byte, the filter works byte per byte (bytewidth = 1)
//precon is the previous filtered scanline, recon the result, scanline the current one
void Decoder::unFilterScanline(unsigned char* recon, const unsigned char* scanline, const unsigned char* precon, size_t bytewidth, unsigned char filterType, size_t length)
{
  switch(filterType)
  {
    case 0:
      for(size_t i = 0; i < length; i++) recon[i] = scanline[i];
      break;
    case 1:
      for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
      for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + recon[i - bytewidth];
      break;
    case 2: 
      if(precon) for(size_t i = 0; i < length; i++) recon[i] = scanline[i] + precon[i];
      else       for(size_t i = 0; i < length; i++) recon[i] = scanline[i];
      break;
    case 3:
      if(precon)
      {
        for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i] + precon[i] / 2;
        for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) / 2);
      }
      else
      {
        for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
        for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + recon[i - bytewidth] / 2;
      }
      break;
    case 4:
      if(precon)
      {
        for(size_t i =         0; i < bytewidth; i++) recon[i] = (unsigned char)(scanline[i] + paethPredictor(0, precon[i], 0));
        for(size_t i = bytewidth; i <    length; i++) recon[i] = (unsigned char)(scanline[i] + paethPredictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
      }
      else
      {
        for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
        for(size_t i = bytewidth; i <    length; i++) recon[i] = (unsigned char)(scanline[i] + paethPredictor(recon[i - bytewidth], 0, 0));
      }
      break;
    default: error = 36; return; //error: unexisting filter type given
  }
}

//filter and reposition the pixels into the output when the image is Adam7 interlaced. This function can only do it after the full image is already decoded. The out buffer must have the correct allocated memory size already.
void Decoder::adam7Pass(unsigned char* out, unsigned char* linen, unsigned char* lineo, const unsigned char* in, unsigned long w, size_t passleft, size_t passtop, size_t spacex, size_t spacey, size_t passw, size_t passh, unsigned long bpp)
{
  if(passw == 0) return; //empty pass, no data, no filtertype
  size_t bytewidth = (bpp + 7) / 8;
  for(unsigned long y = 0; y < passh; y++)
  {
    size_t linelength = 1 + ((bpp * passw + 7) / 8); //filterbyte + pixel bytes
    size_t linestart = y * linelength; //position where we read the filterType: at the start of the scanline
    unsigned char filterType = in[linestart];
    
    unsigned char* prevline = (y == 0) ? 0 : lineo;
    unFilterScanline(linen, &in[linestart + 1], prevline,  bytewidth, filterType, (w * bpp + 7) / 8);
    if(hasError()) return;
    
    //put the filtered pixels in the output image
    if(bpp >= 8)
    {
      for(size_t i = 0; i < passw; i++)
      for(size_t b = 0; b < bytewidth; b++) //b = current byte of this pixel
      {
        out[bytewidth * w * (passtop + spacey * y) + bytewidth * (passleft + spacex * i) + b] = linen[bytewidth * i + b];
      }
    }
    else
    {
      for(size_t i = 0; i < passw; i++)
      {
        size_t obp = bpp * w * (passtop + spacey * y) + bpp * (passleft + spacex * i);
        size_t bp = i * bpp;
        
        for(size_t b = 0; b < bpp; b++) //b = current bit of this pixel
        {
          unsigned long bit = readBitFromReversedStream(bp, &linen[0]);
          setBitOfReversedStream(obp, out, bit);
        }
      }
    }

    //swap the two buffer pointers "line old" and "line new"
    unsigned char* temp = linen;
    linen = lineo;
    lineo = temp;
  }
}

void Decoder::resetParameters()
{
  error = 0; //initially no error happened yet
  infoPng.background_defined = infoPng.key_defined = 0; //initialize info variables that aren't necessarily set later on
  infoPng.text_keys.clear();
  infoPng.text_strings.clear();
  infoPng.palette.clear();
}

void Decoder::decode(std::vector<unsigned char>& out, const unsigned char* in, unsigned long size)
{
  std::vector<unsigned char> data; //the decompressed pixel data
  decodeGeneric(data, in, size);
  if(hasError()) return;
  
  if(!settings.color_convert || (Info)infoRaw == (Info)infoPng)
  {
    out.swap(data); //same color type, no copying or converting of data needed
  }
  else
  {
   //TODO: check if this works according to the statement in the documentation: "The converter can convert from greyscale input color type, to 8-bit greyscale or greyscale with alpha"
    if(!(infoRaw.colorType == 2 || infoRaw.colorType == 6) && !(infoRaw.bitDepth == 8)) { error = 56; return; }

    error = convert(out, &data[0], infoRaw, infoPng, infoPng.width, infoPng.height);
    if(hasError()) return;
  }
}

void Decoder::decode(std::vector<unsigned char>& out, const std::vector<unsigned char>& in)
{
  decode(out, in.empty() ? 0 : &in[0], (unsigned long)in.size());
}

void Decoder::inspect(const unsigned char* in, unsigned long size)
{
  if(size == 0 || in == 0) { error = 48; return; } //the given data is empty
  readPngHeader(in, size);
}

void Decoder::inspect(const std::vector<unsigned char>& in)
{
  inspect(in.empty() ? 0 : &in[0], (unsigned long)in.size());
}

//read a PNG, the result will be in the same color type as the PNG
void Decoder::decodeGeneric(std::vector<unsigned char>& out, const unsigned char* in, unsigned long size)
{
  if(size == 0 || in == 0) { error = 48; return; } //the given data is empty
  
  resetParameters(); //when decoding a new PNG image, make sure all parameters created after previous decoding are reset

  readPngHeader(in, size);
  if(hasError()) return;

  size_t pos = 33; //first byte of the first chunk after the header
  
  std::vector<unsigned char> idat; //the data from idat chunks
  
  bool IEND = false;
  bool known_type = true;
  while(!IEND) //loop through the chunks, ignoring unknown chunks and stopping at IEND chunk. IDAT data is put at the start of the in buffer
  {
    //get chunk length
    if(pos + 8 >= size) { error = 30; return; } //error: size of the in buffer too small to contain next chunk
    size_t chunkLength = read32bitInt(&in[pos]); pos += 4;
    if(chunkLength > 2147483647) { error = 63; return; }
    if(pos + chunkLength >= size) { error = 35; return; } //error: size of the in buffer too small to contain next chunk
    //IDAT chunk, containing compressed image data
    if(in[pos + 0] == 'I' && in[pos + 1] == 'D' && in[pos + 2] == 'A' && in[pos + 3] == 'T')
    {
      pos += 4;
      //idat.insert(idat.end(), in.begin() + pos, in.begin() + pos + chunkLength);
      idat.insert(idat.end(), &in[pos], &in[pos + chunkLength]);
      pos += chunkLength;
    }
    //IEND chunk
    else if(in[pos + 0] == 'I' && in[pos + 1] == 'E' && in[pos + 2] == 'N' && in[pos + 3] == 'D')
    {
      pos += 4;
      IEND = true;
    }
    //palette chunk (PLTE)
    else if(in[pos + 0] == 'P' && in[pos + 1] == 'L' && in[pos + 2] == 'T' && in[pos + 3] == 'E')
    {
      pos += 4; //go after the 4 letters
      infoPng.palette.resize(4 * (chunkLength / 3));
      if(infoPng.palette.size() > (4 * 256)) { error = 38; return; } //error: palette too big
      for(size_t i = 0; i < infoPng.palette.size(); i += 4)
      {
        infoPng.palette[i + 0] = in[pos++]; //R
        infoPng.palette[i + 1] = in[pos++]; //G
        infoPng.palette[i + 2] = in[pos++]; //B
        infoPng.palette[i + 3] = 255; //alpha
      }
    }
    //palette transparency chunk (tRNS)
    else if(in[pos + 0] == 't' && in[pos + 1] == 'R' && in[pos + 2] == 'N' && in[pos + 3] == 'S')
    {
      pos += 4; //go after the 4 letters
      if(infoPng.colorType == 3)
      {
        if(4 * chunkLength > infoPng.palette.size()) { error = 39; return; } //error: more alpha values given than there are palette entries
        for(size_t i = 0; i < chunkLength; i++) infoPng.palette[4 * i + 3] = in[pos++];
      }
      else if(infoPng.colorType == 0)
      {
        if(chunkLength != 2) { error = 40; return; } //error: this chunk must be 2 bytes for greyscale image
        infoPng.key_defined = 1;
        infoPng.key_r = infoPng.key_g = infoPng.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
      }
      else if(infoPng.colorType == 2)
      {
        if(chunkLength != 6) { error = 41; return; } //error: this chunk must be 6 bytes for RGB image
        infoPng.key_defined = 1;
        infoPng.key_r = 256 * in[pos] + in[pos + 1]; pos += 2;
        infoPng.key_g = 256 * in[pos] + in[pos + 1]; pos += 2;
        infoPng.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
      }
      else { error = 42; return; } //error: tRNS chunk not allowed for other color models
    }
    //background color chunk (bKGD)
    else if(in[pos + 0] == 'b' && in[pos + 1] == 'K' && in[pos + 2] == 'G' && in[pos + 3] == 'D')
    {
      pos += 4; //go after the 4 letters
      if(infoPng.colorType == 3)
      {
        if(chunkLength != 1) { error = 43; return; } //error: this chunk must be 1 byte for indexed color image
        infoPng.background_defined = 1;
        infoPng.background_r = infoPng.background_g = infoPng.background_g = in[pos++];
      }
      else if(infoPng.colorType == 0 || infoPng.colorType == 4)
      {
        if(chunkLength != 2) { error = 44; return; } //error: this chunk must be 2 bytes for greyscale image
        infoPng.background_defined = 1;
        infoPng.background_r = infoPng.background_g = infoPng.background_b = 256 * in[pos] + in[pos + 1]; pos += 2;
      }
      else if(infoPng.colorType == 2 || infoPng.colorType == 6)
      {
        if(chunkLength != 6) { error = 45; return; } //error: this chunk must be 6 bytes for greyscale image
        infoPng.background_defined = 1;
        infoPng.background_r = 256 * in[pos] + in[pos + 1]; pos += 2;
        infoPng.background_g = 256 * in[pos] + in[pos + 1]; pos += 2;
        infoPng.background_b = 256 * in[pos] + in[pos + 1]; pos += 2;
      }
    }
    //text chunk (tEXt)
    else if(in[pos + 0] == 't' && in[pos + 1] == 'E' && in[pos + 2] == 'X' && in[pos + 3] == 't')
    {
      pos += 4; //go after the 4 letters
      if(settings.readTextChunks)
      {
        size_t chunk_end = pos + chunkLength;
        std::string key, str;
        while(pos < chunk_end && in[pos] != 0) key += in[pos++];
        pos++;
        while(pos < chunk_end) str += in[pos++];
        infoPng.addText(key.c_str(), str.c_str());
      }
      else pos += chunkLength;
    }
    else //it's not an implemented chunk type, so ignore it: skip over the data
    {
      if(!(in[pos + 0] & 32)) { error = 69; return; } //error: unknown critical chunk (5th bit of first byte of chunk type is 0)
      pos += (chunkLength + 4); //skip 4 letters and uninterpreted data of unimplemented chunk
      known_type = false;
    }
    
    if(!settings.ignoreCrc && known_type) //check CRC if wanted, only on known chunk types
    {
      unsigned long CRC = read32bitInt(&in[pos]);
      size_t chunkStart = pos - chunkLength - 4; //the CRC is taken of the data and the 4 chunk type letters, not the length
      unsigned long checksum = crc32.crc(&in[chunkStart], chunkLength + 4);
      if(CRC != checksum) { error = 57; return; }
    }
    pos += 4; //step over CRC
  }

  unsigned long bpp = infoPng.getBpp();
  size_t scanlength = ((infoPng.width * (infoPng.height * bpp + 7)) / 8) + infoPng.height; //scanline buffer length is larger than final image size because up to h * 7 filter type codes can still be in it! (if there's interlacing)
  std::vector<unsigned char> scanlines(scanlength); //now the out buffer will be filled
  
  //convert from LodePNG settings to the settings of the Zlib decompressor
  LodeZlib::DecompressSettings decompressSettings;
  decompressSettings.ignoreAdler32 = settings.ignoreAdler32;
  
  //decompress with the Zlib decompressor
  error = LodeZlib::decompress(scanlines, idat, decompressSettings);
  
  //stop if the zlib decompressor returned an error
  if(hasError()) return;
  
  //filter and interlace
  size_t bytewidth = (bpp + 7) / 8; //bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise
  size_t outlength = (infoPng.height * infoPng.width * bpp + 7) / 8;

  out.resize(outlength); //time to fill the out buffer
  unsigned char* out_ = outlength ? &out[0] : 0; //use a regular pointer to the std::vector for faster code if compiled without optimization

  if(infoPng.interlaceMethod == 0)
  {
    size_t linestart = 0; //start of current scanline
    size_t linelength = (infoPng.width * bpp + 7) / 8; //length in bytes of a scanline, excluding the filtertype byte
    
    if(bpp >= 8) //byte per byte
    {
      for(size_t y = 0; y < infoPng.height; y++)
      {
        unsigned char filterType = scanlines[linestart];
        const unsigned char* prevline = (y == 0) ? 0 : &out_[(y - 1) * infoPng.width * bytewidth];
        unFilterScanline(&out_[linestart - y], &scanlines[linestart + 1], prevline, bytewidth, filterType,  linelength);
        if(hasError()) return;
        linestart += (1 + linelength); //go to start of next scanline
      }
    }
    else //less than 8 bits per pixel, so fill it up bit per bit
    {
      std::vector<unsigned char> templine((infoPng.width * bpp + 7) >> 3); //only used if bpp < 8
      size_t obp = 0; //out bit pointer, only used if bpp < 8
      for(size_t y = 0; y < infoPng.height; y++)
      {
        unsigned char filterType = scanlines[linestart];
        const unsigned char* prevline = (y == 0) ? 0 : &out_[(y - 1) * infoPng.width * bytewidth];
        unFilterScanline(&templine[0], &scanlines[linestart + 1], prevline, bytewidth, filterType, linelength);
        if(hasError()) return;

        size_t bp = 0;
        while(bp < infoPng.width * bpp)
        {
          unsigned long bit = readBitFromReversedStream(bp, &templine[0]);
          setBitOfReversedStream(obp, out_, bit);
        }
        linestart += (1 + linelength); //go to start of next scanline
      }
    }
  }
  else //interlaceMethod is 1 (Adam7)
  {
    size_t passw[7], passh[7], passstart[7];
    
    passw[0] = (infoPng.width + 7) / 8; passh[0] = (infoPng.height + 7) / 8;
    passw[1] = (infoPng.width + 3) / 8; passh[1] = (infoPng.height + 7) / 8;
    passw[2] = (infoPng.width + 3) / 4; passh[2] = (infoPng.height + 3) / 8;
    passw[3] = (infoPng.width + 1) / 4; passh[3] = (infoPng.height + 3) / 4;
    passw[4] = (infoPng.width + 1) / 2; passh[4] = (infoPng.height + 1) / 4;
    passw[5] = (infoPng.width + 0) / 2; passh[5] = (infoPng.height + 1) / 2;
    passw[6] = (infoPng.width + 0) / 1; passh[6] = (infoPng.height + 0) / 2;
    
    passstart[0] = 0;
    for(int i = 0; i < 6; i++)
      passstart[i + 1] = passstart[i] + passh[i] * ((passw[i] ? 1 : 0) + (passw[i] * bpp + 7) / 8); //if passw[i] is 0, it's 0 bytes, not 1 (no filtertype-byte)
    
    std::vector<unsigned char> scanlineo((infoPng.width * bpp + 7) / 8); //"old" scanline
    std::vector<unsigned char> scanlinen((infoPng.width * bpp + 7) / 8); //"new" scanline
    
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[0]], infoPng.width, 0, 0, 8, 8, passw[0], passh[0], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[1]], infoPng.width, 4, 0, 8, 8, passw[1], passh[1], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[2]], infoPng.width, 0, 4, 4, 8, passw[2], passh[2], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[3]], infoPng.width, 2, 0, 4, 4, passw[3], passh[3], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[4]], infoPng.width, 0, 2, 2, 4, passw[4], passh[4], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[5]], infoPng.width, 1, 0, 2, 2, passw[5], passh[5], bpp);
    adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[6]], infoPng.width, 0, 1, 1, 2, passw[6], passh[6], bpp);
  }
}

bool Decoder::hasError() const { return error != 0; }
int Decoder::getError() const { return error; }

unsigned long Decoder::getWidth()  const { return infoPng.width; }
unsigned long Decoder::getHeight() const { return infoPng.height; }

const InfoPng& Decoder::getInfoPng() const  { return infoPng; }

Decoder::Decoder()
{
  error = 1; //start out with error 1, which means: no image decoded yet
}

void Decoder::setSettings(const Decoder::Settings& settings) { this->settings = settings; }
const Decoder::Settings& Decoder::getSettings() const { return settings; }
Decoder::Settings& Decoder::getSettings() { return settings; }

void Decoder::setInfoRaw(const InfoRaw& infoRaw) { this->infoRaw = infoRaw; }
const InfoRaw& Decoder::getInfoRaw() const { return infoRaw; }
InfoRaw& Decoder::getInfoRaw() { return infoRaw; }

Decoder::Settings::Settings()
{
  color_convert = true;
  readTextChunks = true;
  ignoreCrc = false;
  ignoreAdler32 = false;
}

////////////////////////////////////////////////////////////////////////////////
// ** Functions and data for PNG encoding **                                  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PNG Encoder                                                                //
////////////////////////////////////////////////////////////////////////////////

void Encoder::addChunk(std::vector<unsigned char>& out, const std::string& chunkName, unsigned char* data, unsigned long length)
{
  if(chunkName.size() != 4) return;
  
  //1: length
  add32bitInt(out, length);
  
  //2: chunk name (4 letters)
  out.push_back(chunkName[0]);
  out.push_back(chunkName[1]);
  out.push_back(chunkName[2]);
  out.push_back(chunkName[3]);
  
  //3: the data
  for(size_t i = 0; i < length; i++) out.push_back(data[i]);
  
  //4: CRC (of the chunkname characters and the data)
  unsigned long CRC = crc32.crc(&out[out.size() - length - 4], length + 4);

  add32bitInt(out, CRC);
}

void Encoder::writeSignature(std::vector<unsigned char>& out)
{
  //8 bytes PNG signature
  out.push_back(137);
  out.push_back(80);
  out.push_back(78);
  out.push_back(71);
  out.push_back(13);
  out.push_back(10);
  out.push_back(26);
  out.push_back(10);
}

void Encoder::writeChunk_IHDR(std::vector<unsigned char>& out, unsigned long w, unsigned long h, unsigned long bitDepth, unsigned long colorType)
{
  std::vector<unsigned char> header;
  
  add32bitInt(header, w); //width
  add32bitInt(header, h); //height
  header.push_back((unsigned char)bitDepth); //bit depth
  header.push_back((unsigned char)colorType); //color type
  header.push_back(0); //compression method
  header.push_back(0); //filter method
  header.push_back(0); //interlace method
  
  addChunk(out, "IHDR", &header[0], (unsigned long)header.size());
}

void Encoder::writeChunk_tEXt(std::vector<unsigned char>& out, const std::string& keyword, const std::string& textstring)
{
  //add text chunk
  std::string text = keyword;
  text.push_back(0);
  text = text + textstring;
  addChunk(out, "tEXt", (unsigned char*)text.c_str(), (unsigned long)text.size());
}

void Encoder::writeChunk_PLTE(std::vector<unsigned char>& out, const Info& info)
{
  if(info.palette.empty() || info.palette.size() > (256 * 4)) { error = 68; return; }
  std::vector<unsigned char> PLTE;
  for(size_t i = 0; i < info.palette.size(); i++) if(i % 4 != 3) PLTE.push_back(info.palette[i]); //add all channels except alpha channel
  addChunk(out, "PLTE", PLTE.empty() ? 0 : &PLTE[0], (unsigned long)PLTE.size());
}

void Encoder::writeChunk_tRNS(std::vector<unsigned char>& out, const Info& info)
{
  std::vector<unsigned char> tRNS;
  if(info.colorType == 3)
  {
    for(size_t i = 3; i < info.palette.size(); i += 4) tRNS.push_back(info.palette[i]); //add only alpha channel
  }
  else if(info.colorType == 0)
  {
    if(info.key_defined)
    {
      tRNS.push_back((unsigned char)(info.key_r / 256));
      tRNS.push_back((unsigned char)(info.key_r % 256));
    }
  }
  else if(info.colorType == 2)
  {
    if(info.key_defined)
    {
      tRNS.push_back((unsigned char)(info.key_r / 256));
      tRNS.push_back((unsigned char)(info.key_r % 256));
      tRNS.push_back((unsigned char)(info.key_g / 256));
      tRNS.push_back((unsigned char)(info.key_g % 256));
      tRNS.push_back((unsigned char)(info.key_b / 256));
      tRNS.push_back((unsigned char)(info.key_b % 256));
    }
  }
  
  addChunk(out, "tRNS", tRNS.empty() ? 0 : &tRNS[0], (unsigned long)tRNS.size());
}

void Encoder::writeChunk_IDAT(std::vector<unsigned char>& out, const std::vector<unsigned char>& data)
{
  std::vector<unsigned char> zlibdata;
  
  //convert from LodePNG settings to the settings of the Zlib compressor
  LodeZlib::CompressSettings compressSettings;
  compressSettings.btype = settings.btype;
  compressSettings.useLZ77 = settings.useLZ77;
  compressSettings.windowSize = settings.windowSize;
  
  //compress with the Zlib compressor
  LodeZlib::compress(zlibdata, data, compressSettings);
  
  addChunk(out, "IDAT", zlibdata.empty() ? 0 : &zlibdata[0], (unsigned long)zlibdata.size());
}

void Encoder::writeChunk_IEND(std::vector<unsigned char>& out)
{
  addChunk(out, "IEND", 0, 0); 
}

void Encoder::writeChunk_bKGD(std::vector<unsigned char>& out, const InfoPng& info)
{
  std::vector<unsigned char> bKGD;
  if(info.colorType == 0 || info.colorType == 4)
  {
    bKGD.push_back((unsigned char)(info.background_r / 256));
    bKGD.push_back((unsigned char)(info.background_r % 256));
  }
  else if(info.colorType == 2 || info.colorType == 6)
  {
    bKGD.push_back((unsigned char)(info.background_r / 256));
    bKGD.push_back((unsigned char)(info.background_r % 256));
    bKGD.push_back((unsigned char)(info.background_g / 256));
    bKGD.push_back((unsigned char)(info.background_g % 256));
    bKGD.push_back((unsigned char)(info.background_b / 256));
    bKGD.push_back((unsigned char)(info.background_b % 256));
  }
  else if(info.colorType == 3)
  {
    bKGD.push_back((unsigned char)(info.background_r % 256)); //palette index
  }
  
  addChunk(out, "bKGD", bKGD.empty() ? 0 : &bKGD[0], (unsigned long)bKGD.size());
}

void Encoder::dontFilter(std::vector<unsigned char>& out, const unsigned char* image, unsigned long size, unsigned long w, unsigned long h)
{
  //the width of a scanline in bytes, not including the filter type
  size_t scanwidth = (w * infoPng.getBpp() + 7) / 8;
  
  out.resize(size + h);
  //generate the literal data out of given image vector. filterType has to be added per scanline.
  for(size_t y = 0; y < h; y++)
  {
    size_t begin = y * (scanwidth + 1);
    out[begin] = 0; //filterType 0 for this scanline
    for(size_t x = 0; x < scanwidth; x++) out[begin + 1 + x] = image[y * scanwidth + x];
  }
}

void Encoder::filterScanline(unsigned char* out, const unsigned char* scanline, const unsigned char* prevline, size_t length, size_t bytewidth, unsigned char filterType)
{
  switch(filterType)
  {
    case 0:
      if(prevline) for(size_t i = 0; i < length; i++) out[i] = scanline[i];
      else         for(size_t i = 0; i < length; i++) out[i] = scanline[i];
      break;
    case 1:
      if(prevline)
      {
        for(size_t i =         0; i < bytewidth; i++) out[i] = scanline[i];
        for(size_t i = bytewidth; i < length   ; i++) out[i] = scanline[i] - scanline[i - bytewidth];
      }
      else
      {
        for(size_t i =         0; i < bytewidth; i++) out[i] = scanline[i];
        for(size_t i = bytewidth; i <    length; i++) out[i] = scanline[i] - scanline[i - bytewidth];
      }
      break;
    case 2: 
      if(prevline) for(size_t i = 0; i < length; i++) out[i] = scanline[i] - prevline[i];
      else         for(size_t i = 0; i < length; i++) out[i] = scanline[i];
      break;
    case 3:
      if(prevline)
      {
        for(size_t i =         0; i < bytewidth; i++) out[i] = scanline[i] - prevline[i] / 2;
        for(size_t i = bytewidth; i <    length; i++) out[i] = scanline[i] - ((scanline[i - bytewidth] + prevline[i]) / 2);
      }
      else
      {
        for(size_t i =         0; i < length; i++) out[i] = scanline[i];
        for(size_t i = bytewidth; i < length; i++) out[i] = scanline[i] - scanline[i - bytewidth] / 2;
      }
      break;
    case 4:
      if(prevline)
      {
        for(size_t i =         0; i < bytewidth; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(0, prevline[i], 0));
        for(size_t i = bytewidth; i <    length; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(scanline[i - bytewidth], prevline[i], prevline[i - bytewidth]));
      }
      else
      {
        for(size_t i =         0; i < bytewidth; i++) out[i] = scanline[i];
        for(size_t i = bytewidth; i <    length; i++) out[i] = (unsigned char)(scanline[i] - paethPredictor(scanline[i - bytewidth], 0, 0));
      }
      break;
  default: return; //unexisting filter type given
  }
}

void Encoder::filter(std::vector<unsigned char>& out, const unsigned char* image, unsigned long size, unsigned long w, unsigned long h)
{
  //For filtering it uses the heuristic described here: http://www.cs.toronto.edu/~cosmin/pngtech/optipng.html
  // *  If the image type is Palette, or the bit depth is smaller than 8, then do not filter the image (i.e. use fixed filtering, with the filter None).
  // * (The other case) If the image type is Grayscale or RGB (with or without Alpha), and the bit depth is not smaller than 8, then use adaptive filtering as follows: independently for each row, apply all five filters and select the filter that produces the smallest sum of absolute values per row.
  //Here, the image is RGB(A) and bit depth 8, so the one with smallest sum is used.
  
  out.resize(size + h); //image size plus an extra byte per scanline
  
  //the width of a scanline in bytes, not including the filter type
  unsigned long bpp = infoPng.getBpp();
  size_t scanwidth = (w * bpp + 7) / 8;
  size_t bytewidth = (bpp + 7) / 8; //bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise
  
  std::vector<unsigned char> attempt[5]; //five filtering attempts, one for each filter type
  for(size_t i = 0; i < 5; i++) attempt[i].resize(scanwidth);
  unsigned long sum[5];
  
  for(size_t y = 0; y < h; y++)
  {
    //try the 5 filter types
    for(unsigned char i = 0; i < 5; i++)
    {
      const unsigned char* prevline = (y == 0) ? 0 : &image[(y - 1) * scanwidth];
      filterScanline(&attempt[i][0], &image[y * scanwidth], prevline, scanwidth, bytewidth, i);
      
      //calculate the sum of the result
      sum[i] = 0;
      for(unsigned char j = 0; j < 5; j++) sum[i] += attempt[i][j];
    }
    
    //find smallest sum
    unsigned long smallest = sum[0];
    unsigned char smallestType = 0;
    for(unsigned char i = 1; i < 5; i++)
    if(sum[i] < smallest)
    {
      smallestType = i;
      smallest = sum[i];
    }
    
    //now fill the out values
    out[y * (scanwidth + 1)] = smallestType; //the first byte of a scanline will be the filter type
    for(size_t i = 0; i < scanwidth; i++)
    out[y * (scanwidth + 1) + 1 + i] = attempt[smallestType][i];
  }
}

bool isPaletteFullyOpaque(const unsigned char* palette, unsigned long palette_size) //palette given in format RGBARGBARGBARGBA...
{
  for(unsigned long i = 3; i < palette_size; i += 4)
  {
    if(palette[i] != 255) return false;
  }
  return true;
}

//this function checks if the input image given by the user has no transparent pixels
bool isFullyOpaque(const unsigned char* image, unsigned long w, unsigned long h, const Info& info)
{
  //TODO: When the user specified a color key for the input image, then this function must also check for pixels that are the same as the color key and treat those as transparent.

  unsigned long numpixels = w * h;
  if(info.colorType == 6)
  {
    if(info.bitDepth == 8)
    {
      for(unsigned long i = 0; i < numpixels; i++) if(image[i * 4 + 3] != 255) return false;
    }
    else
    {
      for(unsigned long i = 0; i < numpixels; i++) if(image[i * 8 + 6] != 255 || image[i * 8 + 7] != 255) return false;
    }
    return true; //no single pixel with alpha channel other than 255 found
  }
  else if(info.colorType == 4)
  {
    if(info.bitDepth == 8)
    {
      for(unsigned long i = 0; i < numpixels; i++) if(image[i * 2 + 1] != 255) return false;
    }
    else
    {
      for(unsigned long i = 0; i < numpixels; i++) if(image[i * 4 + 2] != 255 || image[i * 4 + 3] != 255) return false;
    }
    return true; //no single pixel with alpha channel other than 255 found
  }
  else if(info.colorType == 3)
  {
    //when there's a palette, we could check every pixel for translucency, but much quicker is to just check the palette
    return(isPaletteFullyOpaque(info.palette.empty() ? 0 : &info.palette[0], (unsigned long)info.palette.size()));
  }

  return false; //color type that isn't supported by this function yet, so assume there is transparency to be safe
}

void Encoder::encode(std::vector<unsigned char>& out, const unsigned char* image, unsigned long w, unsigned long h)
{
  resetParameters();
  
  if(settings.autoLeaveOutAlphaChannel && isFullyOpaque(image, w, h, infoRaw))
  {
    //go to a color type without alpha channel
    if(infoPng.colorType == 6) infoPng.colorType = 2;
    else if(infoPng.colorType == 4) infoPng.colorType = 0;
  }
  
  if(settings.windowSize > 32768) { error = 60; return; }
  if(settings.btype > 2) { error = 61; return; }
  if(infoPng.text_keys.size() != infoPng.text_strings.size()) { error = 65; return; }
  
  std::vector<unsigned char> data;
  
  if(infoRaw != infoPng)
  {
    if((infoPng.colorType != 6 && infoPng.colorType != 2) || (infoPng.bitDepth != 8)) { error = 59; return; } //for the output image, only these types are supported

    std::vector<unsigned char> converted;
    error = convert(converted, image, infoPng, infoRaw, w, h);
    if(hasError()) return;
    
    filter(data, converted.empty() ? 0 : &converted[0], (unsigned long)converted.size(), w, h);
  }
  else
  {
    unsigned long size = (w * h * infoPng.getBpp() + 7) / 8;
    filter(data, image, size, w, h);
  }
  
  //write signature and chunks
  writeSignature(out);
  //IHDR
  writeChunk_IHDR(out, w, h, infoPng.bitDepth, infoPng.colorType);
  //PLTE and tRNS
  if(infoPng.colorType == 3)
  {
    writeChunk_PLTE(out, infoPng);
    if(!isPaletteFullyOpaque(infoPng.palette.empty() ? 0 : &infoPng.palette[0], (unsigned long)infoPng.palette.size())) writeChunk_tRNS(out, infoPng);
  }
  if(settings.force_palette && (infoPng.colorType == 2 || infoPng.colorType == 6)) writeChunk_PLTE(out, infoPng);
  if(infoPng.key_defined && (infoPng.colorType == 0 || infoPng.colorType == 2)) writeChunk_tRNS(out, infoPng);
  //bKGD
  if(infoPng.background_defined) writeChunk_bKGD(out, infoPng);
  //IDAT
  writeChunk_IDAT(out, data);
  //tEXt
  for(size_t i = 0; i < infoPng.text_keys.size(); i++)
  {
    if(infoPng.text_keys[i].size() > 79) { error = 66; return; }
    if(infoPng.text_keys[i].size() < 1) { error = 67; return; }
    writeChunk_tEXt(out, infoPng.text_keys[i], infoPng.text_strings[i]);
  }
  //id
  if(settings.add_id)
  {
    writeChunk_tEXt(out, "Encoder", std::string("LodePNG ") + VERSION_STRING);
  }
  //IEND
  writeChunk_IEND(out);
}

void Encoder::encode(std::vector<unsigned char>& out, const std::vector<unsigned char>& image, unsigned long w, unsigned long h)
{
  encode(out, image.empty() ? 0 : &image[0], w, h);
}

void Encoder::setSettings(const Encoder::Settings& settings) { this->settings = settings; }
const Encoder::Settings& Encoder::getSettings() const { return settings; }
Encoder::Settings& Encoder::getSettings() { return settings; }

void Encoder::setInfoPng(const InfoPng& infoPng) { this->user_infoPng = infoPng; }
const InfoPng& Encoder::getInfoPng() const { return user_infoPng; }
InfoPng& Encoder::getInfoPng()  { return user_infoPng; }

void Encoder::setInfoRaw(const InfoRaw& infoRaw) { this->infoRaw = infoRaw; }
const InfoRaw& Encoder::getInfoRaw() const { return infoRaw; }
InfoRaw& Encoder::getInfoRaw() { return infoRaw; }

void Encoder::resetParameters()
{
  infoPng = user_infoPng;
  error = 0;
}

bool Encoder::hasError() const { return error != 0; }
int Encoder::getError() const { return error; }

Encoder::Encoder()
{
  user_infoPng.colorType = 6;
  user_infoPng.bitDepth = 8;
  error = 1;
}

Encoder::Settings::Settings()
{
  autoLeaveOutAlphaChannel = true;
  windowSize = 2048; //this is a good tradeoff between speed and compression ratio
  btype = 2;
  useLZ77 = true;
  force_palette = false;
  add_id = true;
}

////////////////////////////////////////////////////////////////////////////////
// 7. Extras                                                                  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// File IO                                                                    //
////////////////////////////////////////////////////////////////////////////////

void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector
{
  std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

  //get filesize
  std::streamsize size = 0;
  if(file.seekg(0, std::ios::end).good()) size = file.tellg();
  if(file.seekg(0, std::ios::beg).good()) size -= file.tellg();

  //read contents of the file into the vector
  buffer.resize(size_t(size));
  if(size > 0) file.read((char*)(&buffer[0]), size);
}

//write given buffer to the file, overwriting the file, it doesn't append to it.
void saveFile(const std::vector<unsigned char>& buffer, const std::string& filename)
{
  std::ofstream file(filename.c_str(), std::ios::out|std::ios::binary);
  file.write(buffer.empty() ? 0 : (char*)&buffer[0], std::streamsize(buffer.size()));
}

////////////////////////////////////////////////////////////////////////////////
// Global Functions Using LodePNG                                             //
////////////////////////////////////////////////////////////////////////////////

int decode(std::vector<unsigned char>& out, unsigned long& w, unsigned long& h, const unsigned char* in, unsigned long size, unsigned long colorType, unsigned long bitDepth)
{
  Decoder decoder;
  decoder.getInfoRaw().colorType = colorType;
  decoder.getInfoRaw().bitDepth = bitDepth;
  decoder.decode(out, in, size);
  w = decoder.getWidth();
  h = decoder.getHeight();
  return decoder.getError();
}

int decode(std::vector<unsigned char>& out, unsigned long& w, unsigned long& h, const std::vector<unsigned char>& in, unsigned long colorType, unsigned long bitDepth)
{
  return decode(out, w, h, in.empty() ? 0 : &in[0], (unsigned long)in.size(), colorType, bitDepth);
}

int decode(std::vector<unsigned char>& out, unsigned long& w, unsigned long& h, const std::string& filename, unsigned long colorType, unsigned long bitDepth)
{
  std::vector<unsigned char> buffer;
  loadFile(buffer, filename);
  return decode(out, w, h, buffer, colorType, bitDepth);
}

int encode(std::vector<unsigned char>& out, const unsigned char* in, unsigned long w, unsigned long h, unsigned long colorType, unsigned long bitDepth)
{
  Encoder encoder;
  encoder.getInfoRaw().colorType = colorType;
  encoder.getInfoRaw().bitDepth = bitDepth;
  encoder.encode(out, in, w, h);
  return encoder.getError();
}

int encode(std::vector<unsigned char>& out, const std::vector<unsigned char>& in, unsigned long w, unsigned long h, unsigned long colorType, unsigned long bitDepth)
{
  return encode(out, in.empty() ? 0 : &in[0], w, h, colorType, bitDepth);
}

int encode(const std::string& filename, const unsigned char* in, unsigned long w, unsigned long h, unsigned long colorType, unsigned long bitDepth)
{
  std::vector<unsigned char> buffer;
  Encoder encoder;
  encoder.getInfoRaw().colorType = colorType;
  encoder.getInfoRaw().bitDepth = bitDepth;
  encoder.encode(buffer, in, w, h);
  if(!encoder.hasError()) saveFile(buffer, filename);
  return encoder.getError();
}

int encode(const std::string& filename, const std::vector<unsigned char>& in, unsigned long w, unsigned long h, unsigned long colorType, unsigned long bitDepth)
{
  return encode(filename, in.empty() ? 0 : &in[0], w, h, colorType, bitDepth);
}

} //end of namespace LodePNG
