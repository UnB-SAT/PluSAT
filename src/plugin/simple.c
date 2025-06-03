#include "dpll.h"
#include "formula.h"
#include <stdio.h>
#include <stdlib.h>

ClauseNode *learned_clauses = NULL;
Clause *conflicting_clause_ptr = NULL;

// A cada no máximo 100 conflitos a ativadade das variáveis decai
#define DECAY_PERIOD 100

static double var_inc = 1.0f;
static double var_decay = 0.95f;
static unsigned short conflicts_since_decay = 0;
int num_variables = 0;
double *activity = NULL;

void PreProcessing(Form *form)
{
    ClauseNode *pivot, *head = form->clauses;

    while (head != NULL)
    {
        pivot = head;

        for (int j = 0; j < pivot->clause->size; ++j)
        {
            printf("%d ", pivot->clause->literals[j]);
        }

        head = head->next;
        printf("\n");
    }

    // Trabalhar estrutura de dados
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
                insertDecisionLevel(var_id - 1, FALSE); // Tenta o valor FALSE primeiro [cite: 306]
                return FOUND_VAR;
            }
        }
        it = it->next;
    }

    double max_activity = -1.0;
    int best_var = -1;

    for (int i = 0; i < num_variables; i++)
    {
        if (getVarState(i) == UNK && activity[i] > max_activity){
            max_activity = activity[i];
            best_var = i;
        }
    }

    if (best_var != -1){
        insertDecisionLevel(best_var, FALSE);
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

    // now if some clause have only negative
    // values than this is a conflict
    while (head != NULL)
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

    return true;
}

int resolveConflict()
{
    conflicts_since_decay++;
    if (conflicting_clause_ptr != NULL) {
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
