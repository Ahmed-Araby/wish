#include <stdlib.h>

#ifndef UTILS_H
#define UTILS_H

#define LINE_FEED 10
#define MAX_PARALLEL_PROGRAMS 10
#define PARALLEL_EXECUTION_OPERATOR "&"
#define REDIRECTION_OPERATOR ">"
#define SPACE " "
#define COLON ":"
#define COMMA ","
#define MAX_ARGS_PER_PROGRAM 10 
#define BUILT_IN_FUNCTIONS_CNT 3
#define isdebugmode 0

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

uint trim(char*);
void deallocateprogram(struct program*);

#endif 