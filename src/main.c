#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{

    char *file = "test.cnf";

    readCNF(fopen(file, "r"));

    return 0;
}
