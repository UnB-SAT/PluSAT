#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void testAllocationVarTree()
{
    LiteralId values[] = {1, 2, 3};

    Clause* clause1 = newClause(values, 3);

    Node *list = NULL;

    list = addOnList(newNode(clause1), list);

    freeList(list);
    freeClause(clause1);
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

    addClause(form, clause1);
    addClause(form, clause2);
    addClause(form, clause3);
    addClause(form, clause4);

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
