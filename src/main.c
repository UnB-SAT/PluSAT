#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "parser.h"
#include "dpll.h"
#include "discovery.h"

int main(int argc, char **argv)
{
    clock_t inicio, fim;
    double tempo_total, tempo_parser,tempo_solve;
    inicio = clock();

    if(argc == 1)
    {
        printf("c Don't have a file\n");
        exit(5);
    }

    const char *file = argv[1];

    char *libplusat = getenv("LIB_PLUSAT");

    if(libplusat == NULL)
    {
        printf("c LIB_PLUSAT environment variable not set\n");
        libplusat = "build/libimplement.so";
        printf("c Setting value %s\n", libplusat);
    }

    printf("c FILE: %s\n", file);


    loadHooks(libplusat);
    Hooks *hooks = getHooks();

    Form *form = readCNF(fopen(file, "r"));
    fim = clock();
    tempo_parser = (double)(fim - inicio) / CLOCKS_PER_SEC;
    initDecisionLevels(form->numVars);

    hooks->preProcessing(form);

    enum SolverResult r = dpll(form);
    fim = clock();
    tempo_solve = (double)(fim - inicio) / CLOCKS_PER_SEC;
    int exit = 0;

    if(r==SAT){
        printf("s SATISFIABLE");
        exit = 10;
    }else{
        printf("s UNSATISFIABLE");
        exit = 20;
    }
    printf("\n");

    cleanDecisionLevels();
    freeForm(form);
  
    fim = clock();

    tempo_total = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("c Time: %.3fs (Parser:%.3fs Solving:%.3fs)\n",tempo_total,tempo_parser,tempo_solve); 
 
    return exit;
    //return (r == SAT)? 1: 0 ;
}
