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
#include <sys/time.h>
#include "functions.h"
#include "structs.h"
#include "Result.h"
#include "Intermediate_Result.h"

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


    for(int counter=0; counter<256; counter++)
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
            if(Psum[where_in_array].count==start && Psum[where_in_array+1].count==end && where_to_write==0)
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
    //PrintResults(ResultList);
   // printf("Number of Joins: %d\n",*num_of_matches);
    free(R.tuples);
    free(S.tuples);
    return ResultList;
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


void queries_analysis(char * FileToOpen,relation * relations)
{

    int i,j;
    FILE *file;
    if((file = fopen(FileToOpen, "r")) == NULL)
    {

        fprintf(stderr,"Error Opening file\n");
        exit(0);
    }
    size_t len = 0;
    ssize_t flag;
    char* line = NULL;
    while( (flag = getline(&line , &len ,file )) != -1 )
    {

        char *token;
        char **tokens;
        // edw tsekarw an h grammh mou exei megethos megalutero apo ena
        if (strlen(line) < 3)
        {
            continue;
        }

        tokens = malloc(3 * sizeof(char *));
        token = strtok(line, "|");
        tokens[0]=strdup(token);

        token = strtok(NULL, "|");
        tokens[1] =strdup(token);

        token = strtok(NULL, "\n");
        tokens[2] =strdup(token);

        ///debugging printf("%s|%s|%s\n",tokens[0],tokens[1],tokens[2]);


////////////////////////////////////////////////////////////////////////////////////
///////// antoistoixish twn 0,1,2.... pou anaferontai sta predicates stis pragamtikes sxeseis me to mapping

        int relations_to_check=0;
        for(i=0; i<strlen(tokens[0]); i++)
        {
            //metraw ta kena pou exw wste na brw kai to poses sxeseis exw
            if(line[i] ==' ')
            {
                relations_to_check ++;
            }
        }
        relations_to_check++; // ta sunolika relation pou exoume na koitaksoume

        int * mapping=malloc(sizeof(int)*relations_to_check);
        char * re=NULL;
        char * sxeseis=strdup(tokens[0]);
        char * temp=strtok_r(sxeseis," ",&re);
        int id=strtol(temp,NULL,10);
        mapping[0]=id;
        for(i=1; i<relations_to_check; i++)
        {

            temp=strtok_r(re," ",&re);
            id=strtol(temp,NULL,10);
            mapping[i]=id;

        }
        free(sxeseis);


///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//////proteraiothta twn predicates ..poio 8a ginei 1o,2o klp ston pianaka prio
        int total_ques=0;
        for(i=0; i<strlen(tokens[1]); i++)
        {
            if(line[i+strlen(tokens[0])] =='&')
            {
                total_ques ++;
            }
        }
        total_ques++;

        ///debugging printf("Exw sunolika %d sxeseis kai %d queries\n\n",relations_to_check,total_ques);

        char* pre =strdup(tokens[1]);
        struct Predicates* predicates=predicates_analysis(total_ques,pre);
        int *prio=predicates_priority(total_ques,predicates);

        free(pre);

////////////////////////////////////////////////////////////////////////////////////////////


        Intermediate_Result * IR=exec_predicates(relations,predicates,prio,total_ques,relations_to_check,mapping);
        EndiamesiSum(IR,relations,mapping,tokens[2]);
        free(IR->relResults);
        for(int i=0; i<relations_to_check; i++)
        {
            free(IR->resArray[i]);
        }
        free(IR->resArray);
        free(IR);



        free(tokens[0]);
        free(tokens[1]);
        free(tokens[2]);
        free(tokens);
        free(prio);
        free(predicates);
        free(mapping);

    }
    free(line);
    fclose(file);

}



Intermediate_Result * exec_predicates(relation * relations,struct Predicates * predicates,int * prio,int total_ques,int relations_to_check,int * mapping)
{
    Intermediate_Result* IR = create_Intermediate_Result(relations_to_check);


    for(int j= 0; j< total_ques; j++)
    {

        if(predicates[prio[j]].num == -1)//exw join.
        {
            column_data column1,sorted_column1;
            column_data column2,sorted_column2;

            if((IR ->relResults[predicates[prio[j]].relation1] == 0) || (IR->relResults[predicates[prio[j]].relation2] == 0))
            {
                //printf("mpainw edw kai prepei na kanw kati \n");
                continue;
            }

            if((IR->relResults[predicates[prio[j]].relation1]) != -1) //an uparxei sthn endiamesh dinw auto
            {
                column1=load_from_IR(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1,IR->relResults[predicates[prio[j]].relation1],IR->resArray[predicates[prio[j]].relation1]);
//                sorted_column1=Sort(column1);
                quickSort(column1,0,column1.num_tuples-1);

            }

            else
            {
                column1=load_column_data(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1);
//                sorted_column1=Sort(column1);
                quickSort(column1,0,column1.num_tuples-1);

            }


            if((IR->relResults[predicates[prio[j]].relation2]) != -1) //an uparxei sthn endiamesh dinw auto
            {
                column2=load_from_IR(relations,mapping[predicates[prio[j]].relation2],predicates[prio[j]].colum2,IR->relResults[predicates[prio[j]].relation2],IR->resArray[predicates[prio[j]].relation2]);
                quickSort(column2,0,column2.num_tuples-1);

//                sorted_column2=Sort(column2);
            }
            else {
                column2 = load_column_data(relations, mapping[predicates[prio[j]].relation2],predicates[prio[j]].colum2);
                quickSort(column2, 0, column2.num_tuples - 1);
            }


//

            int list_result=0;
            Result *List=Join(column1,column2,&list_result);
            if(list_result==0)
            {
                freelist(List);
                continue;
            }

            if(column1.num_tuples< column2.num_tuples)
            {

                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation1,0,relations_to_check);
                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation2,1,relations_to_check);

            }
            else
            {

                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation2,0,relations_to_check);
                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation1,1,relations_to_check);

            }

            freelist(List);

        }
        else // exw sugkrish me enan ari8mo
        {
            int matches=0;
            uint64_t * filter=NULL;
            column_data column=load_column_data(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1);


            if(predicates[prio[j]].op == '<')
            {
                filter=Equalizer(column,predicates[prio[j]].num,3,&matches);
                IR = FilterUpdate(IR , matches,filter ,predicates[prio[j]].relation1,relations_to_check);

            }
            else if(predicates[prio[j]].op == '=')
            {
                filter=Equalizer(column,predicates[prio[j]].num,1,&matches);
                IR = FilterUpdate(IR , matches,filter ,predicates[prio[j]].relation1,relations_to_check);

            }
            else if( predicates[prio[j]].op == '>')
            {
                filter=Equalizer(column,predicates[prio[j]].num,2,&matches);
                IR = FilterUpdate(IR , matches,filter ,predicates[prio[j]].relation1,relations_to_check);

            }
            //PrintMe(IR,relations_to_check);
           // exit(0);
            free(column.tuples);
            free(filter);
        }

    }
    return IR;


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
    //printf("rel:%d.%d\n\n",rel,column_id);
    //printf("num:%lu\n",num_rows);

    for( i=0; i< col.num_tuples; i++)
    {
        col.tuples[i].payload=row_ids[i];
        //printf("pay:%lu\n",col.tuples[i].payload);
        col.tuples[i].key=relations[rel].data[(column_id*relations[rel].num_tuples)+col.tuples[i].payload];
    }
    return col;

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






struct Predicates *predicates_analysis(int total_preds,char * temp_str )
{
    struct Predicates* predicates=malloc(sizeof(struct Predicates)*total_preds);

    int counter=0,erwthmata=0;

    int i,j;
    int flag = 0;
    int flag1 =0;
    char *rest = NULL;
    char * str=strdup(temp_str);
    char *token = strtok_r(str,"&",&rest);

    for(j=0; j<strlen(temp_str); j++)
    {
        //arxika tha brw thn praksi sugkrisis pou prokeitai na kanw.
        if(temp_str[j] == '<' || temp_str[j] == '>' || temp_str[j]== '=')
        {
            predicates[counter].op = temp_str[j];
            //printf("%c\n",predicates[counter].op);
            counter ++;
            erwthmata ++;
        }
    }
    //Init values to -1
    for(i =0; i<total_preds; i++)
    {
        predicates[i].relation1 =-1;
        predicates[i].relation2 =-1;
        predicates[i].num =-1;
        predicates[i].colum1=-1;
        predicates[i].colum2=-1;
        predicates[i].prio = -1;
    }
    /////////////////////////
    counter =0;
    while (erwthmata > 0)
    {
        for(i =0; i<strlen(token); i++)
        {
            if(token[i] == '=' || token[i] == '>' || token[i] =='<')
            {
                flag = 1;
            }
            if(token[i] == '.' && predicates[counter].op == '=' && flag==0)
            {
                token = strtok(token,".");
                predicates[counter].relation1 =atoi(token);
                token=strtok(NULL,"=");
                predicates[counter].colum1 = atoi(token);

                //                printf("%d.%d %c ",predicates[counter].relation1,predicates[counter].colum1,predicates[counter].op);

                token = strtok(NULL , "&");

                for(j =0; j<strlen(token); j++)
                {
                    if(token[j] == '.')
                    {
                        token = strtok(token,".");
                        predicates[counter].relation2 = atoi(token);
                        token= strtok(NULL,"&");
                        predicates[counter].colum2 = atoi(token);

                        //                      printf("%d.%d \n",predicates[counter].relation2,predicates[counter].colum2);
                        flag1 = 1;
                    }
                }

                if(flag1 == 0)
                {
                    predicates[counter].num = atoi(token);
                    //             printf("%d\n",predicates[counter].num);
                }
                counter ++;
            }
            else if(token[i] == '.' && (predicates[counter].op == '>' || predicates[counter].op == '<') && flag == 0)
            {
                token = strtok(token,".");
                predicates[counter].relation1 = atoi(token);
                if(predicates[counter].op == '>')
                {
                    token = strtok(NULL,">");
                }
                else if(predicates[counter].op == '<')
                {
                    token = strtok(NULL,"<");
                }
                predicates[counter].colum1 = atoi(token);

                //         printf("%d.%d %c " ,predicates[counter].relation1 , predicates[counter].colum1, predicates[counter].op );
                token = strtok(NULL,"&");
                predicates[counter].num = atoi(token);
                //          printf("%d\n", predicates[counter].num);
                counter ++;
            }
            if(flag == 1)
            {
                if(predicates[counter].op == '=')
                {
                    token = strtok(token,"=");
                    predicates[counter].num = atoi(token);
                    token = strtok(NULL,".");
                    predicates[counter].relation1  = atoi(token);
                    token = strtok(NULL,"&");
                    predicates[counter].colum1 = atoi(token);
                    //               printf("%d %c %d.%d\n" , predicates[counter].num , predicates[counter].op, predicates[counter].relation1,predicates[counter].colum1);
                    counter ++;
                    flag = 0;
                }
                else if(predicates[counter].op == '>')
                {
                    token = strtok(token,"<");
                    predicates[counter].num = atoi(token);
                    token = strtok(NULL,".");
                    predicates[counter].relation1  = atoi(token);
                    token = strtok(NULL,"&");
                    predicates[counter].colum1 = atoi(token);
                    //             printf("%d %c %d.%d\n" , predicates[counter].num , predicates[counter].op, predicates[counter].relation1,predicates[counter].colum1);
                    counter ++;
                    flag = 0;
                }
                else if(predicates[counter].op == '<')
                {
                    token = strtok(token,"<");
                    predicates[counter].num = atoi(token);
                    token = strtok(NULL,".");
                    predicates[counter].relation1  = atoi(token);
                    token = strtok(NULL,"&");
                    predicates[counter].colum1 = atoi(token);
                    //           printf("%d %c %d.%d\n" , predicates[counter].num , predicates[counter].op, predicates[counter].relation1,predicates[counter].colum1);
                    counter ++;
                    flag = 0;
                }

            }
        }
        if(erwthmata != 1)
        {
            token = strtok_r(rest ,"&",&rest);
        }
        erwthmata --;
        flag1 = 0;
    }
    free(str);
    return predicates;
}
int * predicates_priority(int total_preds,struct Predicates *predicates)
{
    int i,j;
    int* prio_array = malloc(total_preds*(sizeof(int)));
    int prio=0;
    int here = 0;
    for(i =0; i<total_preds; i++)
    {
        prio_array[i] = -1;
    }

    for(i =0; i<total_preds; i++)
    {
        if(predicates[i].num != -1)
        {
            for(j=0; j<total_preds; j++)
            {
                if(i != j)
                {
                    //edw tsekarw an exw kapoia sugkrisi me ari9mo kai tsekarw an h sxesh epenalambanetai ths dinw proteraiothta
                    if(predicates[i].relation1 == predicates[j].relation1 || predicates[i].relation1 == predicates[j].relation2
                       || predicates[i].relation2 == predicates[j].relation1 || predicates[i].relation2 == predicates[j].relation2)
                    {
                        if(predicates[i].prio == -1)
                        {
                            predicates[i].prio = prio;
                            prio ++;
                            predicates[j].prio = prio;
                            prio++;
                            continue;
                        }
                    }
                }
            }
        }
    }

    if(prio == 0)//den exw oute mia praksi tupou >,< ,=
    {
        for(i =0; i< total_preds; i++)
        {
            for(j=0; j<total_preds; j++)
            {
                if(i != j)
                {
                    //edw tsekarw an exw kapoia sugkrisi me ari9mo kai tsekarw an h sxesh epenalambanetai ths dinw proteraiothta
                    if(predicates[i].relation1 == predicates[j].relation1 || predicates[i].relation1 == predicates[j].relation2
                       || predicates[i].relation2 == predicates[j].relation1 || predicates[i].relation2 == predicates[j].relation2)
                    {
                        if(predicates[i].prio == -1)
                        {
                            predicates[i].prio=prio;
                            prio ++;
                            predicates[j].prio=prio;
                            prio++;
                            continue;
                        }
                    }
                    else
                    {
                        if(predicates[i].prio == -1)
                        {
                            predicates[i].prio =prio;
                            prio++;
                            continue;
                        }
                    }
                }

            }
        }
    }
    for(i =0; i< total_preds; i++)
    {
        if(predicates[i].prio == -1)
        {
            predicates[i].prio = prio;
            prio++;
        }
    }
    for(i = 0; i< total_preds; i++)
    {
        for(j=0; j<total_preds; j++)
        {
            if(predicates[j].prio == i)
            {
                prio_array[i]= j;
            }
        }
    }
    return prio_array;
}

