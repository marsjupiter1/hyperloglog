#include "RandomNumberGenerator.h"

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
  

    RandomSet(unsigned long StartNumber,unsigned long NumElements,unsigned long NumUniqueElements)
    {
        startNumber = StartNumber;
        numUniqueElements = NumUniqueElements;
        elementSet = (unsigned long *) malloc((numElements+numUniqueElements)*sizeof(unsigned long));
        
    // Create the set of unique elements to use
   		 for(auto i = startNumber; i < numUniqueElements + startNumber; i++)
         {
         		elementSet[i - startNumber]=i;
         }

   	 // Create duplicates
    	for(auto i = 0; i < numElements - numUniqueElements; i++)
    	{
        	elementSet[i+numElements] = elementSet[RandomNumberGenerator::getInstance().getRandomInt(0,numUniqueElements-1)];
		}
    // shuffle
        for(auto i = 0; i < numElements + numUniqueElements; i++)
    	{
            unsigned long swappos = RandomNumberGenerator::getInstance().getRandomInt(0,numElements+numUniqueElements-1);
            unsigned long swap = elementSet[i];
            elementSet[i] = elementSet[swappos];
            elementSet[swappos] = swap;
        }
    }

    ~RandomSet(){
         free(elementSet);
     }

   
};