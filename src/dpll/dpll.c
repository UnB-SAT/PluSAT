#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// global variables

uint16_t levelNum = 0;
static Decision* levels;
static int* decisions;

void initDecisionLevels(const int numVars)
{
    levelNum = 0;
    levels = malloc(sizeof(Decision)*numVars);
    decisions = malloc(sizeof(int)*numVars);
}

void cleanDecisionLevels()
{
    free(levels);
    free(decisions);
}

static int count=0;

/*
 * Aqui a variável e decidida
 * Inserida a decisão no nível
 */



/* Levels structure*/
void insertDecisionLevel(const VariableId var, const int decision)
{
    decisions[var] = decision;

    levels[levelNum].flipped = 0;
    levels[levelNum].id = var;
    levels[levelNum].value = decision;

    levelNum++;
}

Decision* getLastDecision()
{
    if(levelNum > 0)
        return &levels[levelNum-1];
    else 
        return NULL;
}

void removeDecisionLevel()
{

    Decision *d = getLastDecision();
    decisions[d->id] = UNK;
    levelNum--;
}

enum DecideState Decide(const Form* form)
{

    if(levelNum < form->numVars)
    {
        insertDecisionLevel(levelNum, FALSE);

        return FOUND_VAR;
    }

    return ALL_ASSIGNED;
}


// if some clause have just false assigners then 
// the BCP will return false
bool BCP(Form *formula, const Decision decision)
{

   bool flag;
   Clause *clause;

   for (int i = 0; i<formula->numClauses; ++i)
   {
       clause = formula->clauses[i];

       flag = false;

       for (int j = 0; j< clause->size; ++j)
       {

           int variable = clause->literals[j];

           variable = (variable >= 0)? variable : -variable;

           if (decisions[variable-1] != FALSE)
               flag = true;
       }

       // some clause have only variables with false assigned
       if(flag == false)
       {
           return false;
       }
   }

   return true;
}

bool resolveConflict()
{

    Decision *d;

    while((d = getLastDecision()) && d->flipped)
    {
        removeDecisionLevel();
    }

    if(levelNum <= 0)
        return false;

    d->value = TRUE;
    decisions[d->id] = TRUE;
    d->flipped = 1;

    return true;
}

enum SolverResult dpll(Form *problem)
{

    enum DecideState dState;

    while(true)
    {


        dState = Decide(problem);

        if(dState == ALL_ASSIGNED)
        {
            for(int i = 0; i<problem->numVars; ++i)
                printf("%d ", decisions[i]);
            printf("\n");
            return SAT;
        }

        while(!BCP(problem, *getLastDecision()))
        {
            if(!resolveConflict())
                return UNSAT;

                }
    }
}

