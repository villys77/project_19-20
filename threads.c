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

    ///debugging printf("Exw sunolika %d sxeseis kai %d queries\n\n",relations_to_check,total_ques);

    char* pre =strdup(tokens[1]);
    pthread_mutex_lock(&my_args->mutex);
    struct Predicates* predicates=predicates_analysis(total_ques,pre,my_args->relations,mapping);
//        continue;
    pthread_mutex_unlock(&my_args->mutex);
    int *prio=predicates_priority(total_ques,predicates);

//    reset_statistics(relations,original,tokens[0]);
//        exit(0);
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

    pthread_mutex_lock(&my_args->mutex);/////lock
    my_args->shows[my_args->Sums_count]=show;
    pthread_mutex_unlock(&my_args->mutex);////unlock


    Intermediate_Result * IR=exec_predicates(my_args->relations,predicates,prio,total_ques,relations_to_check,mapping);
    uint64_t * sums=Intermediate_Sum(IR,my_args->relations,mapping,tokens[2],show);

    pthread_mutex_lock(&my_args->mutex);///lock
    my_args->all_sums[my_args->Sums_count]=sums;
//    my_args->Sums_count++;
    pthread_mutex_unlock(&my_args->mutex);////unlock



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

}


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