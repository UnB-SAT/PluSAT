#ifndef __FORMULA__
#define __FORMULA__

#include <stdlib.h>
#define V_INC 10

enum LStates{TRUE=1, FALSE=0 , UNK=2};

typedef int Literal;

typedef struct Clause
{
    int size;
    int capacity;
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

void addClause(Formula* formula, Clause *clause);
void addVariable(Clause* clause, Literal *variable);
Clause* initClause();


#endif
