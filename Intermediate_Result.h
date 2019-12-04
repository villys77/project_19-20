//
// Created by villys77 on 2/12/19.
//

#ifndef PROJECT_JOIN_19_20_INTERMEDIATE_RESULT_H
#define PROJECT_JOIN_19_20_INTERMEDIATE_RESULT_H

#include "structs.h"

Intermediate_Result* create_Intermediate_Result(int numRel);
void PrintMe(Intermediate_Result* mid,int allRels,int total_matches);
Intermediate_Result* FilterUpdate (Intermediate_Result* mid, int newResults ,int *filter, int rel,int allRels);
Intermediate_Result* JoinUpdate (Intermediate_Result* mid, int newResults, Result* List ,int rel1, int count, int allRels );
void EndiamesiSum(Intermediate_Result* mid, char* select);

#endif //PROJECT_JOIN_19_20_INTERMEDIATE_RESULT_H
