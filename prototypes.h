#ifndef PROTOTYPES_H
#define PROTOTYPES_H

// Client prototypes

void initClient(struct sockaddr_in*, int*, int*);

// Server prototypes

void initServer(struct sockaddr_in*, struct sockaddr_in*, int*, int*);

// Main function prototypes



// Console log prototypes

void printLog(char*);
void printWarning(char*);
void printError(char*);

#endif