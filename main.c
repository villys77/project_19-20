#include <stdio.h>
#include "structs.h"
#include "functions.h"


int main(int argc,char **argv)
{
    if(argc!=3)
    {
        printf("Plase give 2 arguments\n");
        return 0;
    }

    relation relA=read_file(argv[1]);
    relation Sorting_relA=Sort(relA);

    relation relB=read_file(argv[2]);
    relation Sorting_relB=Sort(relB);

    Join(Sorting_relA,Sorting_relB);

    return  0;
}