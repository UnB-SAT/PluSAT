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
    int num_vars = form->numVars;
    scores = (double *) calloc((num_vars * 2), sizeof(double));
    sorted_vars = (VarScore *) calloc(num_vars, sizeof(VarScore));

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
    // Fila para literais false-valued a serem propagados
    int numVars = formula->numVars;
    LiteralId *queue = (LiteralId*) malloc(sizeof(LiteralId) * numVars);
    int head = 0, tail = 0;

    // Determina o literal que foi falseficado pela decisão atual
    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? (decision.id + 1) : -decision.id - 1);
    queue[tail++] = falseValuedLiteral;

    // Processa todas as implicações até esvaziar a fila
    while (head < tail)
    {
        LiteralId lit = queue[head++];
        int pos = getPos(lit);
        TwoWatchedLiterals *node = twl[pos];

        // Percorre cada cláusula assistida pelo literal falseficado
        while (node != NULL)
        {
            TwoWatchedLiterals *nextNode = node->next;
            TwoWatchedClause *tw_clause = node->c;
            Clause *clause = tw_clause->c;

            int watch0 = tw_clause->ids[0];
            int watch1 = tw_clause->ids[1];
            int falseIndex = -1;

            // Descobre qual dos literais assistidos se tornou falso
            if (clause->literals[watch0] == lit) falseIndex = watch0;
            else if (clause->literals[watch1] == lit) falseIndex = watch1;
            else
            {
                // Se a cláusula não está assistida por 'lit', ignora-a
                node = nextNode;
                continue;
            }

            // Identifica o outro literal assistido na cláusula
            int otherIndex = (falseIndex == watch0) ? watch1 : watch0;
            LiteralId otherLit = clause->literals[otherIndex];

            // Se o outro literal já é TRUE, a cláusula já está satisfeita
            if (getLitState(otherLit) == TRUE)
            {
                node = nextNode;
                continue;
            }

            // Tenta encontrar um novo literal não-FALSE para assistir na cláusula
            if (find_and_move(twl, tw_clause, falseIndex, pos))
            {
                // Se conseguiu mover o watch para outro literal, passa para a próxima cláusula
                node = nextNode;
                continue;
            }

            // Se não há novo literal para assistir, a cláusula ficou com apenas um literal relevante (otherLit)
            if (getLitState(otherLit) == UNK)
            {
                // Implicação: define otherLit para satisfazer a cláusula
                VariableId var = abs(otherLit) - 1;
                Decision *decisions = getDecisions();
                int currentLevel = getLevel();
                bool already = false;
                // Verifica se a variável já foi registrada no histórico de decisões
                for (int i = 0; i < currentLevel; ++i)
                {
                    if (decisions[i].id == var)
                    {
                        already = true;
                        break;
                    }
                }
                // Se ainda não foi registrada, adiciona como decisão de propagação
                if (!already)
                {
                    insertDecisionLevel(var, (otherLit > 0));
                    setVarState(var, (otherLit > 0) ? TRUE : FALSE);
                    // Adiciona o literal correspondente false-valued à fila de propagação
                    queue[tail++] = -otherLit;
                }
                // Continua com a próxima cláusula assistida por 'lit'
                node = nextNode;
                continue;
            }

            // Se o outro literal for FALSE, ocorre conflito real (cláusula sem literais possíveis)
            free(queue);
            return false;
        }
    }

    // Todas as implicações foram propagadas sem conflito
    free(queue);
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