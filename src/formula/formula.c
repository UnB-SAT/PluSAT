#include <stdlib.h>
#include <string.h>

#include "formula.h"

/*
 * CLAUSES
 */

/*Adding clause in the internal Form struct */
void addClause(Form* form, Clause* clause)
{

    form->numClauses++;
    form->clauses = realloc(form->clauses, form->numClauses*sizeof(Clause*));
    form->clauses[form->numClauses-1] = clause;

    int varId, litId;

    for(int i = 0; i<clause->size; ++i)
    {

        litId = clause->variables[i];

        varId = (litId < 0)? -litId : litId;


        if(litId < 0)
        {
            form->variables[varId - 1].falseLitClauses = addOnList(newNode(clause), form->variables[varId - 1].falseLitClauses);
        }
        else
        {
            form->variables[varId - 1].trueLitClauses = addOnList(newNode(clause), form->variables[varId - 1].trueLitClauses);
        }

    }
}

Clause* newClause(LiteralId *variables, uint8_t numVars)
{
    Clause* clause = malloc(sizeof(Clause));

    if(clause == NULL)
    {
        exit(1);
    }

    clause->size = numVars;
    clause->variables = NULL;

    clause->variables = malloc(sizeof(LiteralId)*numVars);
    memcpy(clause->variables, variables, numVars*sizeof(LiteralId));

    return clause;
}

void freeClause(Clause *clause)
{
    free(clause->variables);
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
    form->variables = malloc(sizeof(VariableTree)*numVars);
    memset(form->variables, 0, sizeof(VariableTree)*numVars);

    return form;
}
void freeForm(Form* form)
{

    for(int i = 0; i<form->numVars; ++i)
    {

        freeList(form->variables[i].falseLitClauses);
        freeList(form->variables[i].trueLitClauses);
    }

    free(form->variables);

    for(int i = 0; i<form->numClauses; ++i)
    {
        freeClause(form->clauses[i]);
    }

    free(form->clauses);

    free(form);
}


// Variable Tree
Node *addOnList(Node *node, Node* list)
{

    if(list!=NULL)
    {
        node->next=list;
    }

    return node;
}

Node* newNode(Clause* clause)
{
    Node* r = malloc(sizeof(Node));

    r->next = NULL;
    r->clause = clause;

    return r;
}


void freeList(Node *list)
{
    Node *aux = list;
    Node *pivot;

    while(aux != NULL)
    {
        pivot = aux;
        aux = aux->next;
        free(pivot);
    }
}

