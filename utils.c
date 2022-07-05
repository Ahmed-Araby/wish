#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
/**
 * @brief 
 * this method assume str has null byte terminator at the end.
 * @param str 
 * @return uint 
 */
uint trim(char *str){
    // handle special cases
    if(str == NULL)
        return -1;
    if(*str == '\0')
        return 0;
    
    // point ptr2 on the first non {white sapce, tap} byte character    
    char *strs = str;
    char *stre = str + strlen(str) -1;
    while(1){
        if(*strs == 32 || *strs == 9) // white space || horizontal tap
            strs ++;
        if(*stre == 32 || *stre == 9)
            stre --;
        if(
            !(*strs == 32 || *strs == 9) &&
            !(*stre == 32 || *stre == 9)
        ) 
            break;
    }

    // shift all chars to the left
    char *ptr = str;
    while(1){
        *ptr = *strs;
        if(strs == stre)
            break;
        strs ++;
        ptr ++;
    }

    // make all remaining bytes = null byte terminator
    ptr++; // post the last character we are interested in
    while(*ptr != '\0'){
        *ptr = '\0';
        ptr ++;
    }
    return strlen(str);
}

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

/**
 * @brief determine the type of the path for the 3rd party binary. 
 * @param pname 
 * @return enum ptype 
 */
enum ptype ppathtype(char* pname){
    if(pname == NULL)
        return INVALID;
    int len = strlen(pname);
    if(len == 0)
        return INVALID;
    
    if(
        len > 2 &&
        pname[0] == '.' && 
        pname[1] == '/'
    )
        return RELATIVE;
    else if(len > 1 && pname[0] == '/')
        return ABSOLUTE;
    else if(pname[0] !='.' && pname[0]!='/')
        return CLUELESS;
    return INVALID;
}


void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();

void test1(){
    printf("test1 :\n");
    char* str = malloc(5 * sizeof(char));
    str[0] = ' ';
    str[1] = 'a';
    str[2] = 'b';
    str[3] = ' ';
    str[4] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
    
}

void test2(){
    printf("test2: \n");
    char* str = malloc(3 * sizeof(char));
    str[0] = ' ';
    str[1] = 'a';
    str[2] = '\0';

    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
}

void test3(){
    printf("test3 :\n");
    char* str = malloc(11 * sizeof(char));
    str[0] = ' ';
    str[1] = ' ';
    str[2] = ' ';
    str[3] = ' ';
    str[4] = ' ';
    str[5] = ' ';
    str[6] = 'a';
    str[7] = 'b';
    str[8] = ' ';
    str[9] = ' ';
    str[10] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
}


void test4(){
    printf("test4 :\n");
    char* str = malloc(11 * sizeof(char));
    str[0] = ' ';
    str[1] = ' ';
    str[2] = ' ';
    str[3] = 'c';
    str[4] = ' ';
    str[5] = ' ';
    str[6] = 'a';
    str[7] = 'b';
    str[8] = ' ';
    str[9] = ' ';
    str[10] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
}

void test5(){
    printf("test5 :\n");
    char* str = malloc(11 * sizeof(char));
    str[0] = ' ';
    str[1] = ' ';
    str[2] = ' ';
    str[3] = 'c';
    str[4] = ' ';
    str[5] = ' ';
    str[6] = ' ';
    str[7] = ' ';
    str[8] = ' ';
    str[9] = ' ';
    str[10] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
}

void test6(){
    printf("test6 :\n");
    char* str = malloc(2 * sizeof(char));
    str[0] = 'c';
    str[1] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
    printf("str[newl-1] = %c \n", str[newl-1]);
}

void test7(){
    printf("test7 :\n");
    char* str = NULL;
    uint newl = trim(str);
    printf("newl = %d \n", newl);
}


void test8(){
    printf("test8 :\n");
    char* str = malloc(1 * sizeof(char));
    str[0] = '\0';
    uint newl = trim(str);
    printf("str = %s, strl = %d, newl = %d \n", str, strlen(str), newl);
    printf("str[0] = %c \n", str[0]);
}