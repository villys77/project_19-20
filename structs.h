//
// Created by villys on 5/11/2018.
//

#ifndef PROJECT_STRUCTS_H
#define PROJECT_STRUCTS_H

#include <stdint.h>
#include <pthread.h>
#define SIZE_NODE (1024*1024)/(2*sizeof(uint64_t))
#define QUICKSORT_SIZE 1024*64



////////////////////////////////
///////QUERIES

typedef struct statistics
{
    uint64_t * min;
    uint64_t * max;
    uint64_t * number;
    uint64_t * distinct;
    int ** dis_vals;
}statistics;


struct Predicates{
    int relation1; // h prwth sxesh
    int colum1; // h kolona apo thn prwth sxesh
    int relation2; // h deyterh sxesh
    int colum2; // h kolona apo thn deuterh sxesh
    int num;
    char op; //einai h praksi ('<','>','=')
    int prio;
};

////////////////////////////////

typedef struct relation
{
    uint64_t num_tuples;
    uint64_t num_columns;
    uint64_t * data;
    struct statistics stats;

}relation;


////////////////////////////////////
////////SORT-JOIN

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


typedef struct Intermediate_Result
{
    uint64_t * relResults; ///posa apotelesmata exei to tade relation
    uint64_t ** resArray;  ///ta apotelesmata
    uint64_t ** Related_Rels;
}Intermediate_Result;
/////////////////////////////////////


////////////////////////////////////
/////// THREADS
typedef struct job
{
    void   (*function)(void* arg);
    void * arg;
    struct job * next;
} job;

typedef struct queue
{
    job *first;
    job *last;
    int n_jobs;
}queue;

typedef struct thread_pool
{
    pthread_t ** threads;
    int * threads_ids;
    int jobs_to_done;
    int jobs_done;
    int threads_alive;
    int threads_working;
    queue * job_queue;
    pthread_mutex_t empty_queue_mtx;
    pthread_cond_t empty_queue_cond;
    pthread_mutex_t barrier_mtx;
    pthread_cond_t barrier_cond;
    pthread_mutex_t alive_mtx ;
    pthread_cond_t alive_cond;
    pthread_mutex_t job_queue_mtx;

}thread_pool;



typedef struct args
{
    char * line;
    int rels;
    relation * relations;
    struct statistics * original;
    int Sums_count;
    uint64_t **all_sums;
    uint64_t *shows;
    pthread_mutex_t mutex;
}args;

//////////////////////////////////////
//////////////////////////////////
#endif //PROJECT_STRUCTS_H