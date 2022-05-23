#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "dpll.h"
#include "discovery.h"

int main(int argc, char **argv)
{

    if(argc == 1)
    {
        printf("Don't have a file\n");
        exit(5);
    }

    char *file = argv[1];

    char *libplusat = getenv("LIB_PLUSAT");

    if(libplusat == NULL)
    {
        printf("LIB_PLUSAT environment variable not set\n");
        libplusat = "build/libimplement.so";
        printf("Setting value %s\n", libplusat);
    }

    printf("FILE: %s\n", file);


    loadHooks(libplusat);
    Hooks *hooks = getHooks();

    Form *form = readCNF(fopen(file, "r"));

    initDecisionLevels(form->numVars);

    hooks->preProcessing(form);

    enum SolverResult r = dpll(form);

    if(r==SAT)
        printf("Is SAT");
    else
        printf("Not SAT");
    printf("\n");

    cleanDecisionLevels();
    freeForm(form);

    return 0;
    //return (r == SAT)? 1: 0 ;
}
