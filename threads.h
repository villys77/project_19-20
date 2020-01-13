#ifndef _THPOOL_H_
#define _THPOOL_H_

#include <pthread.h>
#define N_THREADS 3


typedef struct job{
    void   (*function)(void* arg);
    void*  arg;
    struct job *next;
} job;

typedef struct job_queue{
    job *first;
    job *last;
    int n_jobs;
}job_queue;

typedef struct{
    pthread_t **threads;
    int *threads_id;
    int jobs2bedone;
    int jobsdone;
    int n_threads;
    int threads_alive;
    int threads_working;
    job_queue *queue;
    pthread_mutex_t barrier_mtx;
    pthread_mutex_t working_mtx ;
    pthread_mutex_t empty_queue_mtx;
    pthread_cond_t empty_queue ;
    pthread_mutex_t mtx_queue ;
    pthread_cond_t barrier_cond;
    pthread_mutex_t alive_mtx ;
    pthread_cond_t alive_cond;

}threadpool;

void ThreadJob(threadpool *thpool);
job *GetJob(threadpool *thpool);
threadpool *THP_Init(int n_threads);
void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg);
job_queue *JQ_Init();
void THP_Wait(threadpool *thp);
void THP_Barrier(threadpool *thp);
void THP_Destroy(threadpool *thp);
#endif