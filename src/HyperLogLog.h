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
    int32_t allocated;
    int32_t count;
    IntHash hash;
    int p;
    int m;
    double alpha;
    //
    // this is a bit of a naughty trick
    // that allows us to allocate the class with a single
    // allocation and copy the class with a single memcpy
    // in other languages with sensible bounds checking
    // you'd need to be a bit more sensible.
    //
    maxzero_t maxZeros[0];
    static void IntHash_IntHash(IntHash &me);
    static int IntHash_mostSignificantBits(IntHash &me, uint32_t key, int nBits);
    static maxzero_t IntHash_countLeadingZeros(uint32_t toCount);
    static maxzero_t IntHash_leadingZeros(IntHash &me, uint32_t key);
    static void setsize(HyperLogLog *&old_ptr, int needed);
    
public:
    // interface
    static HyperLogLog *init(int32_t count);
    static void addDatum(HyperLogLog *&bitmap_ptr, const long datum);
    HyperLogLog *copy();
    void setAlpha(int &newP);
    void Union(HyperLogLog *&to);
    static void setP(HyperLogLog *&bitmap_ptr, int newP);

    unsigned long magnitudeIntersection(HyperLogLog *&datum);
    void add(HyperLogLog *&to_ptr);
    HyperLogLog *SetUnion(HyperLogLog *&datum);
    long double estimateCardinality();
      
   
  } __attribute__((__packed__));
  #endif