#include <stdio.h>
#include <sys/mman.h>
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
#include <string.h>



int main(int argc,char **argv)
{

    if(argc!=3)
    {
        printf("Plase give 1 argument\n");
        return 0;
    }

    int rels;
    relation *relations=read_file(argv[1],&rels); ////pinakas apo relations

    queries_analysis(argv[2],relations);

    for(int i=0; i<rels; i++)
    {
        munmap(relations[i].data,relations[i].num_columns*relations[i].num_tuples);
    }
    free(relations);

    return  0;
}