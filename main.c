#include <stdio.h>
#include "structs.h"
#include "functions.h"
#include "threads.h"
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv)
{

    if(argc!=3)
    {
        printf("Plase give correct arguments\n");
        return 0;
    }

    struct statistics *original =NULL;
    int rels=0;
    relation *relations=read_file(argv[1],&rels, &original); ////pinakas apo relations

    thread_pool * pool_threads=thread_pool_init(THREADS_NUM); ////Thread Pool
    queries_analysis(argv[2],relations,rels,original,pool_threads);

    thread_pool_destroy(pool_threads);
    if(original)
    {
        free(original);
    }
    free(relations);

    return  0;
}