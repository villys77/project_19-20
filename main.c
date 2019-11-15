#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"
#include <time.h>


int main(int argc,char **argv)
{
    if(argv[1]==NULL || argv[2]==NULL)
    {
        printf("Plase give 2 arguments\n");
        return 0;
    }

    relation relA=read_file(argv[1]);
    relation Sorting_relA=Sort(relA);

    relation relB=read_file(argv[2]);
    relation Sorting_relB=Sort(relB);

    Join(Sorting_relA,Sorting_relB);



    free(Sorting_relA.tuples);
    free(Sorting_relB.tuples);

    return  0;
}