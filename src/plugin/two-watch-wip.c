#include "dpll.h"
#include "formula.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct WatchList {
    ClauseNode** watchers;
} WatchList;

static WatchList* watchList = NULL;

void initWatchList(Form* form) {
    int totalLits = 2 * form->numVars;
    watchList = (WatchList*) malloc(sizeof(WatchList));
    watchList->watchers = (ClauseNode**) calloc(totalLits, sizeof(ClauseNode*));

    ClauseNode* node = form->clauses;
    while (node != NULL) {
        Clause* clause = node->clause;
        if (clause->size > 0) {
            LiteralId first = clause->literals[0];
            uint16_t pos = getPos(first);
            watchList->watchers[pos] = addNodeOnList(clause, watchList->watchers[pos]);
        }
        node = node->next;
    }
}

enum DecideState Decide(const Form* form) {
    int maxCount = -1;
    LiteralId bestLit = 0;
    int* posCounts = (int*) calloc(form->numVars + 1, sizeof(int));
    int* negCounts = (int*) calloc(form->numVars + 1, sizeof(int));

    ClauseNode* node = form->clauses;
    while (node != NULL) {
        Clause* clause = node->clause;
        for (int i = 0; i < clause->size; ++i) {
            LiteralId lit = clause->literals[i];
            VariableId var = abs(lit);
            if (getVarState(var - 1) == UNK) {
                if (lit > 0)
                    posCounts[var]++;
                else
                    negCounts[var]++;
            }
        }
        node = node->next;
    }

    for (int i = 1; i <= form->numVars; ++i) {
        if (getVarState(i - 1) != UNK) continue;

        if (posCounts[i] >= negCounts[i] && posCounts[i] > maxCount) {
            maxCount = posCounts[i];
            bestLit = i;
        } else if (negCounts[i] > maxCount) {
            maxCount = negCounts[i];
            bestLit = -i;
        }
    }

    free(posCounts);
    free(negCounts);

    if (maxCount == -1) return ALL_ASSIGNED;

    insertDecisionLevel(abs(bestLit) - 1, bestLit < 0 ? FALSE : TRUE);
    return FOUND_VAR;
}

bool BCP(Form *formula, const Decision decision) {
    LiteralId falseLit = (decision.value == FALSE) ? (decision.id + 1) : -(decision.id + 1);
    ClauseNode* head = formula->literals[getPos(falseLit)];

    while (head != NULL) {
        Clause* clause = head->clause;
        int unassignedCount = 0;
        LiteralId lastUnassigned = 0;
        bool clauseSatisfied = false;

        for (int i = 0; i < clause->size; i++) {
            LiteralId lit = clause->literals[i];
            LitState state = getLitState(lit);

            if (state == TRUE) {
                clauseSatisfied = true;
                break;
            } else if (state == UNK) {
                unassignedCount++;
                lastUnassigned = lit;
            }
        }

        if (!clauseSatisfied) {
            if (unassignedCount == 0) {
                return false;
            } else if (unassignedCount == 1) {
                VariableId var = abs(lastUnassigned) - 1;
                bool value = (lastUnassigned > 0) ? TRUE : FALSE;
                setVarState(var, value);
                insertDecisionLevel(var, value);
            }
        }

        head = head->next;
    }

    return true;
}

int resolveConflict() {
    Decision* decisions = getDecisions();
    int i = getLevel() - 1;
    for (; i >= 0; --i)
        if (!decisions[i].flipped)
            break;
    return i + 1;
}

void PreProcessing(Form* form) {
    initWatchList(form);
}
