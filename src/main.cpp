#include <cstdio>
#include "HyperLogLog.h"
#include "RandomSet.h"


#define SAMPLESIZE 3000
#define UNIQUES 2000
#define BITS 16
#define SF 1
int main(int args,char **argc){

    HyperLogLog *log =  HyperLogLog::init(0);
    HyperLogLog::setP(log,BITS);

    RandomSet *set1 = new RandomSet((unsigned long)0,SAMPLESIZE,UNIQUES);
    RandomSet *set2 = new RandomSet((unsigned long)0,SAMPLESIZE,UNIQUES);

    for(auto i = 0; i < SAMPLESIZE+UNIQUES; i++)
    {
       HyperLogLog::addDatum(log,set1->getElement(i));
     }

     int c = log->estimateCardinality();

     printf("Sample %d Unique %d Estimated Unique %d\n",SAMPLESIZE+UNIQUES,UNIQUES,c);

}