#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"
#include "shared.h"

extern char* cwd;
extern int cwdl;
extern char** spath;
extern int spathmaxl;
extern int spathc;

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
    if(p->argc == 0){
        for(int i=0; i<spathc; i++){
            printf("%s", spath[i]);
            if(i+1 < spathc)
                printf(":");
            else 
                printf("\n");
        }
        return;
    }
    if(p->argc != 1){
        printf("error: invlaid number of arguments for 'path'");
        return;
    }
    char* arg = p->argv[0];
    char* token = strsep(&arg, COLON); // should be $PATH
    if(arg == NULL){
        spathc = 0;    
        arg = token;    
    }
    else{
        char * PATH = "$PATH";
        if(strcmp(token, PATH) !=0){
            printf("error: invalid token \n");
            return;
        }
    }
    while(arg != NULL && strcmp(arg, "\0") !=0){
        char *dir = strsep(&arg, COMMA);
        spath[spathc] = malloc((strlen(dir) + 1) * sizeof(char));
        strcpy(spath[spathc], dir);
        spathc++;
    }
}