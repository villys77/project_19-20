//
// Created by villys on 5/11/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "queries_management.h"
#include "structs.h"
#include "Result.h"
#include "Intermediate_Result.h"
#include "threads.h"
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

            int c=0;
            for(j=0; j<relations[i].num_columns; j++)
            {
                uint64_t z;
                counter=0;
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


void queries_analysis(char * FileToOpen,relation * relations,int rels,struct statistics * original,thread_pool * pool_threads)
{

    uint64_t *all_sums[512];
    uint64_t shows[512];
    int Sums_count=0;
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
    int batch=1;
    while( (flag = getline(&line , &len ,file )) != -1 )
    {

        char *token;
        char **tokens;
        // edw tsekarw an h grammh mou exei megethos megalutero apo ena
        if (strlen(line) < 3)
        {
            thread_pool_barrier(pool_threads);

            for(int i=0; i<Sums_count; i++)
            {
                for(int j=0; j<shows[i]; j++)
                {

                    if(all_sums[i][j]==0)
                    {
                        if(j==shows[i]-1)
                        {
                            printf("NULL");
                        }
                        else
                        {
                            printf("NULL ");
                        }

                    }
                    else
                    {
                        if(j==shows[i]-1)
                        {
                            printf("%lu",all_sums[i][j]);
                        }
                        else
                        {
                            printf("%lu ",all_sums[i][j]);
                        }
                    }
                }
                printf("\n");
            }
            for(int i=0; i<Sums_count; i++)
            {
                if(all_sums[i])
                {

                    free(all_sums[i]);
                }
            }

            Sums_count=0;

            continue;
        }

        args *my_args=malloc(sizeof(args));
        my_args->line=strdup(line);
        my_args->rels=rels;
        my_args->relations=relations;
        my_args->original=original;
        my_args->Sums_count=Sums_count;
        my_args->shows=shows;
        my_args->all_sums=all_sums;
        pthread_mutex_init(&my_args->mutex,NULL);


        thread_pool_add_job(pool_threads,(void *)thread_function,my_args);

        Sums_count++;

    }
    for(int i=0; i<rels; i++)
    {
        free(relations[i].stats.max);
        free(relations[i].stats.min);
        free(relations[i].stats.distinct);
        free(relations[i].stats.number);
        free(original[i].max);
        free(original[i].min);
        free(original[i].distinct);
        free(original[i].number);

        for (int j = 0; j < relations[i].num_columns; j++)
        {
            free(relations[i].stats.dis_vals[j]);
        }
        free(relations[i].stats.dis_vals);
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

           if(IR->Related_Rels[predicates[prio[j]].relation1][predicates[prio[j]].relation2 ]==1 )
           {

               column1=load_from_IR(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1,IR->relResults[predicates[prio[j]].relation1],IR->resArray[predicates[prio[j]].relation1]);
               column2=load_from_IR(relations,mapping[predicates[prio[j]].relation2],predicates[prio[j]].colum2,IR->relResults[predicates[prio[j]].relation2],IR->resArray[predicates[prio[j]].relation2]);
               int matches=0;
               Result * List=scan(column1,column2,&matches);
               if(column1.num_tuples< column2.num_tuples)
               {


                   IR=JoinUpdate(IR,matches,List,predicates[prio[j]].relation1,0,relations_to_check,predicates[prio[j]].relation2);
                   IR=JoinUpdate(IR,matches,List,predicates[prio[j]].relation2,1,relations_to_check,predicates[prio[j]].relation1);

               }
               else
               {

                   IR=JoinUpdate(IR,matches,List,predicates[prio[j]].relation2,0,relations_to_check,predicates[prio[j]].relation1);
                   IR=JoinUpdate(IR,matches,List,predicates[prio[j]].relation1,1,relations_to_check,predicates[prio[j]].relation2);

               }
               freelist(List);
               free(column1.tuples);
               free(column2.tuples);

               continue;

           }
            if((IR->relResults[predicates[prio[j]].relation1]) != -1) //an uparxei sthn endiamesh dinw auto
            {
                column1=load_from_IR(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1,IR->relResults[predicates[prio[j]].relation1],IR->resArray[predicates[prio[j]].relation1]);
                sorted_column1=Sort(column1);
            }
            else
            {
                column1=load_column_data(relations,mapping[predicates[prio[j]].relation1],predicates[prio[j]].colum1);
                sorted_column1=Sort(column1);
            }



            if((IR->relResults[predicates[prio[j]].relation2]) != -1) //an uparxei sthn endiamesh dinw auto
            {
                column2=load_from_IR(relations,mapping[predicates[prio[j]].relation2],predicates[prio[j]].colum2,IR->relResults[predicates[prio[j]].relation2],IR->resArray[predicates[prio[j]].relation2]);
                sorted_column2=Sort(column2);
            }
            else
            {
                column2 = load_column_data(relations, mapping[predicates[prio[j]].relation2],predicates[prio[j]].colum2);
                sorted_column2=Sort(column2);
            }


            int list_result=0;
            Result *List=Join(sorted_column1,sorted_column2,&list_result);
            if(list_result==0)
            {
                IR->relResults[predicates[prio[j]].relation1] = 0;
                IR->relResults[predicates[prio[j]].relation2] = 0;
                freelist(List);
                return IR;
            }

            if(sorted_column1.num_tuples< sorted_column2.num_tuples)
            {

                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation1,0,relations_to_check,predicates[prio[j]].relation2);
                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation2,1,relations_to_check,predicates[prio[j]].relation1);

            }
            else
            {

                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation2,0,relations_to_check,predicates[prio[j]].relation1);
                IR=JoinUpdate(IR,list_result,List,predicates[prio[j]].relation1,1,relations_to_check,predicates[prio[j]].relation2);

            }
            IR->Related_Rels[predicates[prio[j]].relation1][predicates[prio[j]].relation2] = 1;
            IR->Related_Rels[predicates[prio[j]].relation2][predicates[prio[j]].relation1] = 1;

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
            if(matches==0)
            {
                free(column.tuples);
                free(filter);
                break;
            }
            free(column.tuples);
            free(filter);
        }

    }
    return IR;


}




struct Predicates *predicates_analysis(int total_preds,char * temp_str,struct relation * relations,int * mapping)
{
    char * ts=NULL;
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
                token = strtok_r(token,".",&ts);
                predicates[counter].relation1 =atoi(token);
                token=strtok_r(NULL,"=",&ts);
                predicates[counter].colum1 = atoi(token);
                token = strtok_r(NULL , "&",&ts);

                for(j =0; j<strlen(token); j++)
                {
                    if(token[j] == '.')
                    {
                        token = strtok_r(token,".",&ts);
                        predicates[counter].relation2 = atoi(token);
                        token= strtok_r(NULL,"&",&ts);
                        predicates[counter].colum2 = atoi(token);
                        flag1 = 1;
                    }
                }

                if(flag1==1)
                {
                    if(mapping[predicates[counter].relation1]==mapping[predicates[counter].relation2])
                    {
                        if(predicates[counter].colum1!=predicates[counter].colum2) //R.A=R.B
                        {
                            count_statistics(relations,mapping,predicates,counter,4);
                        }

                    }
                }

                if(flag1 == 0)
                {
                    predicates[counter].num = atoi(token);
                    if(predicates[counter].op=='=')
                    {
                        count_statistics(relations,mapping,predicates,counter,1);
                    }

                }
                    counter ++;

            }
            else if(token[i] == '.' && (predicates[counter].op == '>' || predicates[counter].op == '<') && flag == 0)
            {
                token = strtok_r(token,".",&ts);
                predicates[counter].relation1 = atoi(token);
                if(predicates[counter].op == '>')
                {
                    token = strtok_r(NULL,">",&ts);
                }
                else if(predicates[counter].op == '<')
                {
                    token = strtok_r(NULL,"<",&ts);
                }
                predicates[counter].colum1 = atoi(token);
                token = strtok_r(NULL,"&",&ts);
                predicates[counter].num = atoi(token);
                if(predicates[counter].op=='>')
                {
                    count_statistics(relations,mapping,predicates,counter,2);
                }
                else if(predicates[counter].op=='<')
                {

                    count_statistics(relations,mapping,predicates,counter,3);

                }

                counter ++;
            }
            if(flag == 1)
            {
                if(predicates[counter].op == '=')
                {
                    token = strtok_r(token,"=",&ts);
                    predicates[counter].num = atoi(token);
                    token = strtok_r(NULL,".",&ts);
                    predicates[counter].relation1  = atoi(token);
                    token = strtok_r(NULL,"&",&ts);
                    predicates[counter].colum1 = atoi(token);
                    counter ++;
                    flag = 0;

                }
                else if(predicates[counter].op == '>')
                {
                    token = strtok_r(token,"<",&ts);
                    predicates[counter].num = atoi(token);
                    token = strtok_r(NULL,".",&ts);
                    predicates[counter].relation1  = atoi(token);
                    token = strtok_r(NULL,"&",&ts);
                    predicates[counter].colum1 = atoi(token);
                    counter ++;
                    flag = 0;

                }
                else if(predicates[counter].op == '<')
                {
                    token = strtok_r(token,"<",&ts);
                    predicates[counter].num = atoi(token);
                    token = strtok_r(NULL,".",&ts);
                    predicates[counter].relation1  = atoi(token);
                    token = strtok_r(NULL,"&",&ts);
                    predicates[counter].colum1 = atoi(token);
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

void count_statistics(relation * relations,int * mapping,struct Predicates * predicates, int counter,int given_mode  )
{
    ///1 gia filtra me =
    ///2 gia filtra me >
    ///3 gia filtra me <
    ///4 gia join ston idio pinaka-sarwsh
    ///5 gia join

    if(given_mode==1)
    {
        //////sthlh A

        uint64_t max=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t min=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];



        relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=predicates[counter].num;//Ua
        relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=predicates[counter].num;//Ia

        uint64_t prev_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];
        uint64_t prev_num=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];


        int k;
        int dist_size=max-min+1;
        int dis_flag=0;
        for(k=0; k< dist_size; k++)
        {
            if(relations[mapping[predicates[counter].relation1]].stats.dis_vals[predicates[counter].colum1][k]==predicates[counter].num)
            {
                dis_flag=1;
                break;
            }

        }

        if(dis_flag==1)
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=1;
            if(prev_dis !=0)
            {
                relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=prev_num/prev_dis;//Fa
            }

        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=0;
            relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=0;
        }

        uint64_t number=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];


        ///////////////////////

        //////oi upoloipes sthles


        for(k=0; k<relations[mapping[predicates[counter].relation1]].num_columns; k++)
        {
            if(k!=predicates[counter].colum1)
            {
                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[k];
                uint64_t prev_C_number=relations[mapping[predicates[counter].relation1]].stats.number[k];
                relations[mapping[predicates[counter].relation1]].stats.number[k]=number;

                if(prev_num!=0 && prev_C_dis!=0)
                {

                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=(uint64_t)(prev_C_dis*(1-pow((double )1-((double )number/(double )prev_num),(double )prev_C_number/(double )prev_C_dis)));
                }
                else
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=0;
                }
                relations[mapping[predicates[counter].relation1]].stats.number[k]=number;
            }
        }
        ////////

    }
    else if(given_mode==2)
    {
        //////sthlh A
        uint64_t prev_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];
        uint64_t prev_min=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];
        uint64_t prev_max=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t prev_num=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];


        relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=predicates[counter].num;//Ia
        uint64_t min=predicates[counter].num;

        if(prev_max==prev_min)
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=(((prev_max-min )*prev_dis)/(prev_max-prev_min));
            relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=(((prev_max-min )*prev_num)/(prev_max-prev_min));
        }

        uint64_t number=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];

        /////////////

        //////oi upoloipes sthles
        int k;
        for(k=0; k<relations[mapping[predicates[counter].relation1]].num_columns; k++)
        {
            if(k!=predicates[counter].colum1)
            {

                uint64_t prev_C_num=relations[mapping[predicates[counter].relation1]].stats.number[k];
                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[k];
                relations[mapping[predicates[counter].relation1]].stats.number[k]=number;
                if(prev_C_dis!=0 && prev_num!=0)
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]*=(uint64_t)(prev_C_dis*(1-(pow((double )1-((double )number / (double )prev_num),(double )prev_C_num / (double )prev_C_dis))));
                }
                else
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=0;
                }
            }
        }
        ////////
    }
    else if(given_mode==3)
    {
        //////sthlh A
        uint64_t prev_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];
        uint64_t prev_max=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t prev_min=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];
        uint64_t prev_num=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];


        relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=predicates[counter].num;//Ia
        uint64_t max=predicates[counter].num;


        if(prev_max==prev_min)
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=((max-prev_min )*prev_dis)/(prev_max-prev_min);
            relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=(((max-prev_min )*prev_num)/(prev_max-prev_min));

        }

        uint64_t number=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];

        /////////////

        //////oi upoloipes sthles


        int k;
        for(k=0; k<relations[mapping[predicates[counter].relation1]].num_columns; k++)
        {
            if(k!=predicates[counter].colum1)
            {

                uint64_t prev_C_num=relations[mapping[predicates[counter].relation1]].stats.number[k];
                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[k];
                relations[mapping[predicates[counter].relation1]].stats.number[k]=number;

                if(prev_C_dis!=0 && prev_num!=0)
                {

                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]*=(uint64_t)(prev_C_dis*(1-(pow(1-((double )number /(double ) prev_num),(double )prev_C_num /(double ) prev_C_dis))));
                }
                else
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=0;
                }
            }
        }


        ////////

    }
    else if(given_mode==4)
    {
        uint64_t prev_max_rel1=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t prev_min_rel1=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];

        uint64_t prev_max_rel2=relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2];
        uint64_t prev_min_rel2=relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2];

        if(prev_max_rel1<prev_max_rel2)
        {
            relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=prev_max_rel1;
            relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2]=prev_max_rel1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=prev_max_rel2;
            relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2]=prev_max_rel2;
        }

        if(prev_min_rel1>prev_min_rel2)
        {
            relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=prev_min_rel1;
            relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2]=prev_min_rel1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=prev_min_rel2;
            relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2]=prev_min_rel2;
        }

        uint64_t max=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t min=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];

        uint64_t num=max-min+1;

        uint64_t prev_number=relations[mapping[predicates[counter].relation2]].stats.number[predicates[counter].colum2];

        relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=prev_number/num;
        relations[mapping[predicates[counter].relation2]].stats.number[predicates[counter].colum2]=prev_number/num;
        uint64_t number=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];

        uint64_t prev_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];

        if(prev_dis==0)
        {
            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=0;
            relations[mapping[predicates[counter].relation2]].stats.distinct[predicates[counter].colum2]=0;
        }
        else
        {

            relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=(uint64_t)(prev_dis*(1-pow((double )1-((double )number/(double )prev_number),(double )prev_number/(double )prev_dis)));
            relations[mapping[predicates[counter].relation2]].stats.distinct[predicates[counter].colum2]=(uint64_t)(prev_dis*(1-pow((double )1-((double )number/(double )prev_number),(double )prev_number/(double )prev_dis)));

        }
        int k;
        for(k=0; k<relations[mapping[predicates[counter].relation1]].num_columns; k++)
        {
            if(k!=predicates[counter].colum1 && k!=predicates[counter].colum2 )
            {
                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[k];
                uint64_t prev_C_number=relations[mapping[predicates[counter].relation1]].stats.number[k];
                relations[mapping[predicates[counter].relation1]].stats.number[k]=number;

                if(prev_number!=0 && prev_C_dis!=0)
                {

                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=(uint64_t)(prev_C_dis*(1-pow((double )1-((double )number/(double )prev_number),(double )prev_C_number/(double )prev_C_dis)));
                }
                else
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=0;
                }
            }
        }
    }
    else if(given_mode==5)
    {
        uint64_t prev_max_rel1=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t prev_min_rel1=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];

        uint64_t prev_max_rel2=relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2];
        uint64_t prev_min_rel2=relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2];

        if(prev_max_rel1<prev_max_rel2)
        {
            relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=prev_max_rel1;
            relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2]=prev_max_rel1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1]=prev_max_rel2;
            relations[mapping[predicates[counter].relation2]].stats.max[predicates[counter].colum2]=prev_max_rel2;
        }

        if(prev_min_rel1>prev_min_rel2)
        {
            relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=prev_min_rel1;
            relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2]=prev_min_rel1;
        }
        else
        {
            relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1]=prev_min_rel2;
            relations[mapping[predicates[counter].relation2]].stats.min[predicates[counter].colum2]=prev_min_rel2;
        }
        uint64_t max=relations[mapping[predicates[counter].relation1]].stats.max[predicates[counter].colum1];
        uint64_t min=relations[mapping[predicates[counter].relation1]].stats.min[predicates[counter].colum1];


        uint64_t prev_dis_rel1=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];
        uint64_t prev_dis_rel2=relations[mapping[predicates[counter].relation2]].stats.distinct[predicates[counter].colum2];

        uint64_t prev_number_rel1=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];
        uint64_t prev_number_rel2=relations[mapping[predicates[counter].relation2]].stats.number[predicates[counter].colum2];

        uint64_t num=max-min+1;

        relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1]=(prev_number_rel1*prev_number_rel2)/num;
        relations[mapping[predicates[counter].relation2]].stats.number[predicates[counter].colum2]=(prev_number_rel1*prev_number_rel2)/num;

        uint64_t plithos=relations[mapping[predicates[counter].relation1]].stats.number[predicates[counter].colum1];

        relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1]=(prev_dis_rel1*prev_dis_rel2)/num;
        relations[mapping[predicates[counter].relation2]].stats.distinct[predicates[counter].colum2]=(prev_dis_rel1*prev_dis_rel2)/num;

        uint64_t dist=relations[mapping[predicates[counter].relation1]].stats.distinct[predicates[counter].colum1];


        int k;
        for(k=0; k<relations[mapping[predicates[counter].relation1]].num_columns; k++)
        {
            if(k!=predicates[counter].colum1 )
            {
                uint64_t prev_C_number=relations[mapping[predicates[counter].relation1]].stats.number[k];
                relations[mapping[predicates[counter].relation1]].stats.number[k]=plithos;

                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation1]].stats.distinct[k];

                if(prev_dis_rel1!=0 &&prev_C_dis!=0)
                {

                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=(uint64_t)(prev_C_dis*(1-pow((double )1-((double )dist/(double )prev_dis_rel1),(double )prev_C_number/(double )prev_C_dis)));
                }
                else
                {
                    relations[mapping[predicates[counter].relation1]].stats.distinct[k]=0;
                }
            }
        }
        for(k=0; k<relations[mapping[predicates[counter].relation2]].num_columns; k++)
        {
            if( k!=predicates[counter].colum2 )
            {
                uint64_t prev_C_number=relations[mapping[predicates[counter].relation2]].stats.number[k];

                relations[mapping[predicates[counter].relation2]].stats.number[k]=plithos;


                uint64_t prev_C_dis=relations[mapping[predicates[counter].relation2]].stats.distinct[k];

                if(prev_dis_rel2!=0 && prev_C_dis!=0)
                {

                    relations[mapping[predicates[counter].relation2]].stats.distinct[k]=(uint64_t)(prev_C_dis*(1-pow((double )1-((double )dist/(double )prev_dis_rel2),(double )prev_C_number/(double )prev_C_dis)));
                }
                else
                {
                    relations[mapping[predicates[counter].relation2]].stats.distinct[k]=0;
                }

            }
        }

    }

}
int * predicates_priority(int total_preds,struct Predicates *predicates)
{
    int i,j;
    int* prio_array = malloc(total_preds*(sizeof(int)));
    int prio=0;
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

int * predicates_priority_with_statistics(int total_preds,struct Predicates *predicates,int * mapping)
{
    int i,j;
    int* prio_array = malloc(total_preds*(sizeof(int)));
    int prio=0;
    for(i =0; i<total_preds; i++)
    {
        prio_array[i] = -1;
    }

    for(i =0; i<total_preds; i++)
    {
        if(predicates[i].num != -1)
        {
            predicates[i].prio = prio;
            prio ++;
        }
    }

    for(i=0; i<total_preds; i++)
    {
        if(predicates[i].num==-1)
        {
            if(predicates[i].op=='=')
            {
                if(predicates[i].relation1==predicates[i].relation2)
                {
                    predicates[i].prio=prio;
                    prio++;
                }
                else if(mapping[predicates[i].relation1]==mapping[predicates[i].relation2])
                {
                    predicates[i].prio=prio;
                    prio++;
                }
                else
                {
                    //Join Enumeration();
                }
            }
        }
    }

    return prio_array;
}

void reset_statistics(relation * relations,struct statistics * original,char* rels)
{
    char *ts=NULL;

    int i;
    char * rel=strtok_r(rels," ",&ts);
    while(rel)
    {
        int relat=strtol(rel,NULL,10);
        memcpy(relations[relat].stats.min,original[relat].min,relations[relat].num_columns*sizeof(uint64_t));
        memcpy(relations[relat].stats.max,original[relat].max,relations[relat].num_columns*sizeof(uint64_t));
        memcpy(relations[relat].stats.number,original[relat].number,relations[relat].num_columns*sizeof(uint64_t));
        memcpy(relations[relat].stats.distinct,original[relat].distinct,relations[relat].num_columns*sizeof(uint64_t));
        rel=strtok_r(NULL," ",&ts);

    }
}