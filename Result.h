//
// Created by villys77 on 2/12/19.
//

#ifndef PROJECT_JOIN_19_20_RESULT_H
#define PROJECT_JOIN_19_20_RESULT_H

#include "structs.h"

Result* ListInit();
void InsertResult(uint64_t,uint64_t,Result*);
void PrintResults(Result*);
void freelist(Result*);

#endif //PROJECT_JOIN_19_20_RESULT_H
