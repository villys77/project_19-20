//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_FUNCTIONS_H
#define PROJECT_FUNCTIONS_H

#include "structs.h"
#include "threads.h"

void queries_analysis(char * ,relation*,int,struct statistics*,thread_pool *);
struct Predicates* predicates_analysis(int,char *,relation*,int *);
int * predicates_priority(int,struct Predicates *);
int * Join_Enumeration(relation*,int ,struct Predicates *,int *,int);
void count_statistics(relation * ,int * ,struct Predicates *, int,int );
Intermediate_Result * exec_predicates(relation *,struct Predicates *,int * ,int, int,int *);
void reset_statistics(relation*,struct statistics*,char*);


#endif //PROJECT_FUNCTIONS_H