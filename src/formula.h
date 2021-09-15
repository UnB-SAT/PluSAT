#ifndef __FORMULA__
#define __FORMULA__

#include <stdlib.h>

enum LStates{TRUE=1, FALSE=0 , UNK=2};

typedef int Literal;

typedef struct Clause
{
    int size;
    Literal* variables;
}Clause;

typedef struct Formula
{
    int clauseSize;
    int varSize;

    Literal* variables;
    Clause* clauses;
}Formula;

Formula* initFormula(int numVar, int numClauses);

#endif
