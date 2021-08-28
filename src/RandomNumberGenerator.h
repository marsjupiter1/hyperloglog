#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

#include <stdint.h>
#include <cassert>
#include <cmath>
#include <algorithm> // std::max
#include <ctime>

class RandomNumberGenerator
{
public:
    static RandomNumberGenerator& getInstance()
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
    RandomNumberGenerator(RandomNumberGenerator const&);
    void operator=(RandomNumberGenerator const&);

    uint64_t xorshift1024();
    uint64_t xorshift64(uint64_t &seed);

    unsigned int seedPosition;
    uint64_t seeds[16];
};


#endif // RANDOMNUMBERGENERATOR_H
