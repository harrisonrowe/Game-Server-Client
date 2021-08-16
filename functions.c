#include "includes.h"

// Main program functions



// Console log functions

void printMessage(char* msg){
    printf("\t\033[0;35m%s\033[0m\n\n", msg); 
}

void printError(char* msg){
    printf("\t\033[0;31mError: \033[0m%s\n\n", msg); 
}
