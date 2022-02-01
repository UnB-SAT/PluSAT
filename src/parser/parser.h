#ifndef __PARSER__
#define __PARSER__

#include "formula.h"
#include <stdio.h>

#define INITIAL_BUFFER_SIZE 64

Form* readCNF(FILE *cnf);

#endif
