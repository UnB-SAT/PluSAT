#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void testAllocationVarTree()
{
    LiteralId values[] = {1, 2, 3};

    Clause* clause1 = newClause(values, 3);

    ClauseNode *list = NULL;

    list = addNodeOnList(clause1, list);

    freeList(list);
}

void testAllocationClause()
{
    int size = 3;
    LiteralId *var = (LiteralId*)malloc(sizeof(LiteralId)*size);

    for(int i = 0; i<size; ++i)
        var[i] = 1;

    Clause* clause = newClause(var, size);
    freeClause(clause);
    free(var);

}

void testCreateForm()
{
    LiteralId values[] = {1, 2, 3};

    Form* form = newForm(3);
    Clause* clause1 = newClause(values, 3);
    Clause* clause2 = newClause(values, 3);
    Clause* clause3 = newClause(values, 3);
    Clause* clause4 = newClause(values, 3);

    
    addClause(clause1, form);
    addClause(clause2, form);
    addClause(clause3, form);
    addClause(clause4, form);

    freeForm(form);
}

int main()
{

    printf(".");
    testAllocationClause();

    printf(".");
    testCreateForm();

    printf(".");
    testAllocationVarTree();


    printf("\n");

    return 0;
}
