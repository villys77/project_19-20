#include <stdio.h>
#include "structs.h"
#include "functions.h"



int main(int argc,char **argv)
{
    if(argc!=2)
    {
        printf("Plase give 1 argument\n");
        return 0;
    }



    relation *relations=read_file(argv[1]); ////pinakas apo relations


    return  0;
}