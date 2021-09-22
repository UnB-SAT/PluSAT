#include "parser.h"
#include "dpll.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    printf("%d\n", argc);

    if(argc == 1)
    {
        printf("Don't have a file");
    }

    char *file = argv[1];

    Formula *form;

    form = readCNF(fopen(file, "r"));

    initLevel();
    dpllRecursive(form);

    free(form);

    return 0;
}
