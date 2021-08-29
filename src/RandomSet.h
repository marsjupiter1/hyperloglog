#ifndef RANDOMSET_H
#define RANDOMSET_H
#include <stdlib.h>
class RandomSet
{
private:
    unsigned long startNumber;
    unsigned long numElements;
    unsigned long numUniqueElements;
    unsigned long *elementSet;

public:
    unsigned long getElement(unsigned long i)
    {
        return elementSet[i];
    }

    static unsigned long getRandom(unsigned long low, unsigned long high)
    {
        return rand() % (high + 1 - low) + low;
    }

    RandomSet(unsigned long StartNumber, unsigned long NumElements, unsigned long NumUniqueElements)
    {
        startNumber = StartNumber;
        numElements = NumElements;
        numUniqueElements = NumUniqueElements;
        elementSet = (unsigned long *)malloc((numElements * 2 - numUniqueElements) * sizeof(unsigned long));

        // Create the set of unique elements to use
        for (auto i = startNumber; i < numUniqueElements + startNumber; i++)
        {
            elementSet[i - startNumber] = i;
        }

        // Create duplicates
        for (auto i = 0; i < numElements - numUniqueElements; i++)
        {
            elementSet[i + numUniqueElements] = elementSet[getRandom(0, numUniqueElements - 1)];
        }
        // shuffle
        for (auto i = 0; i < numElements + numUniqueElements; i++)
        {
            unsigned long swappos = getRandom(0, numElements + numUniqueElements - 1);
            unsigned long swap = elementSet[i];
            elementSet[i] = elementSet[swappos];
            elementSet[swappos] = swap;
        }
    }

    ~RandomSet()
    {
        free(elementSet);
    }
};
#endif