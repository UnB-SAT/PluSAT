#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void testAllocationClause()
{
    int size = 3;
    LiteralId *var = (LiteralId*)malloc(sizeof(LiteralId)*size);

    for(int i = 0; i<size; ++i)
        var[i] = 1;

    Clause* clause = newClause(var, size);
    freeClause(clause);

}

void testAllocateLitTable()
{

    LiteralId *table = NULL;

    initLiteralTable(table);

}

int main()
{

    printf(".");
    testAllocationClause();

    printf(".");
    testAllocateLitTable();

    printf("\n");

    return 0;
}
