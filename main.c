#include <stdio.h>
#include <sys/mman.h>
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
#include <string.h>

column_data read(char * filename)
{
    FILE *file;
    file = fopen(filename, "r"); ////read file
    if (file == NULL)
    {
        printf("Error! Please give correct arguments\n");
        exit(0);
    }


    char *line = NULL;
    size_t len = 0;
    ssize_t my_read;

    int size_of_file = 0;
    while (!feof(file))
    {
        if ((my_read = getline(&line, &len, file)) != -1)
        {
            size_of_file++; //// count lines of doc
        }
    }

    rewind(file);

    column_data array0;

    array0.num_tuples=size_of_file;
    array0.tuples=malloc(sizeof(tuple)*array0.num_tuples);

    int i=0;
    while (!feof(file)) ////dinw times sta stoixeia tou prwtou pinaka
    {
        if ((my_read = getline(&line, &len, file)) != -1)
        {

            line[strcspn(line, "\n")] = 0;
            char *str = strtok(line, ",");
            char * str1=strtok(NULL,"\n");

            array0.tuples[i].key=strtoull(str,NULL,10);
            array0.tuples[i].payload=strtoull(str1,NULL,10);

        }
        i++;
    }

    fclose(file);
    free(line);
    return array0;
}


int main(int argc,char **argv)
{

//
//    column_data relA =read(argv[1]);
//    column_data relB =read(argv[2]);
//
//    column_data sort_relA=Sort(relA);
////quickSort(sort_relA,0,sort_relA.num_tuples-1);
////for(int i=0; i<sort_relA.num_tuples; i++)
////    {
////        printf("%lu\n",sort_relA.tuples[i].key);
////    }
//    column_data sort_relB=Sort(relB);
//    int a=0;
//    Join(sort_relA,sort_relB,&a);
////    printf("%d",a);
//    return 0;


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