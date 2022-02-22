#ifndef __DPLL__
#define __DPLL__

#include "formula.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct Decision
{
    VariableId id;
    uint16_t value;
    int flipped;
}Decision;

void dpll(Form *problem);
void dpllRecursive(Form *problem);

void initDecisionLevels(int);
void cleanDecisionLevels();
bool BCP(Form *, const Decision);
VariableId Decide(const Form *);

#endif
