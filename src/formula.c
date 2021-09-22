#include "formula.h"

Formula* initFormula(int numVar, int numClauses)
{

    Formula *form = (Formula*)malloc(sizeof(Formula));

    form->varSize = numVar;
    form->variables = (Literal*)malloc(sizeof(Literal)*numVar);

    form->clauseSize = numClauses;
    form->clauses = (Clause*)malloc(sizeof(Clause)*numClauses);

    return form;
}

Clause* initClause()
{
    Clause *clause = (Clause*)malloc(sizeof(Clause));

    clause->size = 0;
    clause->capacity = V_INC;
    clause->variables = NULL;

    return clause;
}

void addClause(Formula *formula, Clause *clause)
{

    formula->clauseSize++;
    formula->clauses[formula->clauseSize-1] = *clause;

}

void addVariable(Clause* clause, Literal *variable)
{

    //Init variable vector with V_INC size
    if(clause->variables == NULL)
        clause->variables = (Literal*)malloc(sizeof(Literal)*V_INC);

    //Increase the vector V_INC units
    if(clause->size >= clause->capacity)
    {
        clause->capacity += V_INC;
        clause->variables = (Literal*)realloc(
            clause->variables, 
            clause->capacity
        );
    }


    //adding variable on clause
    clause->size++;
    clause->variables[clause->size - 1] = *variable;
}
