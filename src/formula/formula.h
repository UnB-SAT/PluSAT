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

typedef struct Node
{
    Clause* clause;
    struct Node* next;
}Node;

typedef struct VariableTree
{
    Node *trueLitClauses;
    Node *falseLitClauses;
}VariableTree;

typedef struct Form
{
    uint16_t numClauses;
    Clause** clauses;
    VariableTree *variables;
}Form;

// Handle Clauses
Clause* newClause(LiteralId*, uint8_t);
void freeClause(Clause*);
void addClause(Form*, Clause*);

// Handle form
Form* newForm(uint16_t);
void freeForm(Form*);

#endif
