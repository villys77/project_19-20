//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "structs.h"


column_data Sort(column_data);
void sorting (column_data *,column_data *,int, int , int , int);
void Join(column_data , column_data );
relation * read_file(char*,int *);
uint64_t * loadRelation(char* );
void queries_analysis(char * );
int * select_pred(int , struct preds*);



Result* ListInit();
void InsertResult(uint64_t,uint64_t,Result*);
void PrintResults(Result*);
void freelist(Result*);


void swap(uint64_t * a, uint64_t * b);
int partition (column_data, int , int);
void quickSort(column_data, int , int);

#endif //PROJECT_FUNCTIONS_H