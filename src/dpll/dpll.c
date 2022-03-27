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

enum LiteralStates getLitState(LiteralId lit)
{

    int pos = ((lit > 0)? lit : -lit) -1;
    return decisions[pos];
}

void removeDecisionLevel()
{

    Decision *d = getLastDecision();

    decisions[d->id] = UNK;
    levelNum--;
}

/*
enum DecideState Decide(const Form* form)
{

    if(levelNum < form->numVars)
    {
        insertDecisionLevel(levelNum, FALSE);
        return FOUND_VAR;
    }

    return ALL_ASSIGNED;
}*/

// if some clause have just false assigners then 
// the BCP will return false

/*
bool BCP(Form *formula, const Decision decision)
{

    LiteralId falseValuedLiteral;

    // pick the clause list from 
    // the literal that have a false value
    if(decision.id > 0 && decision.value == TRUE)
        falseValuedLiteral = decision.id + 1;
    else
        falseValuedLiteral = -(decision.id + 1);

 
    bool flag ;
    ClauseNode *head;
    Clause *auxClause;

    head = formula->literals[getPos(falseValuedLiteral)];

    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        flag = false;
        auxClause = head->clause;

        for(int i = 0; i<auxClause->size; ++i)
        {

            // if have just a positive literal than
            // this clause if ok
            //
            LiteralId lit = auxClause->literals[i];

            if(decisions[((lit > 0)? lit : -lit)-1] != FALSE)
                flag=true;
        }

        if(!flag)
            return false;

        head = head->next;
    }

   return true;
}
*/

bool resolveConflict()
{

    Decision *d;

    while((d = getLastDecision()) && d->flipped)
    {
        removeDecisionLevel();
    }

    if(getLevel() <= 0)
        return false;

    d->value = TRUE;
    decisions[d->id] = TRUE;
    d->flipped = 1;

    return true;
}

enum DecideState (*fptr)(const Form*);

enum SolverResult dpll(Form *problem)
{
    enum DecideState dState;

    Hooks* hooks = getHooks();
    DecideHook decide= hooks->decideHook;
    BCPHook bcpH = hooks->bcpHook;


    while(true)
    {

        dState = decide(problem);

        //dState = Decide(problem);

        while(!bcpH(problem, *getLastDecision()))
        {
            if(!resolveConflict())
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

