#include "formula.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This module tries to implemente clause learning 
 * the approache is to create a reverse graph of implications 
 * (reverse because the implications reference the variables that start
 * the inference).
 * The purpose is to find all variables and return to the user, with that 
 * they can add a new clause base on that information, is not all the 
 * work but the objective is to be simple
*/
typedef struct GraphNode{
    VariableId id; 
    struct GraphNode *next;
}GraphNode;

// start graphql
GraphNode** initDependencyGraph(uint32_t numVars){
    GraphNode **graph = calloc(sizeof(GraphNode*), numVars);

    for(int i = 0; i<numVars; ++i)
        graph[i] = NULL;

    return graph;
}

// alloc things
GraphNode* ClauseLearning_AllocNode(const VariableId id){
    GraphNode* node = malloc(sizeof(GraphNode));

    node->id = id;
    node->next = NULL;


    return node;
}

// Create a node and transform to the first item on the list
GraphNode* insertNodeList(GraphNode *list, VariableId id)
{
    GraphNode *newNode = ClauseLearning_AllocNode(id);
    newNode->next = list;

    return newNode;
}

static int getVarPos(const VariableId var){
    return var -1;
}

// receives de var and the list of references that implicates the conflict
void ClauseLearning_Inference(const VariableId var, GraphNode** depGraph, VariableId* reference, const int refsSize){
    GraphNode* node = ClauseLearning_AllocNode(var);

    for(int refPos = 0; refPos < refsSize; ++refPos){
        depGraph[getVarPos(var)] = insertNodeList(depGraph[getVarPos(var)], reference[refPos]);
    }
}

// just a wrapper to populate variables, seeing that they don't have implications
void ClauseLearning_DecideVariable(const VariableId var, GraphNode** depGraph){
    ClauseLearning_Inference(var, depGraph, NULL, 0);
}

int main() {

    GraphNode **graph = initDependencyGraph(5);

    ClauseLearning_DecideVariable(5, graph);
    ClauseLearning_DecideVariable(1, graph);

    ClauseLearning_Inference(2, graph, (VariableId[2]){5,1 } , 2);
    ClauseLearning_Inference(3, graph, (VariableId[1]){ 1 } , 1);
    ClauseLearning_Inference(4, graph, (VariableId[2]){ 2, 3 } , 2);

    VariableId queue[5];
    int qpos = 0;

    queue[qpos++] = 4;

    /*
    for(int i = 0; i < 5; ++i){
        GraphNode *list = graph[i];

        while(list != NULL){
            printf("%d ", list->id);
            list = list->next;
        }
        printf("\n");
    }
    */

    while(qpos > 0){

        printf("queue state\n");
        for(int i = 0; i < qpos; ++i)
            printf("%u ", queue[i]);
        printf("\n");

        GraphNode* clist = graph[getVarPos(queue[qpos-1])];
        qpos--;

        printf("%d -- %d\n", queue[qpos], clist != NULL);

        while(clist != NULL){
            printf("@@\n");
            queue[qpos++] = clist->id;
            clist = clist->next;
        }

    }

    return 0;
}
