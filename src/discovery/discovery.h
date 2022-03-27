#ifndef __DISCOVERY__
#define __DISCOVERY__

#include "formula.h"
#include "dpll.h"

typedef enum DecideState (*DecideHook)(const Form*);
typedef bool (*BCPHook)(Form*, const Decision);

typedef struct Hooks
{
    DecideHook decideHook;
    BCPHook bcpHook;
}Hooks;

void loadHooks(char *);
Hooks* getHooks();

#endif