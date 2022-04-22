#ifndef __DISCOVERY__
#define __DISCOVERY__

#include "formula.h"
#include "dpll.h"

typedef enum DecideState (*DecideHook)(const Form*);
typedef bool (*BCPHook)(Form*, const Decision);
typedef bool (*ResolveConflictHook)();
typedef void (*PreProcessingHook)(Form*);

typedef struct Hooks
{
    DecideHook decideHook;
    BCPHook bcpHook;
    ResolveConflictHook conflictHook;
    PreProcessingHook preProcessing;
}Hooks;

void loadHooks(char *);
Hooks* getHooks();

#endif
