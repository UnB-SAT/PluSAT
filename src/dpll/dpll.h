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
}Decision;

enum DecideState {ALL_TRIED, FOUND_VAR, ALL_ASSIGNED};

/*
 * Decide a literal and set them on the level system
 * This function also have a tracking about all values
 * tried is this level
 */
enum DecideState Decide(const Form*);

bool resolveConflict();

void initDecisionLevels(const int);
void cleanDecisionLevels();


enum SolverResult {UNSAT, SAT};

enum SolverResult dpll(Form *problem);
void dpllRecursive(Form *problem);

bool BCP(Form *, const Decision);

void insertDecisionLevel(const VariableId, const int);

bool resolveConflict();

#endif
