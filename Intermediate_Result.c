//
// Created by villys77 on 2/12/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

Intermediate_Result* create_Intermediate_Result(int numRel)
{
    printf("from vreate %d\n",numRel);
    int  i;

    Intermediate_Result* mid = malloc(sizeof(Intermediate_Result));
    mid->relResults = malloc(numRel*sizeof(uint64_t));  //pinakas me to se poia relations exw vrei matches
    mid->resArray = malloc(numRel*sizeof(uint64_t*));  //pinakas me ta ids twn rels kai ta apotelesmata tous

    for(i=0; i<numRel; i++)
    {
        mid->relResults[i] = -1; //arxikopoiw me -1 ton pinaka me ta relations pou exoun apotelesmata
        mid->resArray[i]=NULL;
    }

    return mid;
}
void PrintMe(Intermediate_Result* mid,int allRels)
{
    for (int i=0; i<allRels; i++)
    {
        printf("\nrel %d\n",i);
        if(mid->relResults[i] != -1)
        {
            for(int  j=0; j<mid->relResults[i]; j++)
            {
               printf("%lu\n",mid->resArray[i][j]);
            }
        }
    }
}

Intermediate_Result* FilterUpdate (Intermediate_Result* mid, int newResults ,uint64_t * filter, int rel,int allRels)
{
    if(mid->relResults[rel] == -1)
    { //edw shmainei oti den exei uparxoun alla endiamesa apotelesmata gia auth th sxesh
    //ara gia kathe mia thesi tou resArray[rel][i] bazw ena apo ti lista.Prosexw h lista na exei na dwsei stoixeia omws
        mid->relResults[rel]= newResults;
        mid->resArray[rel] = malloc(newResults*sizeof(uint64_t));
        for(int i=0; i<newResults; i++)
        {
            mid->resArray[rel][i]=filter[i];
        }
        //memcpy(mid->resArray[rel],filter,newResults);
        return mid;
    }
    else //auto shmainei oti h sxesh auth exei hdh grammena stoixeia mesa sti endiamesh domi opote prepei
  //annanewthoun kai ola ta alla apo tis alles sxeseis na meinoun opws einai.
    {
        Intermediate_Result *new_mid = create_Intermediate_Result(allRels);
        //twra exw ftiaksei mia arxika kenh endiamesh domh kai prepei na thn gemisw me ta katallhla
        //arxika bazw auta pou einai kainourgia
        new_mid ->relResults[rel] = newResults;
        new_mid->resArray[rel] = malloc(newResults*sizeof(uint64_t));
        for(int i=0; i<newResults; i++)
        {
            mid->resArray[rel][i]=filter[i];
        }

        for(int i = 0; i < allRels; i++) // antigrafw ola ta stoixeia pou eixa hdh dimiourghsei.
        {
            if (i==rel)
            {
                continue;

            }
            else if(mid->relResults[i] != -1)
            {
                new_mid->relResults[i] = mid->relResults[i];
                new_mid->resArray[i] = malloc(new_mid->relResults[i]*sizeof(uint64_t));
                for (int j =0 ; j<new_mid->relResults[i]; j++)
                {
                    new_mid->resArray[i][j] = mid->resArray[i][j];
                }
            }
        }
        free(mid->relResults);
        for(int i=0; i<allRels; i++)
        {
            free(mid->resArray[i]);
        }
        free(mid->resArray);
        free(mid);
        return new_mid;
    }
}

Intermediate_Result* JoinUpdate (Intermediate_Result* mid, int newResults, Result* List ,int rel1, int count, int allRels )
{
    int i , j ,c;
    ResultNode* tmp = List->first;
    c =0;
    ///////////gia to prwto relation einai auto
    if(mid->relResults[rel1] == -1)
    {

        mid->relResults[rel1]= newResults;
        mid->resArray[rel1] = malloc(newResults*sizeof(uint64_t));
        while (tmp != NULL)
        {
            for(j = 0; j<tmp->counter; j++ )
            {
                mid->resArray[rel1][c] = tmp->buffer[j][count];
                c++;
            }
            tmp = tmp->next;
        }
        return mid;
    }
    else
    {
        //printf("mphkaa edwww \n\n");
        int flag=0;
        Intermediate_Result *new_mid = create_Intermediate_Result(allRels);
        //twra exw ftiaksei mia arxika kenh endiamesh domh kai prepei na thn gemisw me ta katallhla
        //arxika bazw auta pou einai kainourgia
        new_mid->relResults[rel1] = newResults;
        new_mid->resArray[rel1] = malloc(newResults*sizeof(uint64_t));

        while(tmp!=NULL)//prepei na grapsw sthn thesi mou ta nea stoixeia
        {
            for(j = 0; j<tmp->counter; j++)
            {
                new_mid->resArray[rel1][c] = tmp->buffer[j][count];
                c++;
             }
            tmp = tmp->next;
        }
        for(i = 0; i < allRels; i++) // antigrafw ola ta stoixeia pou eixa hdh dimiourghsei.
        {
            if (i==rel1)
            {
                continue;
            }
            else if(mid->relResults[i]!=-1)
            {

                new_mid->relResults[i] = mid->relResults[i];
                new_mid->resArray[i] = malloc((mid->relResults[i])*sizeof(uint64_t));

                for ( j =0 ; j<mid->relResults[i]; j++)
                {
                    new_mid->resArray[i][j] = mid->resArray[i][j];
                }
            }
        }
        free(mid);
        return new_mid;
    }

}

void EndiamesiSum(Intermediate_Result* mid,relation* relations,int *mapping, char* select)
{
    int i, j;
    char* re;
    int rel, col;
    uint64_t sum = 0;
    char* tok;
    tok = strtok_r(select, " ",&re);
    while(tok!=NULL)
    {
        sscanf(tok, "%d.%d", &rel, &col);

        printf("%d %d %lu\n",mapping[rel],rel,relations[mapping[rel]].num_tuples*relations[mapping[rel]].num_columns);
        for(i=0; i<mid->relResults[rel]; i++)
        {
            printf("aaa\n");
            printf("%d %lu %lu\n",rel,mid->relResults[rel],mid->resArray[rel][i]);
            sum+=relations[mapping[rel]].data[(relations[mapping[rel]].num_tuples*col)+mid->resArray[rel][i]];
        }
        printf("%lu  ", sum);
        tok = strtok_r(re, " ",&re);
        sum =0;
    }
    printf("\n");
}

