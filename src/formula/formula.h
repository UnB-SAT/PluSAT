#ifndef __FORM__
#define __FORM__

#include <stdint.h>

#define LIT_TABLE_SIZE 100

typedef enum LiteralStates{FALSE, TRUE, UNK} LitState;

typedef int16_t LiteralId;
typedef int16_t VariableId;

typedef struct Clause
{
    uint8_t size;
    LiteralId* literals;
}Clause;

typedef struct Node
{
    Clause* clause;
    struct Node* next;
}Node;

typedef struct VariableTree
{
    Node* trueLitClauses;
    Node* falseLitClauses;
}VariableTree;

typedef struct Form
{
    int16_t numClauses;
    int16_t numVars;
    Clause** clauses;
    VariableTree* variables;
    int *decisions;

}Form;
// Variable Tree
Node* addOnList(Node*, Node*);
Node* newNode(Clause* clause);
void freeList(Node*);

// Handle Clauses
Clause* newClause(LiteralId*, uint8_t);
void freeClause(Clause*);
void addClause(Form*, Clause*);

// Handle form
Form* newForm(uint16_t);
void freeForm(Form*);

#endif
