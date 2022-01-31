#include "formula.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>


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

Form* newForm()
{
    Form* form = malloc(sizeof(Form));

    if(form == NULL)
        exit(1);

    form->clauses = NULL;
    form->numClauses = 0;

    return form;
}

void freeForm(Form* form)
{
    for(int i = 0; i<form->numClauses; ++i)
    {
        freeClause(form->clauses[i]);
    }

    free(form->clauses);
    free(form);
}

void addClause(Form* form, Clause* clause)
{

    form->numClauses++;
    form->clauses = realloc(form->clauses, form->numClauses*sizeof(Clause*));
    form->clauses[form->numClauses-1] = clause;
}
