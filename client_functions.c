#include "includes.h"

// Initialise client and connect to specific server
void initClient(struct sockaddr_in* server, int* clientSocket, int* res){
    printLog("2803ICT Assignment 1 - Milestone 2: By Harry Rowe.");
    printLog("Initialising client...");
    // CREATE SOCKET
    // Prepare socket structure
    server->sin_addr.s_addr = inet_addr(IP);
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);

    // Create socket descriptor
    *clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Error handle for socket
    if (*clientSocket < 0){
        printError("Socket failed.");
    } else {
        printLog("Socket created.");
    }

    // CONNECT TO SERVER
    *res = connect(*clientSocket, (struct sockaddr *) &*server, sizeof(*server));
    // Check if connection successful
    if (*res == -1){
        printError("Connection failed.");
    } else {
        printLog("Connection successful.");
    }
}