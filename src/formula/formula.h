#ifndef __FORMULA__
#define __FORMULA__

#include <stdint.h>

enum LiteralStates{FALSE, TRUE, UNK};

typedef int16_t LiteralId;

typedef struct Clause
{
    uint8_t size;
    LiteralId* variables;
}Clause;

typedef struct Node
{
    LiteralId id;
    Clause *clause;
    struct Node *next;
    struct Node *prev;
}Node;

typedef struct Formula
{
    Node *variableTable;

    uint16_t numClauses;
    LiteralId* variables;
}Formula;

Clause* newClause(LiteralId*, uint8_t);
void freeClause(Clause*);

#endif
