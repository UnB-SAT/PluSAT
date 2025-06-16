#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct VarScore {
    int var_index;
    double score;
    bool best_value;
} VarScore;

double *scores = NULL;
VarScore *sorted_vars = NULL;

int compareVarScore(const void *a, const void *b)
{
    const VarScore *va = (const VarScore *)a;
    const VarScore *vb = (const VarScore *)b;

    if (va->score < vb->score)
        return 1;
    else if (va->score > vb->score)
        return -1;
    else
        return 0;
}

void PreProcessing(Form *form)
{
    int num_vars = form->numVars;
    scores = (double *)malloc(sizeof(double) * (num_vars * 2));
    sorted_vars = (VarScore *)malloc(sizeof(VarScore) * num_vars);

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
                scores[var_index + num_vars] += score;
            }
        }
        list = list->next;
    }

    for (int i = 0; i < num_vars; ++i)
    {
        double pos_score = scores[i];
        double neg_score = scores[i + num_vars];

        sorted_vars[i].var_index = i;
        if (pos_score >= neg_score)
        {
            sorted_vars[i].score = pos_score;
            sorted_vars[i].best_value = TRUE;
        }
        else
        {
            sorted_vars[i].score = neg_score;
            sorted_vars[i].best_value = FALSE;
        }
    }

    qsort(sorted_vars, form->numVars, sizeof(VarScore), compareVarScore);
}

enum DecideState Decide(const Form *form)
{
    int num_vars = form->numVars;

    for (int i = 0; i < num_vars; ++i)
    {
        int var = sorted_vars[i].var_index;
        if (getVarState(var) == UNK)
        {
            insertDecisionLevel(var, sorted_vars[i].best_value);
            return FOUND_VAR;
        }
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