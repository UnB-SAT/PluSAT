#ifndef __FORM__
#define __FORM__

#include <stdint.h>

// CLAUSES

typedef enum LiteralStates{FALSE, TRUE, UNK} LitState;

typedef int16_t LiteralId;
typedef int16_t VariableId;

typedef struct Clause{
    uint8_t size;
    LiteralId* literals;
}Clause;

struct Node
{
    Clause* clause;
    struct Node* next;
};

typedef struct Node  ClauseNode;

Clause* newClause(LiteralId*, uint8_t);
void freeClause(Clause*);

// FORM

typedef struct Form
{
    int16_t numClauses;
    int16_t numVars;

    // Clauses to be cleaned
    ClauseNode* clauses;

    // Clause list indexed by literal 
    // positive 2*n and negative 2*n+1
    ClauseNode** literals;

}Form;

Form* newForm(uint16_t);
void freeForm(Form*);

void addClause(Clause*, Form*);

ClauseNode* addNodeOnList(Clause*, ClauseNode*);
void freeList(ClauseNode*);

// Create ClauseNode
ClauseNode *newNode(Clause*);
uint16_t getPos(const LiteralId);

void PreProcessing(Form*);

#endif
