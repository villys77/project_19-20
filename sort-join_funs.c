//
// Created by villys77 on 17/1/20.
//

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "structs.h"
#include "Result.h"
#include "sort-join_funs.h"


relation * read_file(char * filename,int *rels ,struct statistics ** original)
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
    *original=malloc(sizeof(struct statistics)*size_of_file);
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
//////////
/////statistics
            relations[i].stats.min=malloc(sizeof(uint64_t)* relations[i].num_columns);
            relations[i].stats.max=malloc(sizeof(uint64_t)* relations[i].num_columns);
            relations[i].stats.number=malloc(sizeof(uint64_t)* relations[i].num_columns);
            relations[i].stats.distinct=malloc(sizeof(uint64_t)* relations[i].num_columns);
            relations[i].stats.dis_vals=malloc(sizeof(int*)*relations[i].num_columns);


            (*original)[i].min=malloc(sizeof(uint64_t)* relations[i].num_columns);
            (*original)[i].max=malloc(sizeof(uint64_t)* relations[i].num_columns);
            (*original)[i].number=malloc(sizeof(uint64_t)* relations[i].num_columns);
            (*original)[i].distinct=malloc(sizeof(uint64_t)* relations[i].num_columns);

            relations[i].data+=2;



            int j,k,id=0;
            for(j=0; j<relations[i].num_columns; j++)
            {
                if (j > 0)
                {
                    id += relations[i].num_tuples;
                }
                uint64_t temp_max = 0;
                uint64_t temp_min = 100000000;
                for (k = id; k < id + relations[i].num_tuples; k++)
                {
                    if (relations[i].data[k] > temp_max)
                    {
                        temp_max = relations[i].data[k];
                    }
                    if (relations[i].data[k] < temp_min)
                    {
                        temp_min = relations[i].data[k];
                    }
                }
                relations[i].stats.max[j] = temp_max;
                relations[i].stats.min[j] = temp_min;
                relations[i].stats.number[j] = relations[i].num_tuples;

                (*original)[i].max[j] = temp_max;
                (*original)[i].min[j] = temp_min;
                (*original)[i].number[j] = relations[i].num_tuples;

            }

            bool **dists;

            uint64_t *dist_size=malloc(sizeof(uint64_t)*relations[i].num_columns);
            int flag_n=0;
            dists = malloc(sizeof(bool*) * relations[i].num_columns);
            for(j=0; j<relations[i].num_columns; j++)
            {
                dist_size[j]=relations[i].stats.max[j]-relations[i].stats.min[j]+1;
                if(relations[i].stats.max[j]-relations[i].stats.min[j]+1>50000000)
                {
                    flag_n=1;
                    dist_size[j]=50000000;
                }
                dists[j]=malloc(sizeof(bool)*dist_size[j]);
                relations[i].stats.dis_vals[j]=malloc(sizeof(int)*dist_size[j]);

                uint64_t z;


                for(z=0; z<dist_size[j]; z++)
                {
                    relations[i].stats.dis_vals[j][z]=0;
                    dists[j][z]=false;
                }
            }

            id=0;
            for(j=0; j<relations[i].num_columns; j++)
            {
                if (j > 0)
                {
                    id += relations[i].num_tuples;
                }
                for (k = id; k < id + relations[i].num_tuples; k++)
                {
                    if(flag_n==1)
                    {
                        dists[j][(relations[i].data[k]-relations[i].stats.min[j]) % 50000000]=true;
                    }
                    else
                    {
                        dists[j][relations[i].data[k]-relations[i].stats.min[j]]=true;
                    }
                }
            }

            uint64_t counter=0;

            int c;
            for(j=0; j<relations[i].num_columns; j++)
            {
                uint64_t z;
                counter=0;
                c=0;
                for(z=0; z<dist_size[j]; z++)
                {
                    if(dists[j][z]==true)
                    {
                        if(z+relations[i].stats.min[j]<dist_size[j] && c<dist_size[j])
                        {
                            relations[i].stats.dis_vals[j][c]=(int)z+(int)relations[i].stats.min[j];
                            c++;

                        }

                        counter++;
                    }
                }

                relations[i].stats.distinct[j]=counter;
                (*original)[i].distinct[j]=counter;

            }

//////////////
//////////////

            free(path);
            for(k=0;k < relations[i].num_columns; k++)
            {
                free(dists[k]);
            }
            free(dist_size);
            free(dists);

        }

        i++;
    }
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
    int flag=0;
    for(int i=low+1 ; i<=high; i++)
    {
        if(arr.tuples[i-1].key > arr.tuples[i].key)
        {
            flag=1;
            break;
        }

    }
    if(flag==0)
    {
        return ;
    }

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

    int flag=0;
    for(int i=1 ; i<array0.num_tuples; i++)
    {
        if(array0.tuples[i-1].key > array0.tuples[i].key)
        {
            flag=1;
            break;
        }

    }
    if(flag==0)
    {
        memcpy(array1.tuples,array0.tuples,array1.num_tuples* sizeof(uint64_t)*2);
        free(array0.tuples);
        return array1;
    }
    uint64_t mask=pow(2, n) -1;;

    int bt=7;
    flag=0;
    while(bt>0)
    {
        for(int i=0;i<array0.num_tuples; i++)
        {
            uint64_t aa=array0.tuples[i].key;
            uint64_t x =(aa >> (8*bt)) &  0xff;
            if((x & mask)>0)
            {
                flag=1;
                break;
            }


        }
        if(flag==1)
        {
            break;
        }
        bt--;
    }

    sorting(&array0,&array1,0,array0.num_tuples,1,bt);

    free(array0.tuples);
    return array1;
}

void sorting(column_data * array0, column_data * array1 ,int start,int end,int where_to_write,int byte)
{
    uint64_t power = pow(2, n) -1;     /// 2^n (megethos pinakwn psum kai hist)
    uint64_t mask=power;

    uint64_t  i=0,j=0;

    if(byte==0 && start==0 && end==array0->num_tuples )
    {
        quickSort(*array0,start,end-1);
        memcpy(array1->tuples,array0->tuples,array0->num_tuples* sizeof(uint64_t)*2);
        return;
    }
    else
    {
        if(byte==0)
        {

            quickSort(*array1,start,end-1);
            return;
        }
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
    //////////////////////////////////////////////////////
    ////////// mono gia medium
    ////////// epeidh argei se kapoia sort
    int sort_flag=0;
    for(int counter=0; counter<=power; counter++)
    {
        if(Hist[counter].count>100000)
        {
            sort_flag=1;
        }
    }
    if(sort_flag==1)
    {
        if(where_to_write==1)
        {
            quickSort(*array0,start,end-1);
            memcpy(array1->tuples+start,array0->tuples,end*sizeof(tuple));
        }
        else
        {
            quickSort(*array1,start,end-1);
        }
        return;
    }
    /////////////////////////////////////////////////

    int index_size=256;
    int ** index;
    index=malloc(sizeof(int*)*index_size);
    for(int counter=0; counter<index_size; counter++)
    {
        index[counter]=malloc(sizeof(int)*Hist[counter].count);
    }
    for(int row=0; row<index_size; row++)
    {
        for(int column=0; column<Hist[row].count; column++)
        {
            index[row][column]=-1;
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


    for(int counter=0; counter<index_size; counter++)
    {
        free(index[counter]);
    }
    free(index);


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
            if(Psum[where_in_array].count==start && Psum[where_in_array+1].count==end && where_to_write==0 && byte!=7)
            {
                return;
            }
            sorting(array0,array1,Psum[where_in_array].count,Psum[where_in_array+1].count,where_to_write,byte-1);
        }



        where_in_array++;
    }
}



Result * Join(column_data R, column_data S,int * num_of_matches )
{
    Result *ResultList = ListInit();
    int i = 0 , j = 0 , matches = 0  ;

    if(R.num_tuples < S.num_tuples)
    {
        while(i < R.num_tuples && j < S.num_tuples)
        {
            if(R.tuples[i].key == S.tuples[j].key)
            {

                matches++;
                InsertResult(R.tuples[i].payload,S.tuples[j].payload,ResultList);
                (*num_of_matches)++;

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
                (*num_of_matches)++;

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
//    PrintResults(ResultList);
//    printf("Number of Joins: %d\n",*num_of_matches);
    free(R.tuples);
    free(S.tuples);
    return ResultList;
}
column_data load_column_data(relation * relations, int rel,int column_id)
{

    uint64_t i=0;
    column_data col;
    col.num_tuples=relations[rel].num_tuples;
    col.tuples=malloc(sizeof(tuple)*col.num_tuples);
    for( i=0; i< col.num_tuples; i++)
    {
        col.tuples[i].payload=i;
        col.tuples[i].key=relations[rel].data[(column_id*col.num_tuples)+i];
    }

    return col;
}

column_data load_from_IR(relation * relations,int rel,int column_id,uint64_t num_rows,uint64_t * row_ids)
{
    uint64_t i=0;
    column_data col;
    col.num_tuples=num_rows;
    col.tuples=malloc(sizeof(tuple)*col.num_tuples);

    for( i=0; i< col.num_tuples; i++)
    {
        col.tuples[i].payload=row_ids[i];
        col.tuples[i].key=relations[rel].data[(column_id*relations[rel].num_tuples)+col.tuples[i].payload];
    }
    return col;

}

Result * scan(column_data col1,column_data col2,int * matches)
{
    Result * List=ListInit();

    int i,j;
    if(col1.num_tuples < col2.num_tuples)
    {
        for(i=0; i<col1.num_tuples; i++)
        {

            if (col1.tuples[i].key == col2.tuples[i].key)
            {
                InsertResult(col1.tuples[i].payload, col2.tuples[i].payload, List);
                (*matches)++;
            }
        }
    }
    else
    {
        for(i=0; i<col2.num_tuples; i++)
        {

            if (col1.tuples[i].key == col2.tuples[i].key)
            {
                InsertResult(col2.tuples[i].payload, col1.tuples[i].payload, List);
                (*matches)++;
            }
        }
    }
    return List;
}

uint64_t * Equalizer(column_data array,int given_num,int given_mode,int *count)
{
    //pairnw ena array kai prepei na to diatreksw olo wste na brw tis times pou einai megaluteres 'h mikroteres 'h ises apo kapoion ari8mo
    //orizw mode1 gia to = , mode 2 gia to > kai mode 3 gia to <
    uint64_t i=0;
    if(given_mode == 1)
    {
        for(i = 0 ; i < array.num_tuples; i++)
        {
            if(array.tuples[i].key == given_num)
            {
                (*count)++;
            }
        }
    }
    if(given_mode == 2)
    {
        for(i=0; i< array.num_tuples; i++)
        {
            if(array.tuples[i].key > given_num)
            {
                (*count)++;
            }
        }
    }
    if ( given_mode ==3)
    {
        for(i =0; i< array.num_tuples; i++)
        {
            if(array.tuples[i].key < given_num)
            {
                (*count)++;
            }
        }
    }
    uint64_t * filter=malloc(sizeof(uint64_t)*(*count));


    if(given_mode == 1)
    {
        int j=0;
        for(i = 0 ; i < array.num_tuples; i++)
        {
            if(array.tuples[i].key == given_num)
            {
                filter[j]=array.tuples[i].payload;
                j++;
            }
        }
    }
    if(given_mode == 2)
    {
        int j=0;
        for(i=0; i< array.num_tuples; i++)
        {
            if(array.tuples[i].key > given_num)
            {
                filter[j]=array.tuples[i].payload;
                j++;
            }
        }
    }
    if ( given_mode ==3)
    {
        int j=0;
        for(i =0; i< array.num_tuples; i++)
        {
            if(array.tuples[i].key < given_num)
            {
                filter[j]=array.tuples[i].payload;
                j++;
            }
        }
    }
    return filter;
}
