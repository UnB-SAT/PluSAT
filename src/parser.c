#include "parser.h"
#include "formula.h"

Formula* readCNF(FILE *cnf)
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
            getline(&line, &linesize, cnf); 
            free(line);
            line = NULL;
        }
        //problem statements
        else if(pointer == 'p')
        {
            fscanf(cnf, " cnf %d %d", &V, &C);

            problemF = initFormula(V, C);

        }
        //clauses
        else if(pointer == '-' || (pointer >= '0' && pointer <= '9'))
        {

            ungetc((int)pointer, cnf);

            Clause *tempClause = initClause();

            while(fscanf(cnf, "%d", &variableAux) && variableAux != 0)
            {
                addVariable(tempClause, (Literal*)&variableAux);
            }

            variableAux = -1;

        }

    }

    return problemF;
}
