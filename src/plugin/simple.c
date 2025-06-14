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

TwoWatchedLiterals **twl = NULL;
TwoWatchedClause *twc = NULL;

TwoWatchedLiterals* push(TwoWatchedLiterals *list, TwoWatchedClause *c)
{
    TwoWatchedLiterals *new = (TwoWatchedLiterals *) malloc(sizeof(TwoWatchedLiterals));
    new->c = c;
    new->next = list;
    return new;
}

TwoWatchedLiterals* del(TwoWatchedLiterals *head, TwoWatchedClause *target)
{
    if (head == NULL) return NULL;

    if (head->c == target)
    {
        TwoWatchedLiterals *next = head->next;
        free(head);
        return next;
    }

    TwoWatchedLiterals *prev = head;
    TwoWatchedLiterals *curr = head->next;

    while (curr != NULL)
    {
        if (curr->c == target)
        {
            prev->next = curr->next;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    return head;
}

bool find_and_move(TwoWatchedLiterals **list, TwoWatchedClause *tw_clause, int falseIndex, int pos)
{
    Clause *clause = tw_clause->c;
    for (int i=0; i < clause->size; i++)
        {
            if (i == tw_clause->ids[0] || i == tw_clause->ids[1]) continue;

            LiteralId candidate = clause->literals[i];

            if (getLitState(candidate) != FALSE)
            {
                // troca pelo literal falsificado
                tw_clause->ids[(falseIndex == tw_clause->ids[0])? 0 : 1] = i;

                // Move a cláusula da lista do literal antigo para o novo
                list[pos] = del(list[pos], tw_clause);

                int newPos = getPos(candidate);
                list[newPos] = push(list[newPos], tw_clause);

                return true;
            }
        }
        return false;
}

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

    // preenche
    ClauseNode *head=form->clauses;
    for (int i=0; i < form->numClauses; i++){
        int w0 = twc[i].ids[0];
        int w1 = twc[i].ids[1];

        LiteralId l0 = twc[i].c->literals[w0];
        twl[getPos(l0)] = push(twl[getPos(l0)], &twc[i]);

        if (w0 != w1)
        {
            LiteralId l1 = twc[i].c->literals[w1];
            twl[getPos(l1)] = push(twl[getPos(l1)], &twc[i]);
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
    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? decision.id + 1 : -decision.id - 1);
    int pos = getPos(falseValuedLiteral);

    TwoWatchedLiterals *list = twl[pos];

    while (list != NULL)
    {
        TwoWatchedClause *tw_clause = list->c;
        Clause *clause = tw_clause->c;

        int watch0 = tw_clause->ids[0];
        int watch1 = tw_clause->ids[1];

        // verifica qual dos literais se tornou falso
        int falseIndex = -1;

        if (clause->literals[watch0] == falseValuedLiteral) falseIndex = watch0;
        else if (clause->literals[watch1] == falseValuedLiteral) falseIndex = watch1;
        else 
        {
            list = list->next;
            continue;
        }

        int otherIndex = (falseIndex == watch0) ? watch1 : watch0;
        LiteralId otherLit = clause->literals[otherIndex];

        // Se o outro literal está TRUE, a cláusula está satisfeita
        // talvez seja mais correto verificar se é diferente de FALSE
        if (getLitState(otherLit) == TRUE) 
        {
            list = list->next;
            continue;
        }

        // encontrar um novo literal
        if (find_and_move(twl, tw_clause, falseIndex, pos))
        {
            list = list->next;
            continue;
        }
        
        // Nenhum outro literal pôde ser assistido
        if (getLitState(otherLit) == UNK)
        {
            // bool value = (otherLit > 0);
            // propagar
            // insertDecisionLevel(abs(otherLit) -1, (otherLit > 0));
            // setVarState(abs(otherLit) - 1, otherLit > 0 ? TRUE : FALSE);
            // return false;
        } else
        {
            // Conflito: os dois literais são FALSE
            return false;
        }

        list = list->next;
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