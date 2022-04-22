#include "dpll.h"
#include "formula.h"
#include <stdio.h>
#include <stdlib.h>

void PreProcessing(Form* form){}

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
    bool flag;
    ClauseNode *head;
    Clause *clause;

    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? decision.id+1 : -decision.id -1);

    head = formula->literals[getPos(falseValuedLiteral)];

    printf("false literal %d\n", falseValuedLiteral);


    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        printf("||||\n");
        flag = false;
        clause = head->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            printf("Literal %d state %d\n", lit, getLitState(lit));
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
    printf("Conflito level: %d\n", getLevel());
    Decision *d = getLastDecision();


    debugDecision();

    while(d!=NULL && d->flipped)
    {
        removeDecisionLevel();
        d = getLastDecision();
    }

    if(getLevel() <= 0)
    {
        printf(">>>>>>>>>>><<<<<<<<<<\n");
        return false;
    }


    d->value = TRUE;
    setVarState(d->id, TRUE);
    d->flipped = 1;

    return true;
}

