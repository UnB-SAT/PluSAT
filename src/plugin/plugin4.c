#include <limits.h>

#include "dpll.h"
#include "formula.h"

#include <stdio.h>
#include <stdlib.h>

#include "../dpll.h"

void PreProcessing(Form* form){
}

typedef struct {
    int positiveCount;
    int negativeCount;
} LiteralCounts;
typedef struct {
    int variableCount; // Redundante
    LiteralCounts *literal_counts;
} DLISCounts;

/* Given a variable "v", if the most frequent literal is
 * ~v, the function returns -v, otherwise it returns
 * positive v. Literals are 1 indexed
 */
int getMostFrequentLiteral(const DLISCounts* counts) {
    if (counts == NULL) {
        fprintf(stderr, "getMostFrequentLiteral: NULL DLISCounts\n");
        abort();
    }
    if (counts->literal_counts == NULL) {
        fprintf(stderr, "Null pointer in DLIS true literal counts");
        abort();
    }
    int maxCount = INT_MIN;
    int maxLiteral = INT_MIN;
    for (size_t i = 0; i < counts->variableCount; i++) {
        if (counts->literal_counts[i].positiveCount > maxCount) {
            maxCount = counts->literal_counts[i].positiveCount;
            maxLiteral = i+1;
        } else if (counts->literal_counts[i].negativeCount > maxCount) {
            maxCount = counts->literal_counts[i].negativeCount;
            maxLiteral = -(i+1);
        }
    }
    return maxLiteral;
}

DLISCounts* initCounts(const int numVars) {
    DLISCounts* counts = malloc(sizeof(DLISCounts));
    counts->variableCount = numVars;
    counts->literal_counts = calloc(numVars, sizeof(LiteralCounts));
    return counts;
}

void freeCounts(DLISCounts* counts) {
    if (counts == NULL) {
        fprintf(stderr, "NULL DLISCounts when freeing\n");
        abort();
    }
    free(counts->literal_counts);
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
                counts->literal_counts[variableId].positiveCount++;
            else
                counts->literal_counts[variableId].negativeCount++;
            if (getVarState(variableId) == UNK) allDecided = false;
        }
    }
    /* NOTE: Era para ter uma lista de variáveis e suas
     * decisões para não ter que iterar por todas as cláusulas
     * para verificar se todas as variáveis já foram decididas
     */
    if (allDecided) {
        freeCounts(counts);
        return ALL_ASSIGNED;
    }

    const int mostFrequentLiteral = getMostFrequentLiteral(counts);
    insertDecisionLevel(abs(mostFrequentLiteral)-1, mostFrequentLiteral > 0 ? TRUE : FALSE);
    freeCounts(counts);
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

