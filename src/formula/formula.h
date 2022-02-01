#ifndef __FORM__
#define __FORM__

#include <stdint.h>

#define LIT_TABLE_SIZE 100

enum LiteralStates{FALSE, TRUE, UNK};

typedef int16_t LiteralId;
typedef int16_t VariableId;

typedef struct Clause
{
    uint8_t size;
    LiteralId* variables;
}Clause;

typedef struct Form
{
    uint16_t numClauses;
    Clause** clauses;
}Form;

// Handle Clauses
Clause* newClause(LiteralId*, uint8_t);
void freeClause(Clause*);

// Handle form
Form* newForm();
void freeForm(Form*);

// nothing now
void addClause(Form*, Clause*);
void initLiteralTable();
void disableClause(Form*, Clause*);
void removeClause(Form*, Clause*);
void addClauseOnTable(LiteralId, Clause*);

#endif
