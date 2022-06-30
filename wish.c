#include <stdio.h>
#include <stdlib.h>

void interactive();
void batch();

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
        // parse the command
        //
    }
    free(buffer);
}

void batch(){
    printf("batch mode is not implemented yet \n");
}