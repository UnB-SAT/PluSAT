#include "dpll.h"
#include "formula.h"

#define INITIAL_DECISION_VALUE 0
#define MAX_CAPACITY 10

typedef struct Decision
{
    int variableIndex;
    int value;
    int forced;
}Decision;

Decision *newDecision(int varIndex, int value, int forced)
{
    Decision *aux = (Decision*)malloc(sizeof(Decision));

    aux->variableIndex = varIndex;
    aux->value = value;
    aux->forced = forced;

    return aux;
}

static int count = 0;

int Decide(Form *problem)
{
    if(count < problem->varSize)
    {
        return count++;
    }
    else
    {
        return -1;
    }
}

int unDecide(Form *problem)
{

    if(count > 0)
        return count--;

    return 0;
}

typedef struct DecisionStack
{
    Decision array[MAX_CAPACITY];
    int size;
    int capacity;
}DecisionStack;

DecisionStack *createStack()
{
    DecisionStack *aux = (DecisionStack*)malloc(sizeof(DecisionStack));

    aux->size = 0;
    aux->capacity = MAX_CAPACITY;

    return aux;
}

void Stack_InsertDecision(DecisionStack* stack, int variable, int value)
{
    stack->array[stack->size].value = value;
    stack->array[stack->size].variableIndex = variable;
    stack->size++;
}

Decision Stack_PopTop(DecisionStack* stack)
{
    return stack->array[stack->size--];
}

DecisionStack *level;

void initLevel()
{

    printf("---------");
    DecisionStack *stack = createStack();

    printf("asdfjaçlksjf");

    level = stack;
}

void dpllRecursive(Form* problem)
{

    //bcp 
    
    int var = Decide(problem);

    if(var !=-1){

        Stack_InsertDecision(level, var, TRUE);
        printf("A var %d value %d\n", var, TRUE);
        dpllRecursive(problem);
        Stack_PopTop(level);
        printf("R var %d value %d\n", var, TRUE);

        Stack_InsertDecision(level, var, FALSE);
        printf("A var %d value %d\n", var, FALSE);
        dpllRecursive(problem);
        Stack_PopTop(level);
        printf("R var %d value %d\n", var, FALSE);

        unDecide(problem);

    }


}

void dpll(Form* problem)
{

    //This section has a allocation problem
    //Decision* stack = (Decision*)malloc(sizeof(Decision)*problem->varSize);
    
    DecisionStack *level = createStack();
    DecisionStack *stack = createStack();

    Decision top;

    //choose a variable
    int var = Decide(problem);

    Stack_InsertDecision(stack, var, 0);
    Stack_InsertDecision(stack, var, 1);

    while(stack->size)
    {
        top = Stack_PopTop(stack);

        Stack_InsertDecision(level, top.variableIndex, top.value);

        //verifica se ta tudo correto (bcp propagation etc)

        var = Decide(problem);

        printf("var: %d\n", var);

        if(var != -1)
        {
            Stack_InsertDecision(stack, var, 0);
            Stack_InsertDecision(stack, var, 1);
        }

    }

}
 
