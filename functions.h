//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "structs.h"


column_data Sort(column_data);
void sorting (column_data *,column_data *,int, int , int , int);
void Join(column_data , column_data );
relation * read_file(char*);
uint64_t * loadRelation(char* fileName);

Result* ListInit();
void InsertResult(uint64_t,uint64_t,Result*);
void PrintResults(Result*);
void freelist(Result*);


void swap(uint64_t * a, uint64_t * b);
int partition (column_data arr, int low, int high);
void quickSort(column_data arr, int low, int high);

#endif //PROJECT_FUNCTIONS_H