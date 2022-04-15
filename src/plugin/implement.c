#include "dpll.h"
#include "formula.h"
#include <stdio.h>

#include <stdlib.h>

enum DecideState Decide(const Form* form)
{

    ClauseNode* list = form->clauses;
    while(list != NULL)
    {

        Clause *clause = list->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];
            lit = ((lit > 0 )? lit : -lit);
            if(getVarState(lit-1) == UNK)
            {
                insertDecisionLevel(lit-1, FALSE);

                return FOUND_VAR;
            }
        }

        list = list->next;
    }

    return ALL_ASSIGNED;
}

bool BCP(Form *formula, const Decision decision)
{

    LiteralId falseValuedLiteral = -((decision.value) ? decision.id+1 : -decision.id -1);
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
            LiteralId lit = auxClause->literals[i];

            if(getLitState(lit) != FALSE)
                flag=true;
        }

        if(!flag)
            return false;

        head = head->next;
   }

   return true;
}

bool resolveConflict()
{
    Decision *d = getLastDecision();

    while(d!=NULL && d->flipped)
    {
        removeDecisionLevel();
        d = getLastDecision();
    }

    if(getLevel() <= 0)
        return false;


    d->value = TRUE;
    setVarState(d->id, TRUE);
    d->flipped = 1;

    return true;
}

