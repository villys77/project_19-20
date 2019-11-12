//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "structs.h"


relation Sort(relation);
void sorting (relation *,relation *,int, int , int , int);
void Join(relation , relation );
relation read_file(char*);


Result* ListInit();
void InsertResult(uint64_t,uint64_t,Result*);
void PrintResults(Result*);
void freelist(Result*);


void swap(uint64_t * a, uint64_t * b);
int partition (relation arr, int low, int high);
void quickSort(relation arr, int low, int high);

#endif //PROJECT_FUNCTIONS_H