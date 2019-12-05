//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_STRUCTS_H
#define PROJECT_STRUCTS_H

#include <stdint.h>
#define SIZE_NODE (1024*1024)/(2*sizeof(uint64_t))
#define QUICKSORT_SIZE 1024*64



typedef struct tuple
{
    uint64_t key;
    uint64_t payload;
}tuple;


typedef struct hist
{
    unsigned binary;
    unsigned count;
}hist;

typedef struct relation
{
    uint64_t num_tuples;
    uint64_t num_columns;
    uint64_t * data;
}relation;

typedef struct column_data
{
    struct tuple * tuples;
    uint64_t num_tuples;
}column_data;

typedef struct ResultNode
{
    uint64_t buffer[SIZE_NODE][2];
    int counter;
    struct ResultNode *next;
}ResultNode;


typedef struct Result{
    ResultNode *first;
    ResultNode *current;
}Result;


struct Predicates{
    int relation1; // h prwth sxesh
    int colum1; // h kolona apo thn prwth sxesh
    int relation2; // h deyterh sxesh
    int colum2; // h kolona apo thn deuterh sxesh
    int num;
    char op; //einai h praksi ('<','>','=')
    int prio;
};


typedef struct Intermediate_Result
{
    uint64_t * relResults; ///posa apotelesmata exei to tade relation
    uint64_t ** resArray;  ///ta apotelesmata
}Intermediate_Result;

#endif //PROJECT_STRUCTS_H