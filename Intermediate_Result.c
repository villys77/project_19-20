//
// Created by villys77 on 2/12/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

Intermediate_Result* create_Intermediate_Result(int numRel)
{

    int  i;

    Intermediate_Result* mid = malloc(sizeof(Intermediate_Result));
    mid->relResults = malloc(numRel*sizeof(uint64_t));  //pinakas me to se poia relations exw vrei matches
    mid->resArray = malloc(numRel*sizeof(uint64_t*));  //pinakas me ta ids twn rels kai ta apotelesmata tous
    mid->Related_Rels=malloc(numRel* sizeof(uint64_t *));

    for(i=0; i<numRel; i++)
    {
        mid->relResults[i] = -1; //arxikopoiw me -1 ton pinaka me ta relations pou exoun apotelesmata
        mid->resArray[i]=NULL;
        mid->Related_Rels[i]=malloc(numRel* sizeof(uint64_t));

    }
    int j;
    for(i=0; i<numRel; i++)
    {
        for(j=0; j<numRel; j++)
        {
            mid->Related_Rels[i][j]=0;
        }
    }

    return mid;
}
void PrintMe(Intermediate_Result* mid,int allRels)
{

    int i, j;
    for (i=0; i<allRels; i++)
    {
        if(mid->relResults[i] != -1)
        {
            printf("rel %d %lu\n",i,mid->relResults[i]);
            for(j=0; j<mid->relResults[i]; j++)
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
            free(mid->Related_Rels[i]);
            free(mid->resArray[i]);
        }
        free(mid->Related_Rels);
        free(mid->resArray);
        free(mid);
        return new_mid;
    }
}

Intermediate_Result* JoinUpdate (Intermediate_Result* mid, int newResults, Result* List ,int rel1, int count, int allRels ,int rel2)
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
                for(j=0; j<allRels; j++)
                {
                    new_mid->Related_Rels[i][j]=mid->Related_Rels[i][j];
                }
                continue;
            }
            else if(mid->relResults[i]!=-1 )
            {

                new_mid->relResults[i] = mid->relResults[i];
                new_mid->resArray[i] = malloc((mid->relResults[i])*sizeof(uint64_t));

                for ( j =0 ; j<mid->relResults[i]; j++)
                {
                    new_mid->resArray[i][j] = mid->resArray[i][j];
                }
                for(j=0; j<allRels; j++)
                {
                    new_mid->Related_Rels[i][j]=mid->Related_Rels[i][j];
                }

            }
        }

        for(i=0; i<allRels; i++)
        {
            if(new_mid->Related_Rels[rel1][i] == 1 && (i!=rel2)) // exw kapoia sxesi pou epireazetai apo ta nea apotelesmata
            {
                int counter = 0,tmp=0;
                new_mid->relResults[i]=new_mid->relResults[rel1];
                if(new_mid->resArray[i])
                {
                    free(new_mid->resArray[i]);
                }
                new_mid->resArray[i]=malloc(newResults* sizeof(uint64_t));
                int temp = 0,r;
                for(j = 0; j < new_mid->relResults[rel1]; j++) //auta tou j einai ta kainourgia mou results
                {
                    for(int k=counter; k<mid->relResults[rel1]; k++)
                    {
                        if(new_mid->resArray[rel1][j] == mid->resArray[rel1][k])
                        {
                            new_mid->resArray[i][j] = mid->resArray[i][k];
                            temp++;
                            if(j==new_mid->relResults[rel1]-1) break;
                            if(k==mid->relResults[rel1]-1)
                            {
                                if (temp == 1) //auto einai otan to teleutaio mou k bgazei join alla to j den exei teleiwsei. opote prepei se ola ta alla j na balw auto mesa
                                {
                                    for(r = j+1 ; r <new_mid->relResults[rel1]; r++)
                                    {
                                        new_mid->resArray[i][r]=mid->resArray[i][k];
                                    }
                                }
                                else counter = k - (temp);
                                break;
                            }


                            if((new_mid->resArray[rel1][j] == new_mid->resArray[rel1][j+1]) && (mid->resArray[rel1][k] == mid->resArray[rel1][k+1]))
                            {
                                counter=k+1; // paw sto epomeno.
                                break;
                            }
                            else if((new_mid->resArray[rel1][j] == new_mid->resArray[rel1][j+1]) && (mid->resArray[rel1][k] != mid->resArray[rel1][k+1]))
                            {
//                                printf("temp = %d and total rels %lu\n",temp,mid->relResults[rel1]);
                                counter = k-(temp-1); // gurnaw pisw toses theseis oses kai oi fores pou uparxei i idia timh mesa sto k
                                temp=0; //midenizw to k pali giati thelw na ksanakanw to idio sthn periptwsh pou exw thn idia katastash.
                                break;
                            }
                            else if(new_mid->resArray[rel1][j] != new_mid->resArray[rel1][j+1]) // otan allazw to j mou prepei to temp na einai 0
                            {
                                temp=0;
                                break;
                            }
                        }
                    }

                }

            }
        }




        free(mid->relResults);
        for(int i=0; i<allRels; i++)
        {
            free(mid->Related_Rels[i]);
            free(mid->resArray[i]);
        }
        free(mid->Related_Rels);
        free(mid->resArray);
        free(mid);
        return new_mid;
    }

}

uint64_t * Intermediate_Sum(Intermediate_Result* mid,relation* relations,int *mapping, char* select,int shows)
{
    uint64_t * sums=malloc(sizeof(uint64_t)*shows);
    int j=0;
    char* re;
    int rel, col;
    uint64_t sum = 0;
    char* tok;
    tok = strtok_r(select, " ",&re);
    while(tok!=NULL)
    {
        sscanf(tok, "%d.%d", &rel, &col);

        if(mid->relResults[rel]==-1 ||mid->relResults[rel]==0)
        {
            for(int i=0; i<shows; i++ )
            {
                sums[i]=0;
            }
            return sums;
        }


        for(int i=0; i<mid->relResults[rel]; i++)
        {

            sum+=relations[mapping[rel]].data[(relations[mapping[rel]].num_tuples*col)+mid->resArray[rel][i]];
        }
        sums[j]=sum;
        j++;
        tok = strtok_r(re, " ",&re);
        sum =0;
    }
    return sums;
}

