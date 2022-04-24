#include "dpll.h"
#include "discovery.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// global variables
uint16_t levelNum;
Decision* levels;
int* decisions;

int getLevel()
{
    return levelNum;
}


void initDecisionLevels(const int numVars)
{
    levelNum = 0;

    levels = malloc(sizeof(Decision)*numVars);
    decisions = malloc(sizeof(int)*numVars);

    for(int i =0; i<numVars; ++i)
        decisions[i] = UNK;
}
void cleanDecisionLevels()
{
    free(levels);
    free(decisions);
}


/*
 * Aqui a variável e decidida
 * Inserida a decisão no nível
 */



/* Levels structure*/
void insertDecisionLevel(VariableId var, int decision)
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

// Get var position
enum LiteralStates getLitState(LiteralId lit)
{
    int varPos = ((lit > 0)? lit : -lit) -1;

    enum LiteralStates state = decisions[varPos];

    if(state == TRUE && lit < 0)
        state = FALSE;
    else if(state == FALSE && lit < 0)
        state = TRUE;

    return state;
}

void setVarState(VariableId var, enum LiteralStates state)
{ 
    decisions[var] = state;
}

void removeDecisionLevel()
{

    Decision *d = getLastDecision();

    decisions[d->id] = UNK;
    levelNum--;
}

void debugDecision()
{
    printf("-----------\n");
    for(int i = 0; i< levelNum; ++i)
    {
        printf("->%d %d %d\n", levels[i].id, levels[i].value, levels[i].flipped);
    }
    printf("-----------\n");
}

enum SolverResult dpll(Form *problem)
{
    enum DecideState dState;

    Hooks* hooks = getHooks();
    DecideHook decide= hooks->decideHook;
    BCPHook bcpH = hooks->bcpHook;
    ResolveConflictHook resolve = hooks->conflictHook;


    while(true)
    {

        dState = decide(problem);

        while(!bcpH(problem, *getLastDecision()))
        {

            if(!resolve())
                return UNSAT;
        }

        if(dState == ALL_ASSIGNED)
        {
            printf("Model\n");
            for(int i = 0; i<problem->numVars; ++i)
            {
                if(decisions[i] == TRUE) printf("TRUE ");
                if(decisions[i] == FALSE) printf("FALSE ");
                if(decisions[i] == UNK) printf("UNK ");
            }
            printf("\n");
            return SAT;
        }
    }
}


LitState getVarState(const VariableId var)
{
    return decisions[var];
}
