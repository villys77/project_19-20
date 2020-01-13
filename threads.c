#include <stdio.h>
#include <stdlib.h>
#include "threads.h"
#include <pthread.h>



threadpool *THP_Init(int n_threads)
{
    int i;
    threadpool *thpool = (threadpool *)malloc(sizeof(threadpool));
    thpool->threads = (pthread_t **)malloc(sizeof(pthread_t *)*n_threads);
    thpool->threads_id =(int *)malloc(sizeof(int)*n_threads);
    thpool->threads_working=0;
    thpool->queue = JQ_Init();
    thpool->threads_alive = 0 ;
    thpool->jobs2bedone=0;
    thpool->jobsdone=0;

    pthread_mutex_init(&thpool->working_mtx,NULL);
    pthread_mutex_init(&thpool->barrier_mtx,NULL);
    pthread_mutex_init(&thpool->mtx_queue,NULL);
    pthread_mutex_init(&thpool->empty_queue_mtx,NULL);
    pthread_cond_init(&thpool->empty_queue,NULL);
    pthread_cond_init(&thpool->barrier_cond,NULL);

    pthread_mutex_init(&thpool->alive_mtx,NULL);
    pthread_cond_init(&thpool->alive_cond,NULL);

    for(i=0;i<n_threads;i++)
    {
        thpool->threads[i] = (pthread_t *)malloc(sizeof(pthread_t));
        thpool->threads_id[i] = pthread_create(thpool->threads[i], NULL, (void *)ThreadJob, thpool);
    }
    pthread_mutex_lock(&thpool->alive_mtx);
    while (thpool->threads_alive != n_threads)
    {
        pthread_cond_wait(&thpool->alive_cond,&thpool->alive_mtx);
    }
    pthread_mutex_unlock(&thpool->alive_mtx);
    return thpool;
}

void THP_AddJob(threadpool *thp,void (*function)(void* arg),void *arg)
{
//  if(thp==NULL || thp->queue == NULL) return;
    job *newjob = (job *)malloc(sizeof(job));
    newjob->function = function;
    newjob->arg = arg;
    newjob->next = NULL;
    pthread_mutex_lock(&thp->mtx_queue) ;
    thp->jobs2bedone++;
    job_queue *jq = thp->queue;
    if(jq->n_jobs == 0) //first job
    {
        jq->first = jq->last = newjob;
    }
    else
    {
        jq->last->next = newjob;
        jq->last = newjob;
    }
    thp->queue->n_jobs++;
    pthread_cond_signal(&thp->empty_queue);
    pthread_mutex_unlock(&thp->mtx_queue) ;
    //unlock mutex
}

job_queue *JQ_Init()
{
    job_queue *jq = (job_queue *)malloc(sizeof(job_queue));
    jq->first = NULL;
    jq->last = NULL;
    jq->n_jobs=0;
    return jq;
}


job *GetJob(threadpool *thp)
{
    job *j;
    pthread_mutex_lock(&thp->empty_queue_mtx);
    while(thp->queue->n_jobs==0)
    {
        pthread_cond_wait(&thp->empty_queue,&thp->empty_queue_mtx);
    }
    pthread_mutex_lock(&thp->mtx_queue);
    pthread_mutex_lock(&thp->barrier_mtx);
    thp->queue->n_jobs--;
    pthread_mutex_unlock(&thp->barrier_mtx);
    pthread_mutex_unlock(&thp->empty_queue_mtx);
    j = thp->queue->first;
    thp->queue->first = j->next;
    pthread_mutex_unlock(&thp->mtx_queue);
    return j;
}

void ThreadJob(threadpool *thpool)
{
    void (*function)(void*);
    void *arg;
    pthread_mutex_lock(&thpool->alive_mtx);
    thpool->threads_alive += 1;
    pthread_cond_signal(&thpool->alive_cond);
    pthread_mutex_unlock(&thpool->alive_mtx);
    while(1)
    {
        job *my_job = GetJob(thpool);
        pthread_mutex_lock(&thpool->barrier_mtx);
        thpool->threads_working++;
        pthread_mutex_unlock(&thpool->barrier_mtx);
        function = my_job->function;
        arg = my_job->arg;
        function(arg);
        pthread_mutex_lock(&thpool->barrier_mtx);
        thpool->threads_working--;
        thpool->jobsdone++;
        if(thpool->threads_working==0) pthread_cond_signal(&thpool->barrier_cond);
        pthread_mutex_unlock(&thpool->barrier_mtx);
    }
}


void THP_Barrier(threadpool *thp)
{
    pthread_mutex_lock(&thp->barrier_mtx);
    while(thp->jobsdone!=thp->jobs2bedone || thp->threads_working!=0)
    {
        pthread_cond_wait(&thp->barrier_cond,&thp->barrier_mtx);
    }
    thp->jobs2bedone=0;
    thp->jobsdone=0;
    pthread_mutex_unlock(&thp->barrier_mtx);
}

void THP_Destroy(threadpool *thp)
{
    void *ret;
    for(int i=0;i<N_THREADS;i++)
    {
        THP_AddJob(thp,(void*)pthread_exit,NULL);
    }
    for(int i=0;i<N_THREADS;i++)
    {
        pthread_join(*thp->threads[i],&ret);
    }
    pthread_mutex_destroy(&thp->barrier_mtx);
    pthread_mutex_destroy(&thp->working_mtx);
    pthread_mutex_destroy(&thp->empty_queue_mtx);
    pthread_cond_destroy(&thp->empty_queue);
    pthread_mutex_destroy(&thp->mtx_queue);
    pthread_cond_destroy(&thp->barrier_cond);
    pthread_mutex_destroy(&thp->alive_mtx);
    pthread_cond_destroy(&thp->alive_cond);

    free(thp->threads);
    free(thp->threads_id);
    free(thp);

}