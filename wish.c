#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// definitions
#define LINE_FEED 10
#define MAX_PARALLEL_PROGRAMS 10
#define PARALLEL_EXECUTION_OPERATOR "&"
#define REDIRECTION_OPERATOR ">"
#define SPACE " "
//

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
        printf("command = %s , size = %d \n", buffer, csize);
        printf("line feed = %d \n", LINE_FEED);
        /**
         * @brief parsing
         * 1- search for the special operators supported by the shell.
         *      1- parallel execution operator [first] "&".
         *      2- redirection operation [second] ">".
         * 2- search for the program name and arguments.
         */

        struct program programs[MAX_PARALLEL_PROGRAMS];
        uint programscnt = 0;
        char *bufferc = buffer;

        // 1- look for parallel execution operator.
        while(bufferc != NULL){
            char *scommand = strsep(&bufferc, PARALLEL_EXECUTION_OPERATOR);
            printf("token = %s, length = %d \n", scommand, strlen(scommand));
            printf("remaining of the command = %s \n", bufferc);

            // 2- look for the redirection operator
            // after this step scommand will contain the file of the redirectio operator.
            char *sprogram = strsep(&scommand, REDIRECTION_OPERATOR);
            if(sprogram == NULL){
                printf("error: a program must be specifed \n");
                exit(1);
            }

            // construct the program
            struct program p;
            if(scommand != NULL){
                char *outstream = scommand; // just for convience
                p.outstream = malloc( (strlen(outstream) + 1) * sizeof(char));
                if(p.outstream == NULL){
                    printf("error: failed to allocate memory for the outstream string name \n");
                    exit(1);
                }
                strcpy(p.outstream, outstream);
                printf("outstream = %s, length = %d \n", p.outstream, strlen(p.outstream));
            }
        }
    }
    free(buffer);
}

void batch(){
    printf("batch mode is not implemented yet \n");
}