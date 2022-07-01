#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

uint isdebugmode = 0;

void interactive();
void batch();

/**
 * @brief 
 * program is an abstraction of the compution that the user need to do,
 * it could refer to 3rd party binary that is expected to be located
 * in the search path of the shell, 
 * or a built in function supported by the shell.
 */

struct program{
    char* name;
    uint argc;
    char** argv;

    /**
     * @brief 
     * if null output should be written into the standard output stream.
     * other wise the string will be considered as the file name to, 
     * redirect the output to.
     */
    char* outstream; 
};

void deallocateprogram(struct program *p){
    if(isdebugmode)
        printf("free memory for %s program \n", p->name);
    if(p->outstream != NULL)
        free(p->outstream);
    if(p->name != NULL)
        free(p->name);    
    if(p->argv != NULL){
        for(int i=0; i<p->argc; i++){
            free(p->argv[i]);
        }
    }
    p->argc = 0;
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

void interactive(){
    char *buffer;
    uint bsize = 1000; // max command of 1k-1 char 
    buffer = malloc(bsize * sizeof(char));
    if(buffer == NULL){
        printf("error: there is no enough space to allocate buffer for reading commands \n");
        exit(1);
    }

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
        /**
         * @brief Parser
         * 1- search for the special operators supported by the shell.
         *      1- parallel execution operator [first] "&".
         *      2- redirection operation [second] ">".
         * 2- search for the program name and arguments.
         */

        struct program programs[MAX_PARALLEL_PROGRAMS];
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
            struct program p;
            p.name = p.outstream = p.argv = NULL;
            p.argc = 0;
            if(scommand != NULL){
                char *outstream = scommand; // just for convience
                trim(outstream);
                p.outstream = malloc( (strlen(outstream) + 1) * sizeof(char));
                if(p.outstream == NULL){
                    printf("error: failed to allocate memory for the outstream string name \n");
                    exit(1);
                }
                strcpy(p.outstream, outstream);
                if(isdebugmode)
                    printf("outstream = %s, length = %d \n", p.outstream, strlen(p.outstream));
            }
            // 3- extract program name and program arguments
            // 3.1- extract program name
            if(isdebugmode)
                printf("program command = %s, length = %d \n", sprogram, strlen(sprogram));
            char *pName = strsep(&sprogram, SPACE);
            trim(pName);
            p.name = malloc( (strlen(pName) + 1) * sizeof(char) );
            strcpy(p.name, pName);
            if(isdebugmode)
                printf("program name = %s, lenght = %d \n", p.name, strlen(p.name));
            // 3.2- extract program arguments
            p.argv = malloc(MAX_ARGS_PER_PROGRAM * sizeof(char*));
            p.argc = 0 ;
            for(int i=0; i< MAX_ARGS_PER_PROGRAM; i++){
                p.argv[i] = NULL;
            }

            char *pargs = sprogram; // for convience
            while(pargs != NULL){
                trim(pargs);
                char *arg = strsep(&pargs, SPACE);
                trim(arg);
                p.argv[p.argc] = malloc(( strlen(arg) + 1) * sizeof(char));
                strcpy(p.argv[p.argc], arg);
                p.argc +=1;
            }
            if(isdebugmode){
                printf("p.argc = %d \n", p.argc);
                for(int i=0; i<p.argc; i++){
                    printf("p.argv[i] = %s \n", p.argv[i]);
                }
            }
            programs[programscnt] = p;
            programscnt +=1;
        }   

        if(isdebugmode){
            for(int i=0; i<programscnt; i++){
                printf("program #%d \n", i);
                printf("p.name = %s \n", programs[i].name);
                if(programs[i].outstream != NULL){
                    printf("p.outstream = %s \n", programs[i].outstream);
                }

                if(programs[i].argv != NULL){
                    printf("has arguments \n");
                    printf("p.argc = %d \n", programs[i].argc);
                    for(int j=0; j<programs[i].argc; j++){
                        printf("arg #%d = %s\n", j, programs[i].argv[j]);
                    }
                }
            }
        }
        
        /**
         * @brief Execution
         */
        

        // free programs
        for(int i=0; i<programscnt; i++){
            deallocateprogram(&programs[i]);
        }
    }

    free(buffer);
}

void batch(){
    printf("batch mode is not implemented yet \n");
}