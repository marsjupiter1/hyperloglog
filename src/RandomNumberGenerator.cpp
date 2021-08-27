#include "RandomNumberGenerator.h"


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

