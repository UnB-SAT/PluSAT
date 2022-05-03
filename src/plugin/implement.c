#include "dpll.h"
#include "formula.h"
#include <stdio.h>
#include <stdlib.h>

void PreProcessing(Form* form){

    /*
    for(int i = 0; i < 2*form->numVars; ++i){
        if(form->literals[i] == NULL){
            if(i%2==0 && form->literals[i+1] != NULL)
                setVarState((int)i/2, TRUE);

            if(i%2!=0 && form->literals[i-1] != NULL)
                setVarState((int)((i+1)/2), TRUE);
        }
    }
    */
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

    printf("false literal %d\n", falseValuedLiteral);


    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        flag = false;
        clause = head->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            printf("Literal %d state %d\n", lit, getLitState(lit));
            if(getLitState(lit) != FALSE)
                flag=true;
        }

        if(!flag){
            printf("Conflito!!!\n");
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

    printf("Retornar %d\n", i);
    return i+1;
}

