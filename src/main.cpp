#include <cstdio>
#include "HyperLogLog.h"
#include "RandomSet.h"


#define SAMPLESIZE 30
#define UNIQUES 10
#define BITS 16
#define SF 1
int main(int args,char **argc){

    HyperLogLog *log1 =  HyperLogLog::init(0);
    HyperLogLog::setP(log1,BITS);

    RandomSet *set1 = new RandomSet(0,SAMPLESIZE,UNIQUES);
    RandomSet *set2 = new RandomSet(0,SAMPLESIZE,UNIQUES);

    for(auto i = 0; i < SAMPLESIZE+UNIQUES; i++)
    {
           printf("%ld ",set2->getElement(i));
       HyperLogLog::addDatum(log1,set1->getElement(i));
     }
  printf("\n");
     int c = log1->estimateCardinality();

     printf("Sample %d Unique %d Estimated Unique %d\n",SAMPLESIZE+UNIQUES,UNIQUES,c);

  
    HyperLogLog *log2 =  HyperLogLog::init(0);
    HyperLogLog::setP(log2,BITS);
    for(auto i = 0; i < SAMPLESIZE+UNIQUES; i++)
    {
        printf("%ld ",set2->getElement(i));
       HyperLogLog::addDatum(log2,set2->getElement(i));
     }
     printf("\n");

    c = log2->estimateCardinality();

     printf("Sample %d Unique %d Estimated Unique %d\n",SAMPLESIZE+UNIQUES,UNIQUES,c);

     HyperLogLog *log3 = log1->SetUnion(log2);

    c = log3->estimateCardinality();

    printf("Union %d Unique %d Estimated Unique %d\n",SAMPLESIZE+UNIQUES,UNIQUES,c);


     delete set1;
     delete set2;
     delete log1;
     delete log2;

}