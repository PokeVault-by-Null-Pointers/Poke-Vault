/*******************************************************************************
 * @file        Misc.c
 * @brief       Makes my job easier
 *
 * @author      John Rosenberger
 * @date        2026-07-15
 * @copyright   me
 *******************************************************************************/

#include <stdlib.h> //for malloc
#include <string.h> //for strlen
#include <ctype.h> //for toupper
#include "Misc.h" //so it can be used

//better strncpy
void copyString(char* output, const char* input, int size){
    if(output == NULL || input == NULL || size <= 0){
        return;
    }
    int i = 0;
    while( input[i] != '\0'   &&   i < (size-1) ){
        output[i] = input[i];
        i++;
    }
    output[i] = '\0';
}

//to get around case sensitivity
char* upperCase(const char* lowercase){
    int i = 0;
    char* UPPERCASE = malloc(sizeof(char) * (strlen(lowercase)+1));
    if(UPPERCASE == NULL){
        return NULL;
    }
    while(lowercase[i] != '\0'){
        UPPERCASE[i] = toupper((unsigned char)lowercase[i]);
        i++;
    }
    UPPERCASE[i] = '\0';
    //need to free the pointer after you're done with it
    return UPPERCASE;
}
