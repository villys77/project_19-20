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
struct Predicates* predicates_analysis(int,char *);
int * predicates_priority(int,struct Predicates *);
column_data load_column_data(relation *, int rel,int col);

int * Equalizer(column_data array,int given_num,int given_mode,int *matches);
void exec_predicates(relation *,struct Predicates *,int * ,int, int,int *);


void swap(uint64_t * a, uint64_t * b);
int partition (column_data, int , int);
void quickSort(column_data, int , int);

#endif //PROJECT_FUNCTIONS_H