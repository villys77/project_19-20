//
// Created by villys77 on 2/12/19.
//

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "structs.h"
#include "Result.h"

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

