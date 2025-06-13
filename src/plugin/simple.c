#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

double *scores = NULL;

typedef struct TwoWatchedClause 
{
    int ids[2];
    Clause *c;
} TwoWatchedClause;

typedef struct TwoWatchedLiterals 
{
    TwoWatchedClause *c;
    struct TwoWatchedLiterals *next;
} TwoWatchedLiterals;

TwoWatchedLiterals* push(TwoWatchedLiterals *list, TwoWatchedClause *c)
{
    TwoWatchedLiterals *new = (TwoWatchedLiterals *) malloc(sizeof(TwoWatchedLiterals));
    new->c = c;
    new->next = list;
    return new;
}

TwoWatchedLiterals **twl = NULL;
TwoWatchedClause *twc = NULL;

void PreProcessing(Form *form)
{
    // JEROSLAW

    scores = (double *) calloc((form->numVars * 2), sizeof(double));

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

    // TWO WATCHED LITERALS

    // aloca vetor de clausulas
    // TODO: verificar e tratar clausulas unitarias
    twc = (TwoWatchedClause *) malloc(sizeof(TwoWatchedClause) * form->numClauses);
    ClauseNode *twc_pivot = form->clauses;
    for (int i=0; i < form->numClauses; i++)
    {
        twc[i].c = twc_pivot->clause;
        twc[i].ids[0] = 0;
        twc[i].ids[1] = (twc[i].c->size > 1 ? 1 : 0);
        twc_pivot = twc_pivot->next;
    }

    // aloca vetor de literais
    int numLiterals = form->numVars * 2;
    twl = (TwoWatchedLiterals **) calloc(numLiterals, sizeof(TwoWatchedLiterals *));

    // aloca listas encadeadas
    for (int i=0; i < numLiterals; i++)
    {
        twl[i] = (TwoWatchedLiterals *) malloc (sizeof(TwoWatchedLiterals));
    }

    // preenche
    ClauseNode *pivot, *head=form->clauses;
    for (int i=0; i < form->numClauses; i++){
        for (int j=0; j < 2; j++){
            int pos = getPos(twc[i].c->literals[twc[i].ids[j]]); // pos = getPos(twc[i].c->literals[twc[i].ids[j]]);
            twl[pos] = push(twl[pos], &twc[i]);
        }
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