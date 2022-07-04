#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "shared.h"

extern char* cwd;
extern int cwdl;

void myexit(struct program* p){
    exit(0);
}

void cd(struct program* p){
    if(cwd == NULL){
        printf("error: cwd is null");
        exit(1);
    }
    if(p->argc != 1){
        printf("error: invalid number of arguments \n");
        return;
    }
    int ret = chdir(p->argv[0]);
    if(ret == -1)
        printf("error: failed to change the cwd \n");
    else 
        getcwd(cwd, cwdl);
}

void path(struct program* p){
    printf("build-in path function not implemented yet \n");
}