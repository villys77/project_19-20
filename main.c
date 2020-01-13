#include <stdio.h>
#include <sys/mman.h>
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
#include <string.h>

//column_data read(char * filename)
//{
//    FILE *file;
//    file = fopen(filename, "r"); ////read file
//    if (file == NULL)
//    {
//        printf("Error! Please give correct arguments\n");
//        exit(0);
//    }
//
//
//    char *line = NULL;
//    size_t len = 0;
//    ssize_t my_read;
//
//    int size_of_file = 0;
//    while (!feof(file))
//    {
//        if ((my_read = getline(&line, &len, file)) != -1)
//        {
//            size_of_file++; //// count lines of doc
//        }
//    }
//
//    rewind(file);
//
//    column_data array0;
//
//    array0.num_tuples=size_of_file;
//    array0.tuples=malloc(sizeof(tuple)*array0.num_tuples);
//
//    int i=0;
//    while (!feof(file)) ////dinw times sta stoixeia tou prwtou pinaka
//    {
//        if ((my_read = getline(&line, &len, file)) != -1)
//        {
//
//            line[strcspn(line, "\n")] = 0;
//            char *str = strtok(line, ",");char * str1=strtok(NULL,"\n");
//
//            array0.tuples[i].key=strtoull(str,NULL,10);
//            array0.tuples[i].payload=strtoull(str1,NULL,10);
//
//        }
//        i++;
//    }
//
//    fclose(file);
//    free(line);
//    return array0;
//}

int main(int argc,char **argv)
{

//
//    column_data relA=read(argv[1]);
//    column_data sort_relA=Sort(relA);
//    column_data relB=read(argv[2]);
//    column_data sort_relB=Sort(relB);
//    int mat=0;
//    Join(sort_relA,sort_relB,&mat);
//
//    return 0;

    if(argc!=3)
    {
        printf("Plase give correct arguments\n");
        return 0;
    }

    int rels=0;
    struct statistics *original =NULL;
    relation *relations=read_file(argv[1],&rels, &original); ////pinakas apo relations
    for(int i=0; i<rels; i++)
    {
        for(int j=0; j<relations[i].num_columns; j++)
        {
//            printf("%lu %lu %lu %lu\n",relations[i].stats.min[j],relations[i].stats.max[j],relations[i].stats.number[j],relations[i].stats.distinct[j]);

//            printf("%lu %lu %lu %lu\n",original[i].min[j],original[i].max[j],original[i].number[j],original[i].distinct[j]);

        }
    }
//    return 0;
    queries_analysis(argv[2],relations,rels,original);

    free(relations);

    return  0;
}