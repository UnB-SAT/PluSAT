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

    clause->size = 0;
    clause->variables = NULL;

    // TODO use memcpy here
    //memcpy(clause->variables, variables, numVars*sizeof(LiteralId));
    clause->variables = variables;

    return clause;
}

void freeClause(Clause *clause)
{
    free(clause->variables);
    free(clause);
    clause = NULL;
}


void initLiteralTable()
{
    litTable = malloc(sizeof(LiteralId)*LIT_TABLE_SIZE);

    if(litTable == NULL)
        exit(1);
}

void addClauseOnTable(LiteralId literal, Clause* clause)
{
}

void addClause(Formula* form, Clause* clause)
{
    // add reference in each variable reference
    for(int literalIdx = 0; literalIdx < clause->size; literalIdx++)
    {
        addClauseOnTable(clause->variables[literalIdx], clause);
    }

    // add a clause on a formula
}
