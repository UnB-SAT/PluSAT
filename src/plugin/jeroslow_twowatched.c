#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

double *scores = NULL;

void PreProcessing(Form *form)
{
    scores = (double *)malloc(sizeof(double) * (form->numVars * 2));

    ClauseNode *list = form->clauses;
    while (list != NULL)
    {
        Clause *clause = list->clause;
        int clause_size = clause->size;
        double score = 1.0 / (1 << clause_size);

        for (int i = 0; i < clause_size; ++i)
        {
            LiteralId lit = clause->literals[i];
            int var_index = abs(lit) - 1;

            if (lit > 0)
            {
                scores[var_index] += score;
            }
            else
            {
                scores[var_index + form->numVars] += score;
            }
        }
        list = list->next;
    }
}

enum DecideState Decide(const Form *form)
{

    double max_score = -1.0;
    int best_var = -1;
    bool best_value = FALSE;
    int num_vars = form->numVars;

    for (int i = 0; i < num_vars; ++i)
    {
        if (getVarState(i) != UNK)
            continue;

        double pos_score = scores[i];
        double neg_score = scores[i + num_vars];

        if (pos_score > max_score)
        {
            max_score = pos_score;
            best_var = i;
            best_value = TRUE;
        }
        if (neg_score > max_score)
        {
            max_score = neg_score;
            best_var = i;
            best_value = FALSE;
        }
    }

    if (best_var != -1)
    {
        insertDecisionLevel(best_var, best_value);
        return FOUND_VAR;
    }

    return ALL_ASSIGNED;
}

bool BCP(Form *formula, const Decision decision)
{
    bool flag;
    ClauseNode *head;
    Clause *clause;

    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? decision.id + 1 : -decision.id - 1);

    head = formula->literals[getPos(falseValuedLiteral)];

    while (head != NULL)
    {
        flag = false;
        clause = head->clause;

        for (int i = 0; i < clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            if (getLitState(lit) != FALSE)
                flag = true;
        }

        if (!flag)
        {
            return false;
        }

        head = head->next;
    }

    return true;
}

int resolveConflict()
{
    Decision *decisions = getDecisions();

    int i = getLevel() - 1;

    for (; i >= 0; --i)
        if (decisions[i].flipped == false)
            break;

    return i + 1;
}