#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "dpll.h"
#include "discovery.h"

int main(int argc, char **argv)
{

    if(argc == 1)
    {
        printf("Don't have a file");
        exit(1);
    }

    char *file = argv[1];
    loadHooks("build/libimplement.so");

    Hooks *hooks = getHooks();

    Form *form;
    form = readCNF(fopen(file, "r"));

    hooks->preProcessing(form);

    initDecisionLevels(form->numVars);

    enum SolverResult r = dpll(form);

    if(r==SAT){
        printf("Is SAT");
    }else
        printf("Not SAT");

    printf("\n");

    cleanDecisionLevels();
    freeForm(form);

    return (r == SAT)? 1: 0 ;
}
