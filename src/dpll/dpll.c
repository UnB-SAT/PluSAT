#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
    free(decisions);
    free(levels);
}

static int count=0;

VariableId Decide(const Form* form)
{

    if(count < form->numVars)
    {
        return count++;
    }
    else
        return -1;


}

/* Levels structure*/
void insertDecisionLevel(const VariableId var, const int decision)
{
    decisions[var] = decision;

    levels[levelNum].flipped = 0;
    levels[levelNum].id = var;
    levels[levelNum].value = decision;

    levelNum++;
}

Decision getLastDecision()
{
    return levels[levelNum-1];
}

void removeDecisionLevel()
{

    decisions[getLastDecision().id] = UNK;
    levelNum--;
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

       printf("%d\n", clause->size);


       for (int j = 0; j< clause->size; ++j)
       {
           printf("variable ->%d\n", clause->variables[j]);

           int variable = clause->variables[j];

           variable = (variable >= 0)? variable : -variable;

           if (decisions[variable-1] != FALSE)
               flag = true;
       }
       

       // some clause have only variables with false assigned
       if(flag == false)
           return false;
   }

   return true;
}

void dpllRecursive(Form* problem)
{

    VariableId var = Decide(problem);

    Decision decision;

    if(var !=-1){


        insertDecisionLevel(var, TRUE);

        BCP(problem, getLastDecision());
        printf("A var %d value %d\n", var, TRUE);
        dpllRecursive(problem);
        printf("R var %d value %d\n", var, TRUE);
        removeDecisionLevel();

        insertDecisionLevel(var, FALSE);
        BCP(problem, getLastDecision());
        printf("A var %d value %d\n", var, FALSE);
        dpllRecursive(problem);
        printf("R var %d value %d\n", var, FALSE);
        removeDecisionLevel();


    }else{
        printf("good");
    }

}

