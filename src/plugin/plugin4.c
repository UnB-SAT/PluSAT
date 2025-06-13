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
    int varNum;
    LiteralCount *arr;
    int *indexTable;
} MaxHeap;

// Assuming variable is zero indexed;
int getVar(const int literal) {
    return abs(literal)-1;
}

// The Second half of the index table is where the negative literals are stored
int getIndex(const int varNum, const int literal) {
    if (abs(literal) > varNum) {
        fprintf(stderr, "Array index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    if (literal == 0) {
        fprintf(stderr, "Literal cannot be 0\n");
        exit(EXIT_FAILURE);
    }
    const int index = getVar(literal);
    if (literal > 0) {
        return index;
    }
    return index + varNum;
}

// Returns position of literal in the heap
int indexTableLookup(const MaxHeap *h, const int literal) {
    return h->indexTable[getIndex(h->varNum, literal)];
}

void setIndexTable(const MaxHeap *h, const int literal, int newIdx) {
    h->indexTable[getIndex(h->varNum, literal)] = newIdx;
}

LiteralCount *MaxHeapGet(const MaxHeap *h, const int literal) {
    return &h->arr[indexTableLookup(h, literal)];
}

void swap(const MaxHeap *h, const int idxA, const int idxB) {
    LiteralCount *literalA = &h->arr[idxA];
    LiteralCount *literalB = &h->arr[idxB];

    setIndexTable(h, literalA->literal, idxB);
    setIndexTable(h, literalB->literal, idxA);

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

void clearHeap(MaxHeap *h, const int varNum) {
    h->varNum = varNum;
    h->N = varNum*2;
    for (int i=0; i<h->varNum; i++) {
        h->arr[i].count = 0;
        const int literal = i+1;
        h->arr[i].literal = literal;
        setIndexTable(h, literal, i);
    }
    for (int i=h->varNum; i<h->N; i++) {
        h->arr[i].count = 0;
        const int literal = -(i - h->N/2 + 1);
        h->arr[i].literal = literal;
        setIndexTable(h, literal, i);
    }
}

MaxHeap *createMaxHeap(const int varNum) {
    MaxHeap *h = safeMalloc(sizeof(MaxHeap));
    h->varNum = varNum;
    h->N = varNum*2;
    h->arr = (LiteralCount *) safeMalloc(sizeof(LiteralCount)*h->N);
    h->indexTable = (int *) safeMalloc(sizeof(int)*h->N);
    clearHeap(h, varNum);
    return h;
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
void appendCount(MaxHeap *h, int literal) {
    if (h == NULL || h->arr == NULL || h->indexTable == NULL ) {
        fprintf(stderr, "[appendCount]: Heap not initialized properly\n");
        abort();
    }
    if (abs(literal) == 0 || abs(literal) > h->N/2) {
        fprintf(stderr, "[appendCount]: Literal out of bounds\n");
        abort();
    }
    const int idx = indexTableLookup(h, literal);
    h->arr[idx].count++;
    swim(h, idx);
}

LiteralCount pop(MaxHeap *h) {
    if (h->N <= 0) {
        fprintf(stderr, "[pop]: Heap is empty\n");
        abort();
    }

    LiteralCount result = h->arr[0];
    setIndexTable(h, result.literal, -1);
    h->N--;

    h->arr[0] = h->arr[h->N];
    setIndexTable(h, h->arr[0].literal, 0);
    sink(h, 0);

    return result;
}

void freeHeap(MaxHeap *h) {
    if (!h) return;
    free(h->arr);
    free(h->indexTable);
    free(h);
}

// Para debug
void printHeapStatus(const MaxHeap *h) {
    if (h == NULL) {
        printf("Heap is NULL\n");
        return;
    }

    if (h->arr == NULL || h->indexTable == NULL) {
        printf("Heap not properly initialized\n");
        return;
    }

    printf("=== Heap Status ===\n");
    printf("Variables: %d, Total size: %d, Current elements: %d\n",
           h->varNum, h->varNum * 2, h->N);

    if (h->N == 0) {
        printf("Heap is empty\n");
        printf("==================\n");
        return;
    }

    printf("\nHeap elements (in heap order):\n");
    printf("Index | Literal | Count\n");
    printf("------|---------|------\n");
    for (int i = 0; i < h->N; i++) {
        printf("%5d | %7d | %5d", i, h->arr[i].literal, h->arr[i].count);
        if (i == 0) printf(" <- ROOT");
        printf("\n");
    }

    printf("\nIndex table mapping:\n");
    printf("Literal | Heap Index\n");
    printf("--------|----------\n");

    for (int var = 1; var <= h->varNum; var++) {
        const int heapIdx = h->indexTable[getIndex(h->varNum, var)];
        printf("%7d | %10d", var, heapIdx);
        if (heapIdx >= 0 && heapIdx < h->N) {
            printf(" (count: %d)", h->arr[heapIdx].count);
        } else if (heapIdx == -1) {
            printf(" (removed)");
        }
        printf("\n");
    }

    for (int var = 1; var <= h->varNum; var++) {
        int heapIdx = h->indexTable[getIndex(h->varNum, -var)];
        printf("%7d | %10d", -var, heapIdx);
        if (heapIdx >= 0 && heapIdx < h->N) {
            printf(" (count: %d)", h->arr[heapIdx].count);
        } else if (heapIdx == -1) {
            printf(" (removed)");
        }
        printf("\n");
    }

    printf("==================\n");
}

// -----

void PreProcessing(Form* form){
    // Código ruim
    srand(time(NULL));
}

MaxHeap *h;
int *isDecided;
void startCounts(const int numVars) {
    if (h == NULL) {
        h = createMaxHeap(numVars);
    } else {
        clearHeap(h, numVars);
    }

    if (isDecided == NULL) {
        isDecided = (int *) safeMalloc(numVars*sizeof(int));
    }
}

int myCeil(const float num) {
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
            const int variableId = getVar(literal);
            if (getVarState(variableId) == UNK) {
                isDecided[variableId] = false;
                appendCount(h, literal);
            } else {
                isDecided[variableId] = true;
            }
        }
    }
    int unknownCount = 0;
    for (int i=0; i<form->numVars; ++i) if (!isDecided[i]) unknownCount++;
    if (unknownCount == 0) return ALL_ASSIGNED;

    const int topPercent = myCeil(TOP_PERCENT* unknownCount);

    int *topLiterals = safeMalloc(topPercent*sizeof(int));
    for (int i=0; i<topPercent; i++) topLiterals[i] = pop(h).literal;

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

