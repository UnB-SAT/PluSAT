#ifndef __DPLL__
#define __DPLL__

#include "formula.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct Decision
{
    VariableId id;
    uint16_t value;
    bool flipped;
    struct DecisionList *consequences;
}Decision;

typedef struct DecisionList
{
    Decision *decision;
    struct DecisionList *next;
}DecisionList;

enum DecideState {ALL_TRIED, FOUND_VAR, ALL_ASSIGNED};
enum SolverResult {UNSAT, SAT};

/*
 * Decide a literal and set them on the level system
 * This function also have a tracking about all values
 * tried is this level
 */
enum DecideState Decide(const Form*);

void initDecisionLevels(const int);
void cleanDecisionLevels();

enum SolverResult dpll(Form *problem);
void dpllRecursive(Form *problem);

bool BCP(Form *, const Decision);

void insertDecisionLevel(const VariableId, const int);
void removeLastDecision();
Decision* getLastDecision();

int getLevel();

enum LiteralStates getLitState(LiteralId);
void setVarState(VariableId, LitState);
LitState getVarState(const VariableId var);

int resolveConflict();
void debugDecision();

Decision* getDecisions();

ClauseNode* getLiteralClauses(const LiteralId lit, const Form* form);

void backtrackTo(uint16_t);

#endif
