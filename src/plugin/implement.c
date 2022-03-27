#include "dpll.h"
#include "formula.h"

#include <stdlib.h>

enum DecideState Decide(const Form* form)
{

    if(getLevel() < form->numVars)
    {
        insertDecisionLevel(getLevel(), FALSE);
        return FOUND_VAR;
    }

    return ALL_ASSIGNED;
}

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

            if(getVarState(lit) != FALSE )
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

    Decision *d;

    while((d = getLastDecision()) && d->flipped)
    {
        removeDecisionLevel();
    }

    if(getLevel() <= 0)
        return false;

    d->value = TRUE;
    setVarState(d->id, TRUE);
    d->flipped = 1;

    return true;
}

