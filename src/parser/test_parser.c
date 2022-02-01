#include "parser.h"

int main()
{

    Form* f = readCNF(fopen("../../inputs/test2.cnf", "r"));


    for(int j = 0; j<f->numClauses; ++j)
    {
        Clause* one = f->clauses[j];
        for(int i =0 ; i < one->size; ++i)
            printf("%d ", one->variables[i]);

        printf("\n");
    }

    freeForm(f);
    return 0;
}
