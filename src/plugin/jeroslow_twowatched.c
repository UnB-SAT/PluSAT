#include "dpll.h"
#include "formula.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct TwoWatchedClause
{
    int ids[2];
    Clause *c;
    // int size;
} TwoWatchedClause;
// 50 Variáveis | 218 clausulas
typedef struct TwoWatchedLiterals
{
    TwoWatchedClause *c;
    struct TwoWatchedLiterals *next;
} TwoWatchedLiterals;

TwoWatchedLiterals *twl = NULL;

ClauseNode *learned_clauses = NULL;
Clause *conflicting_clause_ptr = NULL;

#define DECAY_PERIOD 100

static double var_inc = 1.0f;
static double var_decay = 0.95f;
static unsigned short conflicts_since_decay = 0;
int num_variables = 0;
double *activity = NULL;

void PreProcessing(Form *form)
{
    ClauseNode *pivot, *head = form->clauses;
    twl = malloc(sizeof(TwoWatchedLiterals) * (form->numClauses));

    int i = form->numClauses - 1;
    while (head != NULL)
    {
        pivot = head;

        LiteralId lit1 = pivot->clause->literals[0];
        LiteralId lit2 = pivot->clause->literals[1];

        if (twl[i].c == NULL)
        {
            twl[i].c = malloc(sizeof(TwoWatchedClause));
            twl[i].c->ids[0] = lit1;
            twl[i].c->ids[1] = lit2;
            twl[i].c->c = pivot->clause;
            // twl[i].c->size = pivot->clause->size;
        }

        i--;
        head = head->next;
    }

    /* for (int i = 0; i < form->numClauses; ++i)
    {
        printf("Literals in %d: %d, %d\n", i, twl[i].c->ids[0], twl[i].c->ids[1]);
    } */

    num_variables = form->numVars;
    activity = (double *)malloc(sizeof(double) * (num_variables + 1));
    if (!activity)
    {
        perror("Falha ao alocar memória para as atividades das variáveis");
        exit(1);
    }

    for (int i = 0; i < form->numVars; i++)
        activity[i] = 0.0f;
}

enum DecideState Decide(const Form *form)
{
    ClauseNode *it = learned_clauses;
    while (it != NULL)
    {
        Clause *clause = it->clause;
        for (int i = 0; i < clause->size; ++i)
        {
            LiteralId var_id = clause->literals[i];
            var_id = ((var_id > 0) ? var_id : -var_id);

            if (getVarState(var_id - 1) == UNK)
            {
                insertDecisionLevel(var_id - 1, FALSE); // Tenta o valor FALSE primeiro
                return FOUND_VAR;
            }
        }
        it = it->next;
    }

    double max_activity = -1.0;
    int best_var = -1;

    for (int i = 0; i < num_variables; i++)
    {
        if (getVarState(i) == UNK && activity[i] > max_activity)
        {
            max_activity = activity[i];
            best_var = i;
        }
    }

    if (best_var != -1)
    {
        insertDecisionLevel(best_var, FALSE);
        return FOUND_VAR;
    }

    return ALL_ASSIGNED;
}

bool BCP(Form *formula, const Decision decision)
{
    LiteralId falseLit = ((decision.value == FALSE) ? decision.id + 1 : -decision.id - 1);

    for (int i = 0; i < formula->numClauses; ++i)
    {
        TwoWatchedClause *twClause = twl[i].c;
        if (!twClause)
            continue;

        int watchedIdx = -1;

        if (twClause->ids[0] == falseLit)
            watchedIdx = 0;
        else if (twClause->ids[1] == falseLit)
            watchedIdx = 1;

        if (watchedIdx == -1)
            continue; // falseLit não é um dos watched

        int otherIdx = 1 - watchedIdx;
        LiteralId otherLit = twClause->ids[otherIdx];

        // Tente achar novo literal para assistir
        bool found = false;
        Clause *clause = twClause->c;

        for (int j = 0; j < clause->size; j++)
        {
            LiteralId l = clause->literals[j];

            if (l != twClause->ids[0] && l != twClause->ids[1] && getLitState(l) != FALSE)
            {
                twClause->ids[watchedIdx] = l;
                found = true;
                break;
            }
        }

        if (found)
            continue; // Conseguiu mover o watch, tudo ok

        // Se não conseguiu mover:
        LitState otherState = getLitState(otherLit);

        if (otherState == TRUE || otherState == UNK)
        {
            continue;
        }
        else
        {
            // Conflito!
            conflicting_clause_ptr = clause;
            return false;
        }
    }

    // now if some clause have only negative
    // values than this is a conflict
    /* while (head != NULL)
    {
        flag = false;
        clause = head->clause;

        for (int i = 0; i < clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            if (getLitState(lit) != FALSE)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            conflicting_clause_ptr = clause;
            return false;
        }

        head = head->next;
    }
    */

    return true;
}

int resolveConflict()
{
    conflicts_since_decay++;
    if (conflicting_clause_ptr != NULL)
    {
        // learned_clauses = addNodeOnList(conflicting_clause_ptr, learned_clauses)
        for (int i = 0; i < conflicting_clause_ptr->size; i++)
        {
            LiteralId lit = conflicting_clause_ptr->literals[i];
            int var_id = ((lit > 0) ? lit : -lit) - 1;

            activity[var_id] += var_inc;
        }

        conflicting_clause_ptr = NULL;
    }

    if (conflicts_since_decay >= DECAY_PERIOD)
    {
        conflicts_since_decay = 0;
        for (int i = 0; i < num_variables; i++)
        {
            activity[i] /= var_decay;
        }
    }

    Decision *decisions = getDecisions();

    int i = getLevel() - 1;

    for (; i >= 0; --i)
        if (decisions[i].flipped == false)
            break;

    return i + 1;
}