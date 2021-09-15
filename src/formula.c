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
