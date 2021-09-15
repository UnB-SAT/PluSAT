#include "parser.h"
#include "formula.h"

void readCNF(FILE *cnf)
{

    Formula *problemF;

    char pointer;
    char *line = NULL;

    size_t linesize = 0;

    int variableAux = -1;
    int V = -1, C= -1;

    while(fscanf(cnf, " %c", &pointer) == 1)
    {

        //comment: just free the memory and continue
        if(pointer == 'c')
        {
            printf("Removin a comment\n");
            getline(&line, &linesize, cnf);
            free(line);
            line = NULL;
        }
        //problem statements
        else if(pointer == 'p')
        {
            fscanf(cnf, " cnf %d %d", &V, &C);

            problemF = initFormula(V, C);

            printf("Vertices %d\n", V);
            printf("Clauses %d\n", C);

        }
        //clauses
        else if(pointer == '-' || (pointer >= '0' && pointer <= '9'))
        {

            ungetc((int)pointer, cnf);

            while(fscanf(cnf, "%d", &variableAux) && variableAux != 0)
            {
                printf("%d\n", variableAux);
            }

            variableAux = -1;

        }

    }

}
