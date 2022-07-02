#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "built-in-funs.h"

char* builtinf[] = {"cd", "path", "exit"};
void (*builtinfptr []) (struct program*) = {&cd, &path, &myexit};

int parse(char* buffer, struct program** programs, int maxparalleprograms)
{
    /**
     * @brief Parser
     * 1- search for the special operators supported by the shell.
     *      1- parallel execution operator [first] "&".
     *      2- redirection operation [second] ">".
     * 2- search for the program name and arguments.
     */
    uint programscnt = 0;
    char *bufferc = buffer; // preserve the buffer pointer for future getline() invokes and to free the buffer at the end

    // 1- look for parallel execution operator.
    while(bufferc != NULL){
        char *scommand = strsep(&bufferc, PARALLEL_EXECUTION_OPERATOR);
        trim(scommand);
        trim(bufferc);
        if(isdebugmode){
            printf("single command = %s, length = %d \n", scommand, strlen(scommand));
            printf("remaining of the parallel execution commands = %s \n", bufferc);
        }
        // 2- look for the redirection operator
        // after this step scommand will contain the file of the redirectio operator.
        char *sprogram = strsep(&scommand, REDIRECTION_OPERATOR);
        trim(sprogram);
        trim(scommand);
        if(sprogram == NULL){
            printf("error: a program must be specifed \n");
            exit(1);
        }

        // construct the program
        struct program* p = malloc(sizeof(struct program));
        p->name = NULL;
        p->outstream = NULL;
        p->argv = NULL;
        p->argc = 0;
        if(scommand != NULL){
            char *outstream = scommand; // just for convience
            trim(outstream);
            p->outstream = malloc( (strlen(outstream) + 1) * sizeof(char));
            if(p->outstream == NULL){
                printf("error: failed to allocate memory for the outstream string name \n");
                exit(1);
            }
            strcpy(p->outstream, outstream);
            if(isdebugmode)
                printf("outstream = %s, length = %d \n", p->outstream, strlen(p->outstream));
        }
        // 3- extract program name and program arguments
        // 3.1- extract program name
        if(isdebugmode)
            printf("program command = %s, length = %d \n", sprogram, strlen(sprogram));
        char *pName = strsep(&sprogram, SPACE);
        trim(pName);
        p->name = malloc( (strlen(pName) + 1) * sizeof(char) );
        strcpy(p->name, pName);
        if(isdebugmode)
            printf("program name = %s, lenght = %d \n", p->name, strlen(p->name));
        // 3.2- extract program arguments
        p->argv = malloc(MAX_ARGS_PER_PROGRAM * sizeof(char*));
        p->argc = 0 ;
        for(int i=0; i< MAX_ARGS_PER_PROGRAM; i++){
            p->argv[i] = NULL;
        }

        char *pargs = sprogram; // for convience
        while(pargs != NULL){
            trim(pargs);
            char *arg = strsep(&pargs, SPACE);
            trim(arg);
            p->argv[p->argc] = malloc(( strlen(arg) + 1) * sizeof(char));
            strcpy(p->argv[p->argc], arg);
            p->argc +=1;
        }
        if(isdebugmode){
            printf("p.argc = %d \n", p->argc);
            for(int i=0; i<p->argc; i++){
                printf("p.argv[i] = %s \n", p->argv[i]);
            }
        }
        programs[programscnt] = p;
        programscnt +=1;
    }   

    return programscnt;
}
void interactive(){
    char *buffer;
    uint bsize = 1000; // max command of 1k-1 char 
    buffer = malloc(bsize * sizeof(char));
    if(buffer == NULL){
        printf("error: there is no enough space to allocate buffer for reading commands \n");
        exit(1);
    }
    struct program** programs = malloc(MAX_PARALLEL_PROGRAMS * sizeof(struct program*));

    while(1){
        printf("wish> ");
        int csize = getline(&buffer, &bsize, stdin);
        if(csize == -1){
            printf("error: failed to read command line \n"); 
            continue;
        }     
        buffer[csize-1] = '\0'; // remove line feed (10)
        trim(buffer);  
        if(isdebugmode)
            printf("command = %s , size = %d \n", buffer, csize);

        int programscnt = parse(buffer, programs, MAX_PARALLEL_PROGRAMS);
        if(isdebugmode){
            for(int i=0; i<programscnt; i++){
                printf("program #%d \n", i);
                printf("p.name = %s \n", programs[i]->name);
                if(programs[i]->outstream != NULL){
                    printf("p.outstream = %s \n", programs[i]->outstream);
                }

                if(programs[i]->argv != NULL){
                    printf("has arguments \n");
                    printf("p.argc = %d \n", programs[i]->argc);
                    for(int j=0; j<programs[i]->argc; j++){
                        printf("arg #%d = %s\n", j, programs[i]->argv[j]);
                    }
                }
            }
        }
            
        /**
         * @brief Execution
         */
        builtinfptr[0](programs[0]);

        // free programs
        for(int i=0; i<programscnt; i++){
            deallocateprogram(programs[i]);
            free(programs[i]);
            programs[i] = NULL;
        }
    }

    free(buffer);
    free(programs);
}


void batch(){
    printf("batch mode is not implemented yet \n");
}

int main(int argc, char *argv[]){
    if(argc > 2){
        printf("error: wish can only run in interactive mode with no arguments or batch mode with file name as the only argument \n");
        exit(1);
    }
    if(argc == 1)
        interactive();
    else
        batch();
    return 0;
}
