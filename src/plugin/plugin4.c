#include <limits.h>

#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../dpll.h"

#define TOP_PERCENT 0.2
// --- HEAP ---

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

void clearHeap(MaxHeap *h, int max) {
    h->N = max;
    for (int i=0; i<h->N; i++) {
        h->arr[i].count = 0;
        h->arr[i].literal = i+1;
    }
    for (int i = 0; i < h->N; i++) {
        h->indexTable[i] = i; // Literal l, at index i=l-1, is at position i on the heap
    }
}

MaxHeap *createMaxHeap(const int literalNum) {
    MaxHeap *maxHeap = safeMalloc(sizeof(MaxHeap));
    maxHeap->N = literalNum;
    maxHeap->arr = (LiteralCount *) safeMalloc(sizeof(LiteralCount)*literalNum);
    maxHeap->indexTable = (int *) safeMalloc(sizeof(int)*literalNum);
    clearHeap(maxHeap, literalNum);
    return maxHeap;
}

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

void sink(MaxHeap *h, int idx) {
    while (true) {
        int leftChild = getLeftChild(idx);
        int rightChild = getRightChild(idx);
        int largest = idx;

        if (leftChild < h->N && h->arr[leftChild].count > h->arr[largest].count) {
            largest = leftChild;
        }

        if (rightChild < h->N && h->arr[rightChild].count > h->arr[largest].count) {
            largest = rightChild;
        }

        if (largest == idx) {
            break;
        }

        swap(h, idx, largest);
        idx = largest;
    }
}

// NOTE: variableId is zero indexed
void appendCount(MaxHeap *h, int variableId) {
    if (h == NULL || h->arr == NULL || h->indexTable == NULL ) {
        fprintf(stderr, "[appendCount]: Heap not initialized properly\n");
        abort();
    }
    if (variableId < 0 || variableId >= h->N) {
        fprintf(stderr, "[appendCount]: Variable index out of bounds\n");
        abort();
    }
    const int idx = h->indexTable[variableId];
    h->arr[idx].count++;
    swim(h, idx);
}

LiteralCount peek(const MaxHeap *h) {
    if (h == NULL || h->arr == NULL || h->indexTable == NULL) {
        fprintf(stderr, "[peek]: Heap not initialized properly\n");
        abort();
    }
    if (h->N <= 0) {
        fprintf(stderr, "[peek]: Heap is empty\n");
        abort();
    }
    return h->arr[0];
}

LiteralCount pop(MaxHeap *h) {
    if (h->N <= 0) {
        fprintf(stderr, "[pop]: Heap is empty\n");
        abort();
    }

    LiteralCount result = h->arr[0];
    h->N--;
    h->arr[0] = h->arr[h->N];
    h->indexTable[h->arr[0].literal - 1] = 0;
    sink(h, 0);
    h->indexTable[result.literal - 1] = -1;

    return result;
}

void freeHeap(MaxHeap *h) {
    if (!h) return;
    free(h->arr);
    free(h->indexTable);
    free(h);
}
// -----

void PreProcessing(Form* form){
    // Código ruim
    srand(time(NULL));
}

typedef struct {
    MaxHeap *positiveLiterals;
    MaxHeap *negativeLiterals;
} DLISCounts;

DLISCounts *startDLIS(const int variableNum) {
    DLISCounts *dlis = safeMalloc(sizeof(DLISCounts));
    dlis->positiveLiterals = createMaxHeap(variableNum);
    dlis->negativeLiterals = createMaxHeap(variableNum);

    return dlis;
}

void freeDLIS(DLISCounts *dlis) {
    if (!dlis) return;
    freeHeap(dlis->positiveLiterals);
    freeHeap(dlis->negativeLiterals);
    free(dlis);
}

int getMostFrequentLiteral(DLISCounts *dlis) {
    if (dlis->positiveLiterals == NULL || dlis->negativeLiterals == NULL) {
        fprintf(stderr, "Checking null pointer (DLIS)\n");
        abort();
    }
    const LiteralCount maxPositive = peek(dlis->positiveLiterals);
    const LiteralCount maxNegative = peek(dlis->negativeLiterals);

    if (maxNegative.count < maxPositive.count) {
        pop(dlis->positiveLiterals);
        return maxPositive.literal;
    }
    pop(dlis->negativeLiterals);
    return -maxNegative.literal;
}

DLISCounts *counts;
int *isDecided;
void startCounts(int numVars) {
    if (counts == NULL) {
        counts = startDLIS(numVars);
    } else {
        clearHeap(counts->positiveLiterals, numVars);
        clearHeap(counts->negativeLiterals, numVars);
    }

    if (isDecided == NULL) {
        isDecided = (int *) safeMalloc(numVars*sizeof(int));
    }
}

int ceil(float num) {
    int inum = (int) num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}

enum DecideState Decide(const Form* form) {
    startCounts(form->numVars);
    for (const ClauseNode* currentNode = form->clauses; currentNode != NULL; currentNode = currentNode->next) {
        /* NOTE: Ter um tipo nó separado da cláusula em vez de usar uma lista encadeada
         * intrusiva é ineficiente e feio
         */
        const Clause *currentClause = currentNode->clause;
        for(int literalIdx = 0; literalIdx<currentClause->size; ++literalIdx) {
            const LiteralId literal = currentClause->literals[literalIdx];
            const int variableId = abs(literal)-1;
            if (getVarState(variableId) == UNK) {
                isDecided[variableId] = false;
                if (literal > 0)
                    appendCount(counts->positiveLiterals, variableId);
                else
                    appendCount(counts->negativeLiterals, variableId);
            } else {
                isDecided[variableId] = true;
            }
        }
    }
    int unknownCount = 0;
    for (int i=0; i<form->numVars; ++i) if (!isDecided[i]) unknownCount++;
    if (unknownCount == 0) return ALL_ASSIGNED;

    const int topPercent = ceil(TOP_PERCENT*(float) unknownCount);

    int *topLiterals = safeMalloc(topPercent*sizeof(int));
    for (int i=0; i<topPercent; i++) topLiterals[i] = getMostFrequentLiteral(counts);
    const int chosenLiteral = topLiterals[rand()%topPercent];
    free(topLiterals);

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

