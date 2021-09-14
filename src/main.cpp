#include <cstdio>
#include "HyperLogLog.h"
#include "RandomSet.h"
#include "time.h"
#include <stdlib.h>

// how many numbers to place in the bitmap and how many will be different
#define SAMPLESIZE 3000
#define UNIQUES 700
// the dataset we create will be keyed into buckets on this
// number of bits, so will be 2^BITS long
// a smaller number of bits will be less accurate
// but more memory efficient
#define BITS 7
// play with the overlap of the datasets
#define SET1STARTNUMBER 0
#define SET2STARTNUMBER 20

int main(int args, char **argc)
{
    HyperLogLog *log1 = HyperLogLog::init(BITS);

   // these are only randomised in order and in which elements are duplicated
   // we can create small sets and print them to see if this works
   RandomSet *set1 = new RandomSet(SET1STARTNUMBER, SAMPLESIZE, UNIQUES);
   RandomSet *set2 = new RandomSet(SET2STARTNUMBER, SAMPLESIZE, UNIQUES);

   for (auto i = 0; i < SAMPLESIZE + UNIQUES; i++)
   {
      // printf("%ld ",set2->getElement(i));
      HyperLogLog::addDatum(log1, set1->getElement(i));
   }
   //printf("\n");
   int c = log1->estimateCardinality();

   printf("Sample %d Unique %d Estimated Unique %d\n", SAMPLESIZE + UNIQUES, UNIQUES, c);

   HyperLogLog *log2 = HyperLogLog::init(BITS);
   for (auto i = 0; i < SAMPLESIZE + UNIQUES; i++)
   {
      //printf("%ld ",set2->getElement(i));
      HyperLogLog::addDatum(log2, set2->getElement(i));
   }
   //printf("\n");

   c = log2->estimateCardinality();

   printf("Sample %d Unique %d Estimated Unique %d\n", SAMPLESIZE + UNIQUES, UNIQUES, c);

   HyperLogLog *log3 = log1->setUnion(log2);

   c = log3->estimateCardinality();

   printf("Union Estimated Unique %d\n", c);

   delete set1;
   delete set2;
   delete log1;
   delete log2;
}