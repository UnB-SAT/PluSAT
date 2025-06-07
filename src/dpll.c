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

Decision* getDecisions()
{
    return levels;
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
    decisions[(int)var] = state;
}

void removeLastDecision()
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
    DecideHook decide = hooks->decideHook;
    BCPHook bcpH = hooks->bcpHook;
    ResolveConflictHook resolve = hooks->conflictHook;

    int goBack;


    while(true)
    {

        dState = decide(problem);

        while(!bcpH(problem, *getLastDecision()))
        {
            goBack = resolve();

            if(goBack == 0)
                return UNSAT;
            else
                backtrackTo(goBack);
        }

        if(dState == ALL_ASSIGNED)
        {
            printf("v ");
            for(int i = 0; i<problem->numVars; ++i)
            {
                if(decisions[i] == TRUE) printf("%d ",i+1);
                if(decisions[i] == FALSE) printf("-%d ",i+1);
                //if(decisions[i] == UNK) printf("UNK ");
            }
            printf("0\n");
            return SAT;

        }
    }
}


LitState getVarState(const VariableId var)
{
    return decisions[var];
}

ClauseNode* getLiteralClauses(const LiteralId lit, const Form* form)
{
    return form->literals[getPos(lit)];
}

void backtrackTo(uint16_t n)
{
   while(levelNum > n )
   {
       setVarState(getLastDecision()->id, UNK);
       removeLastDecision();
   }

   enum LiteralStates state = levels[levelNum-1].value;

   if(state == TRUE)
   {
       levels[levelNum-1].value = FALSE;
       setVarState(levels[levelNum-1].id, FALSE);
   }else if(state == FALSE)
   {    
       levels[levelNum-1].value = TRUE;
       setVarState(levels[levelNum-1].id, TRUE);
   }

   levels[levelNum-1].flipped = true;
}

