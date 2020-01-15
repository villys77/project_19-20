//
// Created by villys77 on 7/1/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "threads.h"
#include "functions.h"
#include "Intermediate_Result.h"
#include <string.h>


void * thread_function(void * args)
{

    struct args *my_args = (struct args *) args;

    char *token;
    char **tokens;

    tokens = malloc(3 * sizeof(char *));
    char* ts=NULL;
    token = strtok_r(my_args->line, "|",&ts);
    tokens[0]=strdup(token);

    token = strtok_r(NULL, "|",&ts);
    tokens[1] =strdup(token);

    token = strtok_r(NULL, "\n",&ts);
    tokens[2]  =strdup(token);

//        printf("%s|%s|%s\n",tokens[0],tokens[1],tokens[2]);


////////////////////////////////////////////////////////////////////////////////////
///////// antoistoixish twn 0,1,2.... pou anaferontai sta predicates stis pragamtikes sxeseis me to mapping

    int relations_to_check=0;
    int i;
    for(i=0; i<strlen(tokens[0]); i++)
    {
        //metraw ta kena pou exw wste na brw kai to poses sxeseis exw
        if(my_args->line[i] ==' ')
        {
            relations_to_check ++;
        }
    }
    relations_to_check++; // ta sunolika relation pou exoume na koitaksoume

    int * mapping=malloc(sizeof(int)*relations_to_check);
    char * re=NULL;
    char * sxeseis=strdup(tokens[0]);
    char * temp=strtok_r(sxeseis," ",&re);
    int id=strtol(temp,NULL,10);
    mapping[0]=id;
    for(i=1; i<relations_to_check; i++)
    {

        temp=strtok_r(re," ",&re);
        id=strtol(temp,NULL,10);
        mapping[i]=id;

    }
    free(sxeseis);


///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//////proteraiothta twn predicates ..poio 8a ginei 1o,2o klp ston pianaka prio
    int total_ques=0;
    for(i=0; i<strlen(tokens[1]); i++)
    {
        if(my_args->line[i+strlen(tokens[0])] =='&')
        {
            total_ques ++;
        }
    }
    total_ques++;

    char* pre =strdup(tokens[1]);

    pthread_mutex_lock(&my_args->mutex);
    struct Predicates* predicates=predicates_analysis(total_ques,pre,my_args->relations,mapping);
    pthread_mutex_unlock(&my_args->mutex);

    int *prio=predicates_priority(total_ques,predicates);

    pthread_mutex_lock(&my_args->mutex);
    reset_statistics(my_args->relations,my_args->original,tokens[0]);
    pthread_mutex_unlock(&my_args->mutex);

    free(pre);


////////////////////////////////////////////////////////////////////////////////////////////


    int show=0;
    for(i=0; i<strlen(tokens[2]); i++)
    {
        //metraw ta kena pou exw wste na brw kai to poses sxeseis exw
        if(tokens[2][i]==' ')
        {
            show++;
        }
    }
    show++; // ta sunolika rel

    my_args->shows[my_args->Sums_count]=show;


    Intermediate_Result * IR=exec_predicates(my_args->relations,predicates,prio,total_ques,relations_to_check,mapping);
    uint64_t * sums=Intermediate_Sum(IR,my_args->relations,mapping,tokens[2],show);

    my_args->all_sums[my_args->Sums_count]=sums;


    free(IR->relResults);
    for(int i=0; i<relations_to_check; i++)
    {

        free(IR->Related_Rels[i]);
        free(IR->resArray[i]);
    }
    free(IR->Related_Rels);
    free(IR->resArray);
    free(IR);



    free(tokens[0]);
    free(tokens[1]);
    free(tokens[2]);
    free(tokens);
    free(prio);
    free(predicates);
    free(mapping);

    free(my_args->line);
    pthread_mutex_destroy(&my_args->mutex);
}


thread_pool * thread_pool_init(int n_threads)
{
    int i;
    thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
    pool->threads = (pthread_t **)malloc(sizeof(pthread_t *)*n_threads);
    pool->threads_ids =(int *)malloc(sizeof(int)*n_threads);
    pool->threads_working=0;
    pool->job_queue = queue_init();
    pool->threads_alive = 0 ;
    pool->jobs_to_done=0;
    pool->jobs_done=0;

    pthread_mutex_init(&pool->barrier_mtx,NULL);
    pthread_mutex_init(&pool->job_queue_mtx,NULL);
    pthread_mutex_init(&pool->empty_queue_mtx,NULL);
    pthread_cond_init(&pool->empty_queue_cond,NULL);
    pthread_cond_init(&pool->barrier_cond,NULL);

    pthread_mutex_init(&pool->alive_mtx,NULL);
    pthread_cond_init(&pool->alive_cond,NULL);

    for(i=0;i<n_threads;i++)
    {
        pool->threads[i] = (pthread_t *)malloc(sizeof(pthread_t));
        pool->threads_ids[i] = pthread_create(pool->threads[i], NULL, (void *)ThreadJob, pool);
    }
    pthread_mutex_lock(&pool->alive_mtx);
    while (pool->threads_alive != n_threads)
    {
        pthread_cond_wait(&pool->alive_cond,&pool->alive_mtx);
    }
    pthread_mutex_unlock(&pool->alive_mtx);
    return pool;
}

void thread_pool_add_job(thread_pool * pool,void (*function)(void* arg),void *arg)
{
//  if(thp==NULL || thp->queue == NULL) return;
    job *newjob = (job *)malloc(sizeof(job));
    newjob->function = function;
    newjob->arg = arg;
    newjob->next = NULL;
    pthread_mutex_lock(&pool->job_queue_mtx);
    pool->jobs_to_done++;
    queue *jq = pool->job_queue;
    if(jq->n_jobs == 0) //first job
    {
        jq->first = jq->last = newjob;
    }
    else
    {
        jq->last->next = newjob;
        jq->last = newjob;
    }
    pool->job_queue->n_jobs++;
    pthread_cond_signal(&pool->empty_queue_cond);
    pthread_mutex_unlock(&pool->job_queue_mtx) ;
    //unlock mutex
}

queue * queue_init()
{
    queue *jq = (queue *)malloc(sizeof(queue));
    jq->first = NULL;
    jq->last = NULL;
    jq->n_jobs=0;
    return jq;
}


job * thread_pool_get_job(thread_pool * pool)
{
    job *j;
    pthread_mutex_lock(&pool->empty_queue_mtx);
    while(pool->job_queue->n_jobs==0)
    {
        pthread_cond_wait(&pool->empty_queue_cond,&pool->empty_queue_mtx);
    }
    pthread_mutex_lock(&pool->job_queue_mtx);
    pthread_mutex_lock(&pool->barrier_mtx);
    pool->job_queue->n_jobs--;
    pthread_mutex_unlock(&pool->barrier_mtx);
    pthread_mutex_unlock(&pool->empty_queue_mtx);
    j = pool->job_queue->first;
    pool->job_queue->first = j->next;
    pthread_mutex_unlock(&pool->job_queue_mtx);
    return j;
}

void ThreadJob(thread_pool * pool)
{
    void (*function)(void*);
    void *arg;
    pthread_mutex_lock(&pool->alive_mtx);
    pool->threads_alive += 1;
    pthread_cond_signal(&pool->alive_cond);
    pthread_mutex_unlock(&pool->alive_mtx);
    while(1)
    {
        job *my_job = thread_pool_get_job(pool);
        pthread_mutex_lock(&pool->barrier_mtx);
        pool->threads_working++;
        pthread_mutex_unlock(&pool->barrier_mtx);
        function = my_job->function;
        arg = my_job->arg;
        function(arg);
        pthread_mutex_lock(&pool->barrier_mtx);
        pool->threads_working--;
        pool->jobs_done++;
        if(pool->threads_working==0)
        {
            pthread_cond_signal(&pool->barrier_cond);
        }
        pthread_mutex_unlock(&pool->barrier_mtx);
        free(my_job->arg);
        free(my_job);

    }
}


void thread_pool_barrier(thread_pool * pool)
{
    pthread_mutex_lock(&pool->barrier_mtx);
    while(pool->jobs_done!=pool->jobs_to_done || pool->threads_working!=0)
    {
        pthread_cond_wait(&pool->barrier_cond,&pool->barrier_mtx);
    }
    pool->jobs_to_done=0;
    pool->jobs_done=0;
    pthread_mutex_unlock(&pool->barrier_mtx);
}

void thread_pool_destroy(thread_pool * pool)
{
    void *ret;
    for(int i=0;i<THREADS_NUM;i++)
    {
        thread_pool_add_job(pool,(void*)pthread_exit,NULL);
    }
    for(int i=0;i<THREADS_NUM;i++)
    {
        pthread_join(*pool->threads[i],&ret);
    }
    pthread_mutex_destroy(&pool->barrier_mtx);
    pthread_mutex_destroy(&pool->empty_queue_mtx);
    pthread_cond_destroy(&pool->empty_queue_cond);
    pthread_mutex_destroy(&pool->job_queue_mtx);
    pthread_cond_destroy(&pool->barrier_cond);
    pthread_mutex_destroy(&pool->alive_mtx);
    pthread_cond_destroy(&pool->alive_cond);

    for(int i=0; i<THREADS_NUM; i++)
    {
        free(pool->threads[i]);
    }
    free(pool->threads);
    free(pool->threads_ids);
    free(pool->job_queue->first);
    free(pool->job_queue->last);
    free(pool->job_queue);

    free(pool);


}
