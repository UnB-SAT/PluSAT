#ifndef __FORMULA__
#define __FORMULA__

#include <stdint.h>

#define LIT_TABLE_SIZE 100

enum LiteralStates{FALSE, TRUE, UNK};

typedef int16_t LiteralId;

typedef struct Clause
{
    uint8_t size;
    LiteralId* variables;
}Clause;

typedef struct VarEntry
{
    LiteralId id;
    Clause *clause;
}VarEntry;

typedef struct Formula
{
    uint16_t numClauses;
    LiteralId* variables;
}Formula;

LiteralId* litTable;

Clause* newClause(LiteralId*, uint8_t);
void initLiteralTable();
void freeClause(Clause*);
void addClause(Formula*, Clause*);
void disableClause(Formula*, Clause*);
void removeClause(Formula*, Clause*);
void addClauseOnTable(LiteralId, Clause*);

#endif
