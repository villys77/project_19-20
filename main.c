#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "functions.h"
#include <inttypes.h>

int main(int argc,char **argv)
{

    if(argv[1]==NULL || argv[2]==NULL)
    {
        printf("Plase give 2 arguments\n");
        return 0;
    }

    relation relA=read_file(argv[1]);
    relation Sorting_relA=Sort(relA);

//    for(int i=0; i<relA.num_tuples; i++)
//    {
//        printf("%llu\n",Sorting_relA.tuples[i].key);
//    }

    printf("eeeee\n\n");

    /////////
    /////mexri edw gia Relation A

    relation relB=read_file(argv[2]);

    relation Sorting_relB=Sort(relB);

    printf("eeeee\n\n");

//    for(int i=0; i<relB.num_tuples; i++)
//    {
//        printf("%llu\n",Sorting_relB.tuples[i].key);
//    }



    Join(Sorting_relA,Sorting_relB);



    free(Sorting_relA.tuples);
    free(Sorting_relB.tuples);

    return  0;
}