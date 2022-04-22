#include "discovery.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>

Hooks *hooks = NULL;

void *loadFunction(void *libhandle, char* funcName){
    void *func = dlsym(libhandle, funcName);

    if(func == NULL)
    {
        printf("Function import ERROR\n");
        exit(1);
    }

    return func;
}


void loadHooks(char *sharedObjectPath)
{

    void *libhandle = dlopen(sharedObjectPath, RTLD_LAZY);

    hooks = malloc(sizeof(Hooks));

    if(libhandle == NULL){
       printf("%s\n", dlerror());
       exit(1);
    }

    hooks->decideHook = loadFunction(libhandle, "Decide");
    hooks->bcpHook = loadFunction(libhandle, "BCP");
    hooks->conflictHook = loadFunction(libhandle, "resolveConflict");
    hooks->preProcessing = loadFunction(libhandle, "PreProcessing");
}

Hooks* getHooks(){
    return hooks;
}
