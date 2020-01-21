//
// Created by villys77 on 7/1/20.
//


#ifndef _THREADS_H_
#define _THREADS_H_

#include <pthread.h>
#include "structs.h"

#define THREADS_NUM 4


void * thread_function(void *);

void ThreadJob(thread_pool *);
job * thread_pool_get_job(thread_pool * );
thread_pool * thread_pool_init(int );
void thread_pool_add_job(thread_pool * ,void (*function)(void* arg),void *);
queue * queue_init();
void thread_pool_barrier(thread_pool * );
void thread_pool_destroy(thread_pool * );

#endif
