#include <stdio.h>
#include <stdlib.h>

#include "formula.h"
#include "parser.h"

Form* readCNF(FILE *cnf)
{

    char pointer;
    char *line = NULL;

    size_t linesize = 0;

    int variableAux = -1;
    int V = -1, C= -1;

    int bufferMaxSize = INITIAL_BUFFER_SIZE;
    int bufferSize = 0;
    LiteralId* literalBuffer = malloc(sizeof(LiteralId)*bufferMaxSize);

    Form* problemF = NULL;

    while(fscanf(cnf, " %c", &pointer) == 1)
    {

        //comment: just free the memory and continue
        if(pointer == 'c')
        {
            getline(&line, &linesize, cnf); 
            free(line);
            line = NULL;
        }
        //problem statements
        else if(pointer == 'p')
        {
            fscanf(cnf, " cnf %d %d", &V, &C);

            problemF = newForm(V);
        }
        //clauses
        else if(pointer == '-' || (pointer >= '0' && pointer <= '9'))
        {

            ungetc((int)pointer, cnf);


            while(fscanf(cnf, "%d", &variableAux) && variableAux != 0)
            {

                literalBuffer[bufferSize++] = variableAux;

                if(bufferSize == bufferMaxSize)
                {
                    bufferMaxSize*=2;
                    literalBuffer = realloc(literalBuffer, sizeof(LiteralId)*bufferMaxSize);
                }

            }
            Clause *tempClause = newClause(literalBuffer, bufferSize);

            addClause(problemF, tempClause);

            bufferSize=0;
            variableAux = -1;

        }

    }

    free(literalBuffer);

    return problemF;
}
