//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "structs.h"


column_data Sort(column_data);
void sorting (column_data *,column_data *,int, int , int , int);
Result * Join(column_data , column_data );
relation * read_file(char*,int *);
uint64_t * loadRelation(char* );
void queries_analysis(char * ,relation*);
int * select_pred(int , char *,int*,struct preds *);
column_data load_column_data(relation *, int rel,int col);
void Equalizer(column_data array,int b_size,int given_num,int given_mode);



Result* ListInit();
void InsertResult(uint64_t,uint64_t,Result*);
void PrintResults(Result*);
void freelist(Result*);


void swap(uint64_t * a, uint64_t * b);
int partition (column_data, int , int);
void quickSort(column_data, int , int);

#endif //PROJECT_FUNCTIONS_H