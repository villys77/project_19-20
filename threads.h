//
// Created by villys77 on 7/1/20.
//

#ifndef _THREADS_H_
#define _THREADS_H_

#include <pthread.h>
#include "structs.h"

#define THREADS_NUM 8

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

void * thread_function(void *);

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

void ThreadJob(thread_pool *);
job * thread_pool_get_job(thread_pool * );
thread_pool * thread_pool_init(int );
void thread_pool_add_job(thread_pool * ,void (*function)(void* arg),void *);
queue * queue_init();
void thread_pool_barrier(thread_pool * );
void thread_pool_destroy(thread_pool * );
#endif