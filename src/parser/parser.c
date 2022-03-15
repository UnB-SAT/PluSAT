#include <stdio.h>
#include <stdlib.h>
#include "formula.h"
#include "parser.h"

Form* readCNF(FILE *cnf)
{

    // caracter pointed on the file
    char pointer;

    // buffer to get whole lines and free them
    char *line = NULL;
    size_t linesize = 0;

    // Variable and clause numbers
    int V = -1, C= -1;

    // appoint to a new variable
    int literalAux = -1;

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
        else if(pointer == '-' || (pointer >= '1' && pointer <= '9'))
        {

            // seek on caracter before (undo the read operation)
            ungetc((int)pointer, cnf);

            //Read a clause line that finish on 0
            while(fscanf(cnf, "%d", &literalAux) && literalAux != 0)
            {
                literalBuffer[bufferSize++] = literalAux;

                if(bufferSize == bufferMaxSize)
                {
                    bufferMaxSize*=2;
                    literalBuffer = realloc(literalBuffer, sizeof(LiteralId)*bufferMaxSize);
                }

            }

            //Adding this clause into the formula
            Clause *tempClause = newClause(literalBuffer, bufferSize);
            addClause(tempClause, problemF);

            bufferSize=0;
            literalAux = -1;

        }

    }

    free(literalBuffer);

    return problemF;
}
