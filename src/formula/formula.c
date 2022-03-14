#include <stdlib.h>
#include <string.h>

#include "formula.h"

//CLAUSES

/* Create a new clause and copies variables 
 * argument into clause->variables
 */
Clause* newClause(LiteralId *variables, uint8_t numVars)
{
    Clause* clause = malloc(sizeof(Clause));

    if(clause == NULL)
    {
        exit(1);
    }

    clause->size = numVars;
    clause->literals = malloc(sizeof(LiteralId)*numVars);
    memcpy(clause->literals, variables, numVars*sizeof(LiteralId));

    return clause;
}

/* Non negative literals indexed with 2*literalId
 * Negative literals indexed with 2*literalId + 1
 */
static uint16_t getPos(const LiteralId literal)
{
    return (literal > 0)? 2*literal : -2*literal +1;
}

ClauseNode* addNodeOnList(Clause *clause, ClauseNode *list)
{

    ClauseNode *node = malloc(sizeof(ClauseNode));

    if(node == NULL)
        exit(1);

    node->clause = clause;
    node->next = NULL;

    node->next = list;

    return node;
}

/*Adding clause in the internal Form struct */
void addClause(Clause* clause, Form* form)
{

    form->clauses = addNodeOnList(clause, form->clauses);

    int pos;
    for(LiteralId litPos = 0; litPos < clause->size; litPos++)
    {
        pos = getPos(clause->literals[litPos]);
        form->literals[pos] = addNodeOnList(clause, form->literals[pos]);
    }

    form->numClauses++;

}

void freeClause(Clause *clause)
{
    free(clause->literals);
    free(clause);
}

/*
 * FORM
 */


Form* newForm(uint16_t numVars)
{
    Form* form = malloc(sizeof(Form));

    if(form == NULL)
        exit(1);

    form->clauses = NULL;
    form->numClauses = 0;

    form->numVars = numVars;
    form->literals = malloc(sizeof(ClauseNode*)*2*numVars);

    return form;
}
void freeForm(Form* form)
{

    free(form->literals);

    /*
    for(int i = 0; i<form->numClauses; ++i)
    {
        freeClause(form->clauses[i]);
    }*/

    free(form->clauses);

    free(form);
}


ClauseNode* newNode(Clause* clause)
{
    ClauseNode* r = malloc(sizeof(ClauseNode));

    r->next = NULL;
    r->clause = clause;

    return r;
}


void freeList(ClauseNode *list)
{
    ClauseNode *aux = list;
    ClauseNode *pivot;

    while(aux != NULL)
    {
        pivot = aux;
        aux = aux->next;
        free(pivot);
    }
}

