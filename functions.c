//
// Created by villys on 5/11/2018.
//
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "functions.h"

#include "structs.h"


#define n 8



void swap(uint64_t * a, uint64_t * b)
{
    uint64_t t = *a;
    *a = *b;
    *b = t;
}

int partition (relation arr, int low, int high)
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

void quickSort(relation arr, int low, int high)
{
    if(low < high)
    {
        int pi = partition(arr , low , high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1 , high);
    }
}


relation Sort(relation array0)
{
    relation array1;
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

void sorting(relation * array0, relation * array1 ,int start,int end,int where_to_write,int byte)
{
    switch (byte)
    {
        case 6:
            where_to_write=0;
            break;
        case 5:
            where_to_write=1;
            break;
        case 4:
            where_to_write=0;
            break;
        case 3:
            where_to_write=1;
            break;
        case 2:
            where_to_write=0;
            break;
        case 1:
            where_to_write=1;
            break;
        case 0:
            where_to_write=0;
            break;
    }


    // printf("mphka me start %d kai end %d byte %d kai where %d\n",start,end,byte,where_to_write);


    uint64_t power = pow(2, n) -1;     /// 2^n (megethos pinakwn psum kai hist)
    uint64_t mask=power;


    uint64_t  i=0,j=0;

    for(i=0; i<array0->num_tuples; i++)
    {
        array0->tuples[i].check=0;
        array1->tuples[i].check=0;

    }



    if(byte==7)
    {

        ////////////////////////////////////////////////////////
        ////////HIST


        hist Hist[power];

        for (i = 0; i <= power; i++)
        {
            Hist[i].binary = i;
            Hist[i].count = 0;
        }

        for (i = 0; i < array0->num_tuples; i++)
        {
            for (j = 0; j <= power; j++)
            {
                uint64_t aa=array0->tuples[i].key;
                uint64_t x =(aa >> (8*byte)) &  0xff;
                if (  (x & mask) == Hist[j].binary )
                {
                    Hist[j].count++;
                    break;
                }
            }
        }



        ////////////////////////////////////////////////////////////////////
        //////////////PSUM
        hist Psum[power];
        for (i = 0; i<= power; i++)
        {
            Psum[i].binary = i;
            if (i == 0)
            {
                Psum[i].count = 0;
            }
            else
            {
                Psum[i].count = Psum[i - 1].count + Hist[i - 1].count;
            }
        }


        //////antigrafh timwn stous pinakes

        int where_in_array = 0;
        while (where_in_array <=power)
        {
            if (where_in_array == power) ///////pepiptwsh pou eimaste sthn teleytaia epanalh4h
            {
                for (i = Psum[where_in_array].count; i < array0->num_tuples; i++)
                {
                    for (j = 0; j < array0->num_tuples; j++)
                    {
                        uint64_t aa=array0->tuples[j].key;
                        uint64_t x =(aa >> (8*byte)) &  0xff;
                        if( ( (x & mask) == where_in_array ) && (array0->tuples[j].check == 0)  )
                        {
                            array1->tuples[i].key = array0->tuples[j].key;
                            array1->tuples[i].payload = array0->tuples[j].payload;
                            array0->tuples[j].check = 1;
                            break;
                        }
                    }
                }
                break;
            }
            else ///opoiadhpote endiamesh periptwsh
            {

                for (i = Psum[where_in_array].count; i < Psum[where_in_array + 1].count; i++)
                {
                    for (j = 0; j < array0->num_tuples; j++)
                    {
                        uint64_t aa=array0->tuples[j].key;
                        uint64_t x =(aa >> (8*byte)) &  0xff;
                        if( ( (x & mask) == where_in_array ) && (array0->tuples[j].check == 0)  )////an matcharei aki den exei ksanatsekaristei
                        {
                            array1->tuples[i].key = array0->tuples[j].key;
                            array1->tuples[i].payload = array0->tuples[j].payload;
                            array0->tuples[j].check = 1;
                            break;
                        }
                    }
                }
            }

            where_in_array++;
        }





        where_in_array=0;
        while (where_in_array <=power)
        {
            if (where_in_array == power) ///////pepiptwsh pou eimaste sthn teleytaia epanalh4h
            {

                if(Hist[where_in_array].count==0 )
                {
                    where_in_array++;
                    continue;
                }

                if( (Hist[where_in_array].count) <= 8000  )
                {
                    quickSort(*array1,Psum[where_in_array].count,array0->num_tuples-1);
                }
                else
                {
                    sorting(array0,array1,Psum[where_in_array].count,array0->num_tuples,where_to_write,byte-1);
                }
            }
            else ///opoiadhpote endiamesh periptwsh
            {


                if(Hist[where_in_array].count==0 || Psum[where_in_array].count==Psum[where_in_array+1].count)
                {
                    where_in_array++;
                    continue;
                }

                if( (Hist[where_in_array].count) <= 8000 )
                {
                    quickSort(*array1,Psum[where_in_array].count,Psum[where_in_array+1].count-1);

                }
                else
                {
                    sorting(array0,array1,Psum[where_in_array].count,Psum[where_in_array+1].count,where_to_write,byte-1);
                }
            }

            where_in_array++;
        }

    }
    else
    {
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
                        int k=x & mask;
                        Hist[j].count++;
                        break;
                    }
                }

            }
        }



        ////////////////////////////////////////////////////////////////////
        //////////////PSUM
        hist Psum[power];
        for (i = 0; i<= power; i++)
        {
            Psum[i].binary = i;
            if (i == 0)
            {
                Psum[i].count = start;
            }
            else
            {
                Psum[i].count = Psum[i - 1].count + Hist[i - 1].count;
            }
        }


        int where_in_array = 0;
        while (where_in_array <=power)
        {
            if (where_in_array == power) ///////pepiptwsh pou eimaste sthn teleytaia epanalh4h
            {
                for (i = Psum[where_in_array].count; i < end; i++)
                {
                    if(where_to_write==1)
                    {
                        for (j = start; j < end; j++)
                        {
                            uint64_t aa=array0->tuples[j].key;
                            uint64_t x =(aa >> (8*byte)) &  0xff;
                            if( ( (x & mask) == where_in_array ) && (array0->tuples[j].check == 0)  )
                            {
                                array1->tuples[i].key = array0->tuples[j].key;
                                array1->tuples[i].payload = array0->tuples[j].payload;
                                array0->tuples[j].check = 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = start; j < end; j++)
                        {
                            uint64_t aa=array1->tuples[j].key;
                            uint64_t x =(aa >> (8*byte)) &  0xff;
                            if( ( (x & mask) == where_in_array )  && (array1->tuples[j].check == 0)  )
                            {
                                array0->tuples[i].key = array1->tuples[j].key;
                                array0->tuples[i].payload = array1->tuples[j].payload;
                                array1->tuples[j].check = 1;
                                break;
                            }
                        }
                    }
                }
                break;
            }
            else ///opoiadhpote endiamesh periptwsh
            {

                for (i = Psum[where_in_array].count; i < Psum[where_in_array + 1].count; i++)
                {
                    if(where_to_write==1)
                    {
                        for (j = start; j < end; j++)
                        {
                            uint64_t aa=array0->tuples[j].key;
                            uint64_t x =(aa >> (8*byte)) &  0xff;
                            if( ( (x & mask) == where_in_array ) && (array0->tuples[j].check == 0)  )////an matcharei aki den exei ksanatsekaristei
                            {
                                array1->tuples[i].key = array0->tuples[j].key;
                                array1->tuples[i].payload = array0->tuples[j].payload;
                                array0->tuples[j].check = 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = start; j < end; j++)
                        {
                            uint64_t aa=array1->tuples[j].key;
                            uint64_t x =(aa >> (8*byte)) &  0xff;
                            if( ( (x & mask) == where_in_array )  && (array1->tuples[j].check == 0) )////an matcharei aki den exei ksanatsekaristei
                            {
                                array0->tuples[i].key = array1->tuples[j].key;
                                array0->tuples[i].payload = array1->tuples[j].payload;
                                array1->tuples[j].check = 1;
                                break;
                            }
                        }
                    }
                }
            }

            where_in_array++;
        }


        if(where_to_write==1)
        {
            where_to_write=0;
        }
        else
        {
            where_to_write=1;
        }
        where_in_array=0;
        while (where_in_array <=power)
        {

            if (where_in_array == power) ///////pepiptwsh pou eimaste sthn teleytaia epanalh4h
            {

                if(Hist[where_in_array].count==0 )
                {
                    where_in_array++;
                    continue;
                }


                if( (Hist[where_in_array].count) <= 8000)
                {


                    if(where_to_write==1)
                    {
                        quickSort(*array0,Psum[where_in_array].count,end-1);
                        for(int s=Psum[where_in_array].count; s< end; s++)
                        {
                            array1->tuples[s].key=array0->tuples[s].key;
                            array1->tuples[s].payload=array0->tuples[s].payload;

                        }

                    }
                    else
                    {
                        quickSort(*array1,Psum[where_in_array].count,end-1);
                    }
                }
                else
                {
                        sorting(array0,array1,Psum[where_in_array].count,end,where_to_write,byte-1);
                }
            }
            else ///opoiadhpote endiamesh periptwsh
            {



                if(Hist[where_in_array].count==0 || Psum[where_in_array].count==Psum[where_in_array+1].count)
                {
                    where_in_array++;
                    continue;
                }

                if( (Hist[where_in_array].count) <= 8000 )
                {
                    if(where_to_write==1)
                    {
                        quickSort(*array0,Psum[where_in_array].count,Psum[where_in_array+1].count-1);
                        for(int s=Psum[where_in_array].count; s<Psum[where_in_array+1].count; s++)
                        {
                            //printf(" apo 1 %llu\n",array0->tuples[s].key);
                            array1->tuples[s].key=array0->tuples[s].key;
                            array1->tuples[s].payload=array0->tuples[s].payload;

                        }
                    }
                    else
                    {
                        quickSort(*array1,Psum[where_in_array].count,Psum[where_in_array+1].count-1);
                    }
                }
                else
                {
                    if(Psum[where_in_array].count==start && Psum[where_in_array+1].count==end)
                    {
                        return;
                    }
                    sorting(array0,array1,Psum[where_in_array].count,Psum[where_in_array+1].count,where_to_write,byte-1);
                }
            }

            where_in_array++;
        }




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
        head->first->c=0;
    }
    else
    {
        if(head->current->c == SIZE_NODE-1)
        {
            head->current->next = (ResultNode *)malloc(sizeof(ResultNode));
            head->current = head->current->next;
            head->current->next=NULL;
            head->current->c=0;
        }
    }
    head->current->buffer[head->current->c][0] = R_id ;
    head->current->buffer[head->current->c][1] = S_id ;
    head->current->c++;
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
        for(i=0;i<temp->c;i++)
        {
            fprintf(fp, "|%" PRIu64 "|%" PRIu64 "|\n", temp->buffer[i][0],temp->buffer[i][1]);
            count++;
        }
        temp = temp->next;
    }
    fclose(fp);

}
//

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
//
void Join(relation R, relation S )
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
                //stelnw se lista
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
    //PrintResults(ResultList);
    printf("Number of Joins: %d\n",num_of_matches);
    freelist(ResultList);
}

relation read_file(char * filename)
{
    FILE *file;
    file = fopen(filename, "r"); ////read file
    if (file == NULL)
    {
        printf("Error");
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

    relation array0;

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
            array0.tuples[i].check=0;

        }
        i++;
    }

    fclose(file);
    free(line);
    return array0;
}