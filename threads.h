#ifndef _THPOOL_H_
#define _THPOOL_H_

#include <pthread.h>
#include "structs.h"

#define N_THREADS 8



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
    pthread_t **threads;
    int * th_ids;
    int jobs2bedone;
    int jobsdone;
    int n_threads;
    int threads_alive;
    int threads_working;
    queue *queue;
    pthread_mutex_t barrier_mtx;
    pthread_mutex_t empty_queue_mtx;
    pthread_cond_t empty_queue ;
    pthread_mutex_t mtx_queue ;
    pthread_cond_t barrier_cond;
    pthread_mutex_t alive_mtx ;
    pthread_cond_t alive_cond;

}thread_pool;

void ThreadJob(thread_pool *);
job * thread_pool_get_job(thread_pool * );
thread_pool * thread_pool_init(int );
void thread_pool_add_job(thread_pool * ,void (*function)(void* arg),void *);
queue * queue_init();
void thread_pool_wait(thread_pool * );
void thread_pool_barrier(thread_pool * );
void thread_pool_destroy(thread_pool * );
#endif