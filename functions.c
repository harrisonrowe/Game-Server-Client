#include "includes.h"

// Main program functions



// Console log functions

void printLog(char* msg){
    printf("\t\033[0;35mSystem Log:\033[0m %s\n\n", msg); 
}

void printWarning(char* msg){
    printf("\t\033[0;33mSystem Warning:\033[0m %s\n\n", msg); 
}

void printError(char* msg){
    printf("\t\033[0;31mSystem Error:\033[0m %s\n\n", msg);
    exit(1); 
}
