#include <limits.h>

#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

#include "../dpll.h"

void PreProcessing(Form* form){
}

typedef struct {
    int variableCount; // Redundante
    int *trueCounts;
    int *falseCounts;
} DLISCounts;

/* Given a variable "v", if the most frequent literal is
 * ~v, the function returns -v, otherwise it returns
 * positive v. Literals start at 1
 */
int getMostFrequentLiteral(const DLISCounts* counts) {
    if (counts == NULL) {
        fprintf(stderr, "getMostFrequentLiteral: NULL DLISCounts\n");
        abort();
    }
    if (counts->trueCounts == NULL) {
        fprintf(stderr, "Null pointer in DLIS true literal counts");
        abort();
    }
    if (counts->falseCounts == NULL) {
        fprintf(stderr, "Null pointer in DLIS false literal counts");
        abort();
    }
    int maxCount = INT_MIN;
    int maxLiteral = INT_MIN;
    for (size_t i = 0; i < counts->variableCount; i++) {
        if (counts->trueCounts[i] > maxCount) {
            maxCount = counts->trueCounts[i];
            maxLiteral = i+1;
        } else if (counts->falseCounts[i] > maxCount) {
            maxCount = counts->falseCounts[i];
            maxLiteral = -(i+1);
        }
    }
    return maxLiteral;
}

DLISCounts* initCounts(int numVars) {
    DLISCounts* counts = malloc(sizeof(DLISCounts));
    counts->variableCount = numVars;
    counts->trueCounts = calloc(numVars, sizeof(int));
    counts->falseCounts = calloc(numVars, sizeof(int));
    return counts;
}

void freeCounts(DLISCounts* counts) {
    if (counts == NULL) {
        fprintf(stderr, "NULL DLISCounts when freeing\n");
        abort();
    }
    free(counts->trueCounts);
    free(counts->falseCounts);
    free(counts);
}

// NOTE: Alocar e desalocar repetidamente é rui
enum DecideState Decide(const Form* form) {
    bool allDecided = true;
    DLISCounts* counts = initCounts(form->numVars);
    for (const ClauseNode* currentNode = form->clauses; currentNode != NULL; currentNode = currentNode->next) {
        const Clause *currentClause = currentNode->clause;
        for(int literalIdx = 0; literalIdx<currentClause->size; ++literalIdx) {
            const LiteralId literal = currentClause->literals[literalIdx];
            const int variableId = abs(literal)-1;
            if (literal > 0)
                counts->trueCounts[variableId]++;
            else
                counts->falseCounts[variableId]++;
            if (getVarState(variableId) == UNK) allDecided = false;
        }
    }
    const int mostFrequentLiteral = getMostFrequentLiteral(counts);
    insertDecisionLevel(abs(mostFrequentLiteral)-1, mostFrequentLiteral > 0 ? TRUE : FALSE);
    freeCounts(counts);
    return allDecided ? ALL_ASSIGNED : FOUND_VAR;
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

