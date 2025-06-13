#include <limits.h>

#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOP_PERCENT 0.2

void *safeMalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }

    return p;
}

typedef struct {
    int count;
    int literal;
} LiteralCount;

void PreProcessing(Form* form){
    // Código ruim
    srand(time(nullptr));
}

int comparLiteral(const void *a, const void *b) {
    const LiteralCount *p = (LiteralCount*) a;
    const LiteralCount *pb = (LiteralCount*) b;
    return (pb->count > p->count) - (pb->count < p->count);
}

LiteralCount *literalCounts;
int *topLiterals;
void startCounts(const Form *form) {
    if (topLiterals == NULL) topLiterals = safeMalloc(form->numVars * 2 * sizeof(int));
    if (literalCounts != NULL) return;
    literalCounts = safeMalloc(form->numVars * 2 * sizeof(LiteralCount));
    for (int i=0; i<form->numVars; i++) {
        literalCounts[i].count = 0;
        literalCounts[i].literal = i+1;
    }
    for (int i=form->numVars; i<form->numVars*2; i++) {
        literalCounts[i].count = 0;
        literalCounts[i].literal = -(i+1 - form->numVars);
    }
    for (const ClauseNode* currentNode = form->clauses; currentNode != NULL; currentNode = currentNode->next) {
        /* NOTE: Ter um tipo nó separado da cláusula em vez de usar uma lista encadeada
         * intrusiva é ineficiente e feio
         */
        const Clause *currentClause = currentNode->clause;
        for(int literalIdx = 0; literalIdx<currentClause->size; ++literalIdx) {
            const LiteralId literal = currentClause->literals[literalIdx];
            const int idx = literal > 0 ? literal-1 : abs(literal)-1+form->numVars;
            literalCounts[idx].count++;
        }
    }
    // TODO: Trocar por sort mais eficiente
    qsort(literalCounts, form->numVars * 2, sizeof(LiteralCount), comparLiteral);
}

int myCeil(const float num) {
    int inum = (int) num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}

enum DecideState Decide(const Form* form) {
    startCounts(form);

    // NOTE: Possível off by one
    const int unknownCount = form->numVars - getLevel();
    // printf("(%d/%d): %d\n", getLevel(), form->numVars, unknownCount);
    if (unknownCount == 0)
        return ALL_ASSIGNED;
    const int topPercent = myCeil(TOP_PERCENT* unknownCount);

    int insertedCount = 0;
    int i = 0;
    while (insertedCount < topPercent) {
        if (i >= form->numVars * 2) {
            fprintf(stderr, "[Decide]: Index out of bounds\n");
            abort();
        }
        const LiteralCount l = literalCounts[i++];
        if (getVarState(abs(l.literal)-1) != UNK) continue;
        topLiterals[insertedCount++] = l.literal;
    }

    const int chosenLiteral = topLiterals[rand()%topPercent];
    insertDecisionLevel(abs(chosenLiteral)-1,  chosenLiteral > 0 ? TRUE : FALSE);
    return FOUND_VAR;
}

bool BCP(Form *formula, const Decision decision)
{
    bool flag;
    ClauseNode *head;
    Clause *clause;

    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? decision.id+1 : -decision.id -1);

    head = formula->literals[getPos(falseValuedLiteral)];

    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        flag = false;
        clause = head->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            if(getLitState(lit) != FALSE)
                flag=true;
        }

        if(!flag){
            return false;
        }

        head = head->next;
   }

   return true;
}

int resolveConflict()
{
    Decision* decisions =  getDecisions();

    int i = getLevel()-1;

    for(; i>=0; --i)
        if(decisions[i].flipped == false)
            break;

    return i+1;
}

