#include "parser.h"
#include "dpll.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

    if(argc == 1)
    {
        printf("Don't have a file");
        exit(1);
    }

    char *file = argv[1];

    Form *form;

    form = readCNF(fopen(file, "r"));

    //dpllRecursive(form);
    //
    freeForm(form);

    return 0;
}
