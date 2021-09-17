#include "includes.h"

// Initialise client and connect to specific server
void initClient(struct sockaddr_in* server, int* clientSocket, int* res, int port, char* host){
    // Print init message to clients
    printf("\n");
    printLog("2803ICT Assignment 1 - Milestone 2: By Harry Rowe.");
    printLog("Initialising client...");
    // CREATE SOCKET
    // Get IP address of server name (hostname) the client wishes to connect to
    struct hostent *host_entry;
    char* ip;
    host_entry = gethostbyname(host);
    ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); // Returns IP address of host
    // Prepare socket structure
    server->sin_addr.s_addr = inet_addr(ip);
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
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

// Response to servers TEXT message
void printTextMessage(char*** msg, int size){
    // Log that a message has arrived from the server
    printLog("Message from server...");
    printf("\t");
    // Print remaining messages after TEXT
    for (int i = 1; i < size; i++){
        printf("%s ", (*msg)[i]);
    }
    printf("\n\n");
}

// Response to servers GO message
void clientGo(int clientSocket, int res){
    // Message variable
    char clientMsg[SIZE] = "";
    // Prepare client timeout structure
    struct timeval timeout;
    fd_set readfds;
    int fd = 0, sret;
    // Clear and set file descriptors
    FD_ZERO(&readfds); // Clear file descriptor array of bits
    FD_SET(fd, &readfds); // Add file descriptor to set

    // Request input from user
    printf("> ");
    fflush(stdout);
    // Set timeout length - 30 seconds
    timeout.tv_sec = 30;
    // timeout.tv_usec = 0;
    // Check for timeout
    sret = select(1, &readfds, NULL, NULL, &timeout); 
    // Client timeout -> Data not entered
    if (sret == 0){
        printf("\n\n");
        printLog("Client timeout.");
        res = send(clientSocket, "QUIT", SIZE, 0);
        if (res < 0){
            printError("Send data error.");
        }
    } else {
        // Data entered
        // Scan entire input line
        scanf("%[^\n]%*c", clientMsg);
        printf("\n");
        // Check if client wants to quit
        if (strcmp(clientMsg, "quit") == 0){
            strcpy(clientMsg, "QUIT");
        }
        // Check if client message is just numbers, if so, append MOVE
        if (isdigit(clientMsg[0])){
            char move[SIZE] = "MOVE ";
            strcat(move, clientMsg);
            strcpy(clientMsg, move);
        }
        // Send data back to server
        res = send(clientSocket, clientMsg, SIZE, 0);
        if (res < 0){
            printError("Send data error.");
        }
    }
}