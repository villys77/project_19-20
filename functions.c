//
// Created by villys on 5/11/2018.
//
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "functions.h"
#include "structs.h"


#define n 8



void swap(uint64_t * a, uint64_t * b)
{
    uint64_t t = *a;
    *a = *b;
    *b = t;
}

int partition (column_data arr, int low, int high)
{
    uint64_t pivot = arr.tuples[high].key;    // pivot
    int i = (low - 1);  // Index of smaller element

    for ( int j = low; j <= high- 1; j++)
    {
        if (arr.tuples[j].key < pivot)
        {
            i++;    // increment index of smaller element
            swap(&(arr.tuples[i].key),&(arr.tuples[j].key));
            swap(&(arr.tuples[i].payload),&(arr.tuples[j].payload));

        }
    }
    swap(&(arr.tuples[i+1].key), &(arr.tuples[high].key));
    swap(&(arr.tuples[i+1].payload), &(arr.tuples[high].payload));

    return (i + 1);
}

void quickSort(column_data arr, int low, int high)
{
    if(low < high)
    {
        int pi = partition(arr , low , high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1 , high);
    }
}


column_data Sort(column_data array0)
{
    column_data array1;
    array1.num_tuples=array0.num_tuples;
    array1.tuples=malloc(sizeof(tuple)*array1.num_tuples);
    for(int i=0 ; i<array1.num_tuples; i++)
    {
        array1.tuples[i].key=0;
        array1.tuples[i].payload=0;

    }
    sorting(&array0,&array1,0,array0.num_tuples,1,7);

    free(array0.tuples);
    return array1;
}

void sorting(column_data * array0, column_data * array1 ,int start,int end,int where_to_write,int byte)
{

    uint64_t power = pow(2, n) -1;     /// 2^n (megethos pinakwn psum kai hist)
    uint64_t mask=power;


    uint64_t  i=0,j=0;

    if(byte==0)
    {
        quickSort(*array1,start,end-1);
        return;
    }
    hist Hist[power];

    for (i = 0; i <= power; i++)
    {
        Hist[i].binary = i;
        Hist[i].count = 0;
    }

    for (i = start; i < end; i++)
    {
        for (j = 0; j <= power; j++)
        {
            if(where_to_write==1)
            {
                uint64_t aa=array0->tuples[i].key;
                uint64_t x =(aa >> (8*byte)) &  0xff;
                if ((x & mask) == Hist[j].binary)
                {
                    Hist[j].count++;
                    break;
                }
            }
            else
            {
                uint64_t aa=array1->tuples[i].key;
                uint64_t x =(aa >> (8*byte)) &  0xff;
                if ((x & mask) == Hist[j].binary)
                {
                    Hist[j].count++;
                    break;
                }
            }

        }
    }


    int ** index;
    index=malloc(sizeof(int*)*256);
    for(int counter=0; counter<256; counter++)
    {
        index[counter]=malloc(sizeof(int)*Hist[counter].count);
    }
    for(int row=0; row<256; row++)
    {
        for(int column=0; column<Hist[row].count; column++)
        {
            index[row][column]=-1;
        }
    }

    for(int row=0; row <256; row++)
    {
        for(int column=0; column<Hist[row].count; column++)
        {

        }
    }

        for(i=start; i<end; i++)
        {
            if(where_to_write==1)
            {
                uint64_t aa=array0->tuples[i].key;
                uint64_t x =(aa >> (8*byte)) &  0xff;
                uint64_t bt=x & mask;
                for(int column=0; column<Hist[bt].count; column++)
                {
                    if(index[bt][column]==-1)
                    {
                        index[bt][column]=(int)i;
                        break;
                    }
                }
            }
            else
            {
                uint64_t aa=array1->tuples[i].key;
                uint64_t x =(aa >> (8*byte)) &  0xff;
                uint64_t bt=x & mask;
                for(int column=0; column<Hist[bt].count; column++)
                {
                    if(index[bt][column]==-1)
                    {
                        index[bt][column]=(int)i;
                        break;
                    }
                }

            }
        }

//    for(int row=0; row<256; row++)
//    {
//        for(int column=0; column<Hist[row].count; column++)
//        {
//            printf("%d\n",index[row][column]);
//        }
//    }
//    return;
    ////////////////////////////////////////////////////////////////////
    //////////////PSUM
    hist Psum[power +1 ];
    for (i = 0; i<= power+1; i++)
    {
        Psum[i].binary = i;
        if (i == 0)
        {
            Psum[i].count = start;
        }
        else if(i==power+1)
        {
            Psum[i].count=end;
        }
        else
        {
            Psum[i].count = Psum[i - 1].count + Hist[i - 1].count;
        }
    }

    int where_in_array = 0;
    while (where_in_array <=power)
    {
        int column=0;
        for (i = Psum[where_in_array].count; i < Psum[where_in_array+1].count; i++)
        {
            if(where_to_write==1)
            {
                array1->tuples[i].key=array0->tuples[index[where_in_array][column]].key;
                array1->tuples[i].payload=array0->tuples[index[where_in_array][column]].payload;

            }
            else
            {
                array0->tuples[i].key=array1->tuples[index[where_in_array][column]].key;
                array0->tuples[i].payload=array1->tuples[index[where_in_array][column]].payload;

            }

            column++;
        }
        where_in_array++;
    }


    where_to_write^=1; ////change where to write

    where_in_array=0;
    while (where_in_array <=power)
    {
        if(Hist[where_in_array].count==0 || Psum[where_in_array].count==Psum[where_in_array+1].count)
        {
            where_in_array++;
            continue;
        }

        if( (Hist[where_in_array].count)*(sizeof(uint64_t)) <= QUICKSORT_SIZE )
        {
            if(where_to_write==1)
            {
                quickSort(*array0,Psum[where_in_array].count,Psum[where_in_array+1].count-1);
                memcpy(array1->tuples+Psum[where_in_array].count,array0->tuples+Psum[where_in_array].count,Hist[where_in_array].count*
                                                                                                           sizeof(tuple));
            }
            else
            {
                quickSort(*array1,Psum[where_in_array].count,Psum[where_in_array+1].count-1);
            }
        }
        else
        {
            if(Psum[where_in_array].count==start && Psum[where_in_array+1].count==end && where_to_write==0)
            {
                return;
            }
            sorting(array0,array1,Psum[where_in_array].count,Psum[where_in_array+1].count,where_to_write,byte-1);
        }


        where_in_array++;
    }
}


Result *ListInit()
{
    Result *node = (Result *)malloc(sizeof(Result));
    node->first=NULL;
    node->current=NULL;

    return node;
}

void InsertResult(uint64_t R_id,uint64_t S_id,Result *head)
{

    if(head->current==NULL) // adeia
    {
        head->current = (ResultNode *)malloc(sizeof(ResultNode));
        head->first = head->current ;
        head->first->next = NULL;
        head->first->counter=0;
    }
    else
    {
        if(head->current->counter == SIZE_NODE-1)
        {
            head->current->next = (ResultNode *)malloc(sizeof(ResultNode));
            head->current = head->current->next;
            head->current->next=NULL;
            head->current->counter=0;
        }
    }
    head->current->buffer[head->current->counter][0] = R_id ;
    head->current->buffer[head->current->counter][1] = S_id ;
    head->current->counter++;
}

void PrintResults(Result *head)
{
    FILE *fp;
    fp = fopen("Results.csv", "w");// "w" means that we are going to write on this file
    ResultNode *temp;
    int i,count=1;
    temp = head->first;
    if(temp==NULL)
    {
        printf("There are no matches.\n");
        return ;
    }
    while(temp!=NULL)
    {
        for(i=0;i<temp->counter ;i++)
        {
            fprintf(fp, "|%" PRIu64 "|%" PRIu64 "|\n", temp->buffer[i][0],temp->buffer[i][1]);
            count++;
        }
        temp = temp->next;
    }
    fclose(fp);

}

//////////////////////////////////
void freelist(Result *head)
{
    if(head==NULL)
    {
        return;
    }
    else
    {
        if(head->first == NULL)
        {
            free(head);
            return;
        }
        else
        {
            ResultNode* temp=(ResultNode*)malloc(sizeof(ResultNode*));
            temp = head->first;
            ResultNode *going_to_free;
            while(temp!=NULL)
            {
                going_to_free = temp;
                temp = temp->next;
                free(going_to_free);
            }
            free(head);
        }
    }
}

void Join(column_data R, column_data S )
{
    Result *ResultList = ListInit();
    int i = 0 , j = 0 , matches = 0 ,num_of_matches =0 ;

    if(R.num_tuples < S.num_tuples)
    {
        while(i < R.num_tuples && j < S.num_tuples)
        {
            if(R.tuples[i].key == S.tuples[j].key)
            {

                matches++;
                InsertResult(R.tuples[i].payload,S.tuples[j].payload,ResultList);
                num_of_matches++;

                j++;
            }
            else if(R.tuples[i].key > S.tuples[j].key)
            {

                j++;
            }
            else
            {

                i++;
                if(i==R.num_tuples)
                {
                    continue;
                }
                if((R.tuples[i].key == R.tuples[i-1].key) &&(matches !=0))
                {
                    j-= matches;
                }
                matches=0;
            }
            if( j >= S.num_tuples && i < R.num_tuples)
            {

                i++;
                j-=matches;
                matches =0;
            }
        }
    }
    else
    {
        while(i < S.num_tuples && j < R.num_tuples)
        {
            if(S.tuples[i].key == R.tuples[j].key)
            {

                matches ++;
                InsertResult(S.tuples[i].payload,R.tuples[j].payload,ResultList);
                num_of_matches++;
                j++;
            }
            else if(S.tuples[i].key > R.tuples[j].key)
            {
                j++;
            }
            else
            {
                i++;
                if(i==S.num_tuples)
                {
                    continue;
                }
                if((S.tuples[i].key == S.tuples[i-1].key) &&(matches !=0))
                {
                    j-= matches;
                }
                matches=0;
            }
            if( j >= R.num_tuples && i < S.num_tuples)
            {
                i++;
                j-=matches;
                matches =0;
            }
        }
    }
    PrintResults(ResultList);
    printf("Number of Joins: %d\n",num_of_matches);
    free(R.tuples);
    free(S.tuples);
    freelist(ResultList);
}

relation * read_file(char * filename,int *rels)
{
    char * name=malloc(sizeof(char)*strlen(filename)+1);
    strcpy(name,filename);

    char *dataset=strtok(name,"/");
    sprintf(dataset,"%s/",dataset);

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
    *rels=size_of_file;
    rewind(file);

    relation * relations=malloc(size_of_file* sizeof(relation));


    int i=0;
    while (!feof(file)) ////dinw times sta stoixeia tou prwtou pinaka
    {
        if ((my_read = getline(&line, &len, file)) != -1)
        {
            line[strcspn(line, "\n")] = 0;
            char * path=malloc(sizeof(char)*(strlen(line)+strlen(dataset)+3));
            sprintf(path,"%s%s",dataset,line);

            relations[i].data=loadRelation(path);
            relations[i].num_tuples=relations[i].data[0];
            relations[i].num_columns=relations[i].data[1];
            int size=relations[i].num_tuples*relations[i].num_columns;
            relations[i].data+=2;
            free(path);

        }

        i++;
    }
    rewind(file);


    free(name);

    fclose(file);
    free(line);
    return relations;
}

uint64_t * loadRelation(char* fileName)
{
    int fp = open(fileName, O_RDONLY);

    if (fp == -1)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    struct stat sb;

    if (fstat(fp,&sb)==-1)
    {
        perror("stat\n");
        exit(EXIT_FAILURE);
    }

    size_t length = sb.st_size;

    uint64_t* ptr = mmap(NULL, length ,PROT_READ, MAP_PRIVATE, fp, 0);

    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fp);
    return ptr;


}
