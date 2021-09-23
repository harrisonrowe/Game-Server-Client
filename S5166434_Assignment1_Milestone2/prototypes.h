#ifndef PROTOTYPES_H
#define PROTOTYPES_H

// Client prototypes
void initClient(struct sockaddr_in*, int*, int*, int, char*);
void printTextMessage(char***, int);
void clientGo(int, int);

// Server prototypes
void initServer(struct sockaddr_in*, int*, int*, int);
void connectClient(int*, int*, int*, int*, int*, struct sockaddr_in*, int*, int, pid_t*);
void startGame(int*, int, struct sockaddr_in*, int, int, pid_t*);
void runGo(int*, int*, char*, int, int);
void signalHandle();
void clientDisconnect(int*, int, int);

// Main function prototypes
void tokenString(char***, char*, int*);
void printLog(char*);
void printError(char*);

#endif