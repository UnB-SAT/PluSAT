#include <stdio.h>
#include <stdlib.h>

#include "dpll.h"
#include "formula.h"

void PreProcessing(Form* form){
    ClauseNode* pivot, *head=form->clauses;

    while(head != NULL)
    {
        pivot = head;

        for(int j = 0; j < pivot->clause->size; ++j)
        {
            printf("%d ", pivot->clause->literals[j]);
        }

        head = head->next;
        printf("\n");
    }
}

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



    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        flag = false;
        clause = head->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            if(getLitState(lit) != FALSE)
                flag=true;
        }

        if(!flag){
            return false;
        }

        head = head->next;
   }

   return true;
}

int resolveConflict()
{
    Decision* decisions =  getDecisions();

    int i = getLevel()-1;

    for(; i>=0; --i)
        if(decisions[i].flipped == false)
            break;

    return i+1;
}

