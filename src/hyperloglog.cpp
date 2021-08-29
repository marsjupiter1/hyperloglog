
#include <cmath>
#include <algorithm> // std::max
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cassert>
#include "murmur3.h"

#include "HyperLogLog.h"

using namespace std;

#define HASHSEED 0

static const double alphas[18] =
    {0, 0.351193943305104, 0.532434616688025, 0.625608716971165,
     0.673102032011193, 0.697122649688705, 0.709208485323602,
     0.715271255627600, 0.718307770416137, 0.719827413209098,
     0.720587757723026, 0.720968410691135, 0.721159556732532,
     0.721256733328830, 0.721308519914072, 0.721340807633915,
     0.721369740077220

};

int HyperLogLog::IntHash_mostSignificantBits(uint32_t *datum_hash, int nBits)
{
    assert(nBits <= 32);
    return (int)(datum_hash[3] >> (32 - nBits));
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

maxzero_t HyperLogLog::IntHash_leadingZeros(uint32_t *datum_hash)
{
    int count = HyperLogLog::IntHash_countLeadingZeros(datum_hash[0]);
    if (count != 8)
        return count;
    count = HyperLogLog::IntHash_countLeadingZeros(datum_hash[1]);
    if (count != 8)
        return count + 8;
    count = HyperLogLog::IntHash_countLeadingZeros(datum_hash[2]);
    if (count != 8)
        return count + 16;
    count = HyperLogLog::IntHash_countLeadingZeros(datum_hash[3]);
    return count + 24;
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

void HyperLogLog::addDatum(HyperLogLog *&bitmap_ptr, const long datum)
{

    uint32_t hash[4];
    MurmurHash3_x64_128(&datum, sizeof(datum), HASHSEED, hash);

    int index = IntHash_mostSignificantBits(hash, bitmap_ptr->KeyBitCount);

    assert(index >= 0);
    bitmap_ptr->maxZeros[index] = max(bitmap_ptr->maxZeros[index], (maxzero_t)(HyperLogLog::IntHash_leadingZeros(hash) + 1));
}

HyperLogLog *HyperLogLog::init(int keybitcount)
{
    int keySize = pow(2, keybitcount);

    int size = sizeof(HyperLogLog) + keySize * sizeof(maxzero_t);
    HyperLogLog *bitmap_ptr = (HyperLogLog *)malloc(size);

    assert(keybitcount >= 1);
    bitmap_ptr->KeyBitCount = keybitcount;
    bitmap_ptr->KeyArraySize = keySize;
    bitmap_ptr->setAlpha(keybitcount);

    return bitmap_ptr;
}

long double HyperLogLog::estimateCardinality()
{
    long double sum = 0;
    unsigned int totalZeros = 0;
    maxzero_t z;
    for (int i = 0; i < KeyArraySize; i++)
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
        return (long double)alpha * (long double)pow(KeyArraySize, 2) / sum;
    }
    else
    {
        return (long double)KeyArraySize * log((double)KeyArraySize / (double)totalZeros);
    }
}

void HyperLogLog::Union(HyperLogLog *&bitmap_ptr)
{

    assert(bitmap_ptr->KeyBitCount == this->KeyBitCount);

    this->add(bitmap_ptr);
}

void HyperLogLog::add(HyperLogLog *&to_ptr)
{
    for (auto i = 0; i < KeyArraySize; i++)
    {
        to_ptr->maxZeros[i] = max(to_ptr->maxZeros[i], maxZeros[i]);
    }
}

HyperLogLog *HyperLogLog::copy()
{
    HyperLogLog *to = HyperLogLog::init(KeyBitCount);
    memcpy(to, this, sizeof(HyperLogLog) + sizeof(maxzero_t) * pow(2, KeyBitCount));
    return to;
}

HyperLogLog *HyperLogLog::setUnion(HyperLogLog *&datum)
{
    assert(datum->KeyBitCount == KeyBitCount);
    HyperLogLog *retVal = HyperLogLog::init(KeyBitCount);
    memcpy(retVal, datum, sizeof(HyperLogLog) + datum->KeyArraySize * sizeof(maxzero_t));

    for (auto i = 0; i < KeyArraySize; i++)
    {
        retVal->maxZeros[i] = max(retVal->maxZeros[i], maxZeros[i]);
    }
    return retVal;
}

unsigned long HyperLogLog::magnitudeIntersection(HyperLogLog *&datum)
{
    long double A = estimateCardinality();
    long double B = datum->estimateCardinality();

    HyperLogLog *setunion = this->setUnion(datum);
    unsigned long AuB = setunion->estimateCardinality();
    free(setunion);
    return A + B - AuB;
}
