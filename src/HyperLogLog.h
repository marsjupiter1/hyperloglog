#ifndef HYPERLOGLOG_H
#define HYPERLOGLOG_H
#include <stdint.h>

typedef unsigned char maxzero_t;
typedef struct __attribute__((__packed__))
{
    uint32_t seed;

} IntHash;


class HyperLogLog
{
private:

    int KeyBitCount;
    int KeyArraySize;
    //
    // this is a bit of a naughty trick
    // that allows us to allocate the class with a single
    // allocation and copy the class with a single memcpy
    // in other languages with sensible bounds checking
    // you'd need to be a bit more sensible.
    //
    maxzero_t maxZeros[0];
    static void IntHash_IntHash(IntHash &me);
    static int IntHash_mostSignificantBits(  uint32_t *datum_hash, int nBits);
     static maxzero_t IntHash_leadingZeros(  uint32_t *datum_hash);
    
public:
    // interface
    static HyperLogLog *init(int32_t keybitcount);
    static void addDatum(HyperLogLog *&bitmap_ptr, const long datum);
    HyperLogLog *copy();
    void Union(HyperLogLog *&to);
  
    unsigned long magnitudeIntersection(HyperLogLog *&datum);
    void add(HyperLogLog *&to_ptr);
    HyperLogLog *setUnion(HyperLogLog *&datum);
    long double estimateCardinality();
      
   
  } __attribute__((__packed__));
  #endif