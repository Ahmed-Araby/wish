#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "utils.h"
#include "built-in-funs.h"
#include "shared.h"

char* builtinfnames[] = {"cd", "path", "exit", "clear", "cls"};
void (*builtinfptrs []) (struct program*) = {&cd, &path, &myexit, &clear, &clear};

extern char* cwd;
extern int cwdl;
extern char** spath;
extern int spathmaxl;
extern int spathc;

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

void* getBuiltInFuncPtr(char* pname){
    for(int i=0; i < BUILT_IN_FUNCTIONS_CNT; i++) {
        if(strcmp(pname, builtinfnames[i]) == 0)
            return builtinfptrs[i];
    }
    return NULL;
}


/**
 * @brief 'e3rdPB' stands for execute 3rd party binary, 
 * should carry on the logic needed to execute 3rd party binary
 * 
 * @param program
 * @return int, child pid
 */
int e3rdPB(struct program* program){
    // [todo] we need to make sure this binary exist before forking
    enum ptype pt = ppathtype(program->name);
    if(pt == INVALID){
        printf("error: invlaid 3rd party binary path \n");
        return -1;
    }
    char *ppath = program->name;

    if(
        (pt == ABSOLUTE  || pt == RELATIVE) &&
        access(program->name, R_OK | X_OK) == -1
    ){
        printf("error: can not execute '%s' program \n", program->name);
        return -1;
    }
    else if(pt == CLUELESS){
        ppath = NULL;
        getpabspath(&ppath, program->name);
        if(ppath == NULL){
            printf("error: can not execute '%s' program \n", program->name);
            return -1;
        }
        if(isdebugmode)
            printf("3rd party binary executable path = %s \n", ppath);
    }

    int rc = fork();
    if(rc  == -1){
        printf("error: failed to excute '%s' program \n", program->name);
        return -1;
    }
    else if(rc == 0){
        // child process
        if(program->outstream != NULL){
            close(STDOUT_FILENO);
            open(program->outstream, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU); // the mode flag give the current user rw and execute permission
        }
        
        /**
         * @brief refactor this part
         * to hold the program name into argv of the struct program, 
         * also think about doing all the path type and absolute path stuff in the parser.
         */
        char *pname = NULL;
        getpname(&pname, ppath);
        if(pname == NULL){
            printf("error: failed to get program name out of %s \n", ppath);
            exit(1);
        }
        char **argv = malloc((program->argc + 1) * sizeof(char*));
        argv[0] = pname;
        for(int i=0; i<program->argc; i++)
            argv[i+1] = program->argv[i];
        execv(ppath, argv);
        printf("error: failed to execv '%s' program \n", program->name);
        exit(1);
    }
    else {
        // parent process (wish)
        return rc;
    }
}

void execute(struct program** programs, int programscnt)
{
    uint* childpids = malloc(programscnt * sizeof(uint));
    int forkscnt = 0;
    
    for(int i=0; i<programscnt; i++)
    {
        void (*builtinfunc) (struct program *) = getBuiltInFuncPtr(programs[i]->name);
        if(builtinfunc != NULL){
            // built in function in the shell
            builtinfunc(programs[i]);
        }
        else{
            // 3rd party bianry
            int cpid = e3rdPB(programs[i]);
            if(cpid > 0){
                childpids[forkscnt] = cpid;
                forkscnt++;
            }
        }
    }

    // wait
    for(int i=0; i<forkscnt; i++){
        int rc = waitpid(childpids[i], NULL, 0);
        if(rc == -1){
            printf("error: failed to wait for %s program \n", programs[i]->name);
        }
    }
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
        printf("\033[1;32m"); // green
        printf("%s$ ", cwd);
        printf("\033[0m"); // default
        int csize = getline(&buffer, &bsize, stdin);
        if(csize == -1){
            printf("error: failed to read command line \n"); 
            continue;
        }     
        buffer[csize-1] = '\0'; // remove line feed (10)
        trim(buffer);  
        if(isdebugmode)
            printf("command = %s , size = %d \n", buffer, csize);

        // parser
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

        // executer
        execute(programs, programscnt);

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


void init(){
    // cwd
    cwdl = 1000;
    cwd = malloc((cwdl + 1) * sizeof(char));
    getcwd(cwd, cwdl);
    
    // search path
    spathmaxl = 1000;
    spath = malloc(spathmaxl * sizeof(char*));
    
    /**
     * strings within the double quotes are not located in the heap, 
     * but after the strcpy we get copy of them into the heap
     */
    spathc = 0;
    spath[spathc] = malloc((strlen("/usr/bin") + 1 ) * sizeof(char));
    strcpy(spath[spathc], "/usr/bin");

    spathc++;
    spath[spathc] = malloc((strlen("/bin") + 1 ) * sizeof(char));
    strcpy(spath[spathc], "/bin");
    
    spathc++;
    spath[spathc] = malloc((strlen("/usr") + 1 ) * sizeof(char));
    strcpy(spath[spathc], "/usr");

    spathc++;
}

void destroy(){
    // we need to free the cwd and search path memory 
    // (however this will be on exit, so OS will do it for us)
}

int main(int argc, char *argv[]){
    if(argc > 2){
        printf("error: wish can only run in interactive mode with no arguments or batch mode with file name as the only argument \n");
        exit(1);
    }
    init();
    if(argc == 1)
        interactive();
    else
        batch();
    return 0;
}
