#include <limits.h>

#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

#include "../dpll.h"

// --- HEAP ---

typedef struct {
    int count;
    int literal;
} LiteralCount;

typedef struct {
    int N;
    LiteralCount *arr;
    int *indexTable;
} MaxHeap;

void swap(const MaxHeap *h, const int idxA, const int idxB) {
    LiteralCount *literalA = &h->arr[idxA];
    LiteralCount *literalB = &h->arr[idxB];

    h->indexTable[literalA->literal - 1] = idxB;
    h->indexTable[literalB->literal - 1] = idxA;

    const LiteralCount temp = *literalA;
    *literalA = *literalB;
    *literalB = temp;
}

int getFather(const int idx) {
    return (idx-1)/2;
}

int getLeftChild(const int idx) {
    return idx*2+1;
}

int getRightChild(const int idx) {
    return idx*2+2;
}

MaxHeap *createMaxHeap(const int literalNum) {
    MaxHeap *maxHeap = malloc(sizeof(MaxHeap));
    maxHeap->N = literalNum;
    maxHeap->arr = (LiteralCount *) malloc(sizeof(LiteralCount)*literalNum);
    for (int i=0; i<literalNum; i++) {
        maxHeap->arr[i].count = 0;
        maxHeap->arr[i].literal = i+1;
    }
    maxHeap->indexTable = (int *) malloc(sizeof(int)*literalNum);
    for (int i = 0; i < literalNum; i++) {
        maxHeap->indexTable[i] = i; // Literal l, at index i=l-1, is at position i on the heap
    }
    return maxHeap;
}

// There is no sink function since we only count up
void swim(MaxHeap *h, int idx) {
    if (h->arr == NULL) {
        fprintf(stderr, "[swim]: Heap not initialized properly\n");
        abort();
    }
    while (idx > 0 && h->arr[idx].count > h->arr[getFather(idx)].count ) {
        swap(h, idx, getFather(idx));
        idx = getFather(idx);
    }
}

// NOTE: variableId is zero indexed
void appendCount(MaxHeap *h, int variableId) {
    if (h->arr == NULL || h->indexTable == NULL) {
        fprintf(stderr, "[appendCount]: Heap not initialized properly\n");
        abort();
    }
    const int idx = h->indexTable[variableId];
    h->arr[idx].count++;
    swim(h, idx);
}

const LiteralCount* peek(const MaxHeap *h) {
    if (h->N > 0) {
        return &h->arr[0];
    }
    fprintf(stderr, "[peek]: Heap not initialized properly\n");
    abort();
}


void freeHeap(MaxHeap *h) {
    if (!h) {
        fprintf(stderr, "Freeing null pointer (MaxHeap)\n");
        abort();
    }
    free(h->arr);
    free(h->indexTable);
    free(h);
}
// -----

void PreProcessing(Form* form){
}

typedef struct {
    MaxHeap *positiveLiterals;
    MaxHeap *negativeLiterals;
} DLISCounts;

DLISCounts *startDLIS(const int variableNum) {
    DLISCounts *dlis = malloc(sizeof(DLISCounts));
    dlis->positiveLiterals = createMaxHeap(variableNum);
    dlis->negativeLiterals = createMaxHeap(variableNum);

    return dlis;
}

void freeDLIS(DLISCounts *dlis) {
    if (dlis == NULL) {
        fprintf(stderr, "Freeing null pointer (DLIS)\n");
        abort();
    }
    freeHeap(dlis->positiveLiterals);
    freeHeap(dlis->negativeLiterals);
    free(dlis);
}

int getMostFrequentLiteral(DLISCounts *dlis) {
    if (dlis->positiveLiterals == NULL || dlis->negativeLiterals == NULL) {
        fprintf(stderr, "Checking null pointer (DLIS)\n");
        abort();
    }
    const LiteralCount *maxPositive = peek(dlis->positiveLiterals);
    const LiteralCount *maxNegative = peek(dlis->negativeLiterals);

    return maxNegative->count > maxPositive->count ? -maxNegative->literal : maxPositive->literal;
}

// NOTE: Alocar e desalocar repetidamente é rui
enum DecideState Decide(const Form* form) {
    bool allDecided = true;
    DLISCounts* counts = startDLIS(form->numVars);
    for (const ClauseNode* currentNode = form->clauses; currentNode != NULL; currentNode = currentNode->next) {
        const Clause *currentClause = currentNode->clause;
        for(int literalIdx = 0; literalIdx<currentClause->size; ++literalIdx) {
            const LiteralId literal = currentClause->literals[literalIdx];
            const int variableId = abs(literal)-1;
            printf("%d\n", variableId);
            if (literal > 0)
                appendCount(counts->positiveLiterals, variableId);
            else
                appendCount(counts->negativeLiterals, variableId);
            if (getVarState(variableId) == UNK) allDecided = false;
        }
    }
    /* NOTE: Era para ter uma lista de variáveis e suas
     * decisões para não ter que iterar por todas as cláusulas
     * para verificar se todas as variáveis já foram decididas
     */
    if (allDecided) {
        freeDLIS(counts);
        return ALL_ASSIGNED;
    }

    const int mostFrequentLiteral = getMostFrequentLiteral(counts);
    insertDecisionLevel(abs(mostFrequentLiteral)-1, mostFrequentLiteral > 0 ? TRUE : FALSE);
    freeDLIS(counts);
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

