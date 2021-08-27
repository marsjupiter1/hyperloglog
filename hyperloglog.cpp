
#include <cmath>
#include <algorithm> // std::max
#include <ctime>
#include <cstring>
#include <cassert>
#include "murmur3.h"

class RandomNumberGenerator
{
public:
    static RandomNumberGenerator &getInstance()
    {
        static RandomNumberGenerator instance;
        return instance;
    }

    // Generates a random float between 0 and 1 (inclusive)
    float getRandomFloat();

    // Generates a random double between 0 and 1 (inclusive)
    double getRandomDouble();

    // Generates random numbers between 0 and max value (inclusive)
    unsigned long getRandomInt(const unsigned long &maxValue);

    // Generates uniform random numbers between the values (inclusive)
    unsigned long getRandomInt(const unsigned long &minValue, const unsigned long &maxValue);

private:
    RandomNumberGenerator();
    RandomNumberGenerator(RandomNumberGenerator const &);
    void operator=(RandomNumberGenerator const &);

    u_int64_t xorshift1024();
    u_int64_t xorshift64(uint64_t &seed);

    unsigned int seedPosition;
    u_int64_t seeds[16];
};

using namespace std;

RandomNumberGenerator::RandomNumberGenerator() : seedPosition(0)
{
    uint64_t seed = time(NULL);
    for (int i = 0; i < 16; i++)
        seeds[i] = xorshift64(seed);
}

uint64_t RandomNumberGenerator::xorshift64(uint64_t &seed)
{
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 2685821657736338717LL;
}

uint64_t RandomNumberGenerator::xorshift1024()
{
    uint64_t s0 = seeds[seedPosition];
    uint64_t s1 = seeds[seedPosition = (seedPosition + 1) & 15];
    s1 ^= s1 << 31;
    s1 ^= s1 >> 11;
    s0 ^= s0 >> 30;
    return (seeds[seedPosition] = s0 ^ s1) * 1181783497276652981LL;
}

unsigned long RandomNumberGenerator::getRandomInt(const unsigned long &maxValue)
{
    static const uint64_t max64Int = ~0;
    if (maxValue == max64Int)
        return xorshift1024();

    uint64_t inclusiveMax = maxValue + 1;

    uint64_t retVal;
    uint64_t maxAcceptableValue = (max64Int / inclusiveMax) * inclusiveMax;
    do
    {
        retVal = xorshift1024();
    } while (retVal > maxAcceptableValue);
    retVal %= inclusiveMax;
    return retVal;
}

unsigned long RandomNumberGenerator::getRandomInt(const unsigned long &minValue, const unsigned long &maxValue)
{
    assert(minValue <= maxValue);
    unsigned long difference = maxValue - minValue;
    return getRandomInt(difference) + minValue;
}

// Generates a random float between 0 and 1 (inclusive)
float RandomNumberGenerator::getRandomFloat()
{
    static const uint64_t max64Int = ~0;
    uint64_t rand = xorshift1024();
    float retVal = (float)rand / max64Int;
    return retVal;
}

// Generates a random double between 0 and 1 (inclusive)
double RandomNumberGenerator::getRandomDouble()
{
    static const uint64_t max64Int = ~0;
    uint64_t rand = xorshift1024();
    double retVal = (double)rand / max64Int;
    return retVal;
}

typedef struct __attribute__((__packed__))
{
    uint32_t seed;

} IntHash;

typedef unsigned char maxzero_t;

class HyperLogLog
{
public:
    int32_t allocated;
    int32_t count;
    IntHash hash;
    int p;
    int m;
    double alpha;
    maxzero_t maxZeros[0];
    static void IntHash_IntHash(IntHash &me);
    static int IntHash_mostSignificantBits(IntHash &me, uint32_t key, int nBits);
    static maxzero_t IntHash_countLeadingZeros(uint32_t toCount);
    static maxzero_t IntHash_leadingZeros(IntHash &me, uint32_t key);
    static void setsize(HyperLogLog *&old_ptr, int needed);
    static void setP(HyperLogLog *&bitmap_ptr, int newP);
    static void addDatum(HyperLogLog *&bitmap_ptr, const long datum);
    static HyperLogLog *init(int count);
    long double estimateCardinality();
  
    // interface
    HyperLogLog *copy();
    void setAlpha(int &newP);
    void Union(HyperLogLog *&to);
    unsigned long magnitudeIntersection(HyperLogLog *&datum);
    void add(HyperLogLog *&to_ptr);
    HyperLogLog *SetUnion(HyperLogLog *&datum);
    
   
  } __attribute__((__packed__));

static const double alphas[18] =
    {0, 0.351193943305104, 0.532434616688025, 0.625608716971165,
     0.673102032011193, 0.697122649688705, 0.709208485323602,
     0.715271255627600, 0.718307770416137, 0.719827413209098,
     0.720587757723026, 0.720968410691135, 0.721159556732532,
     0.721256733328830, 0.721308519914072, 0.721340807633915,
     0.721369740077220

};

void HyperLogLog::IntHash_IntHash(IntHash &me)
{

#ifndef MAKETEST
    uint32_t max = ~0;
    me.seed = RandomNumberGenerator::getInstance().getRandomInt(max);
#else
    me.seed = 0;
#endif
}

int HyperLogLog::IntHash_mostSignificantBits(IntHash &me, uint32_t key, int nBits)
{

    assert(nBits <= 32);
    uint32_t hash[4];
    MurmurHash3_x64_128(&key, sizeof(key), me.seed, hash);
    return (int)(hash[3] >> (32 - nBits));
}

maxzero_t HyperLogLog::IntHash_countLeadingZeros(uint32_t toCount)
{
    int count = 0;
    for (int i = 0; i < 8; i++)
    {
        if (((toCount >> i) & 1) == 0)
            return count;
        count++;
    }
    return count;
}

maxzero_t HyperLogLog::IntHash_leadingZeros(IntHash &me, uint32_t key)
{
    uint32_t hash[4];
    MurmurHash3_x64_128(&key, sizeof(key), me.seed, hash);
    int count = HyperLogLog::IntHash_countLeadingZeros( hash[0]);
    if (count != 8)
        return count;
    count = HyperLogLog::IntHash_countLeadingZeros(hash[1]);
    if (count != 8)
        return count + 8;
    count = HyperLogLog::IntHash_countLeadingZeros( hash[2]);
    if (count != 8)
        return count + 16;
    count = HyperLogLog::IntHash_countLeadingZeros( hash[3]);
    return count + 24;
}

void HyperLogLog::setsize(HyperLogLog *&old_ptr, int needed)
{

    if (needed > old_ptr->allocated)
    {
        HyperLogLog *new_ptr = HyperLogLog::init( needed);

        memcpy(new_ptr, old_ptr, sizeof(HyperLogLog) + old_ptr->count * sizeof(maxzero_t));
        new_ptr->allocated = needed;
        free(old_ptr);
        old_ptr = new_ptr;
    }
}

void HyperLogLog::setAlpha(int &newP)
{
    if (newP > 16)
    {
        alpha = 0.72136974007722; // It doesn't get much more accurate with higher p
        return;
    }

    alpha = alphas[newP];
}

void HyperLogLog::setP(HyperLogLog *&bitmap_ptr, int newP)
{
    bitmap_ptr->p = newP;
    bitmap_ptr->m = pow(2, newP);
 
    bitmap_ptr->setAlpha( newP);
 
    if (bitmap_ptr->m > bitmap_ptr->count)
    {
        HyperLogLog::setsize( bitmap_ptr, bitmap_ptr->m);
        for (int i = bitmap_ptr->count; i < bitmap_ptr->m; i++)
        {
            bitmap_ptr->maxZeros[i] = 0;
        }
        bitmap_ptr->count = bitmap_ptr->m;
    }
}

void HyperLogLog::addDatum(HyperLogLog *&bitmap_ptr, const long datum)
{
    uint32_t key = datum;
    int index = IntHash_mostSignificantBits(bitmap_ptr->hash, key, bitmap_ptr->p);

    assert(index >= 0);
    HyperLogLog::setsize(bitmap_ptr, index);
    bitmap_ptr->maxZeros[index] = max(bitmap_ptr->maxZeros[index], (maxzero_t)(HyperLogLog::IntHash_leadingZeros(bitmap_ptr->hash, key) + 1));
    if (index >= bitmap_ptr->count)
    {

        bitmap_ptr->count = index + 1;
    }
}

HyperLogLog *HyperLogLog::init(int count)
{
    int size = sizeof(HyperLogLog) + count * sizeof(maxzero_t);
    HyperLogLog *payload = (HyperLogLog *)malloc(size);

    assert(count >= 0);

    payload->count = 0;
    payload->allocated = count;

    IntHash_IntHash(payload->hash);

    return payload;
}

long double HyperLogLog::estimateCardinality()
{
    long double sum = 0;
    unsigned int totalZeros = 0;
    maxzero_t z;
    for (int i = 0; i < count; i++)
    {
        z = maxZeros[i];
        sum += pow(2, -((double)z));
        if (z == 0)
        {
            totalZeros++;
        }
    }

    if (totalZeros == 0)
    {
        return (long double)alpha * (long double)pow(m, 2) / sum;
    }
    else
    {
        return (long double)m * log((double)m / (double)totalZeros);
    }
}

void HyperLogLog::Union(HyperLogLog *&bitmap_ptr)
{

    assert(bitmap_ptr->count == this->count);

    this->add(bitmap_ptr);
}

void HyperLogLog::add(HyperLogLog *&to_ptr)
{
    for (auto i = 0; i < count; i++)
    {
        to_ptr->maxZeros[i] = max(to_ptr->maxZeros[i], maxZeros[i]);
    }
    to_ptr->count = count;
}

HyperLogLog *HyperLogLog::copy()
{
    HyperLogLog *to = HyperLogLog::init(allocated);
    memcpy(to, this, sizeof(HyperLogLog) + sizeof(maxzero_t) * allocated);
    return to;
}

HyperLogLog *HyperLogLog::SetUnion(HyperLogLog *&datum)
{
    assert(datum->count == count);
    HyperLogLog *retVal = HyperLogLog::init(count);
    memcpy(retVal, datum, sizeof(HyperLogLog) + datum->count * sizeof(maxzero_t));

    for (auto i = 0; i < count; i++)
    {
        retVal->maxZeros[i] = max(retVal->maxZeros[i], maxZeros[i]);
    }
    return retVal;
}

unsigned long HyperLogLog::magnitudeIntersection(HyperLogLog *&datum)
{
    long double A = estimateCardinality();

    long double B = datum->estimateCardinality();

    HyperLogLog *setunion = this->SetUnion( datum);
    unsigned long AuB = setunion->estimateCardinality();
    free(setunion);
    return A + B - AuB;
}
