//
// Created by villys77 on 17/1/20.
//

#ifndef PROJECT_JOIN_19_20_SORT_JOIN_FUNS_H
#define PROJECT_JOIN_19_20_SORT_JOIN_FUNS_H

#include "Result.h"
#include "structs.h"


#define n 8


relation * read_file(char*,int *,struct statistics**);
uint64_t * loadRelation(char* );

void swap(uint64_t * a, uint64_t * b);
int partition (column_data, int , int);
void quickSort(column_data, int , int);


column_data Sort(column_data);
void sorting (column_data *,column_data *,int, int , int , int);
Result * Join(column_data , column_data,int * );

column_data load_column_data(relation *, int rel,int col);
column_data load_from_IR(relation * ,int rel,int ,uint64_t ,uint64_t * );
Result * scan(column_data ,column_data ,int * );
uint64_t * Equalizer(column_data ,int ,int ,int *);

#endif //PROJECT_JOIN_19_20_SORT_JOIN_FUNS_H
