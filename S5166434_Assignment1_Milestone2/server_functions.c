#include "includes.h"

// Create and establish a server process and await for an incomming connection
void initServer(struct sockaddr_in* server, int* serverSocket, int* res, int port){
    // Print init message to server
    printf("\n");
    printLog("2803ICT Assignment 1 - Milestone 2: By Harry Rowe.");
    printLog("Initialising server...");
    // CREATE SOCKET
    // Create TCP socket (using address family and socket stream)
    *serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Check if socket unavailible
    if (*serverSocket == -1){
        printError("Socket failed");
    } else {
        printLog("Socket created.");
    }
    // Prepare sockaddr structure
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = INADDR_ANY; // Current IP address
    server->sin_port = htons(port); // Port number specified as macro
    // BIND ADDR TO SOCKET
    // Bind address
    *res = bind(*serverSocket, (struct sockaddr *) &*server, sizeof(*server));
    // Error handle for bind error
    if (*res < 0){
        printError("Bind failed.");
    } else {
        printLog("Bind successful.");
    }
    // LISTEN FOR CLIENT
    *res = listen(*serverSocket, BACKLOG);
    // Error handle for waiting for client
    if (*res != 0){
        printError("Listen failed.");
    } else {
        printLog("Waiting for players to join...");
    }
}

// Connect a client to the server
void connectClient(int* users, int* activeUsers, int* p1, int* p2, int* numUsers, struct sockaddr_in* client, int* clientSocket, int serverSocket, pid_t* cpid){
    printf("\tConnected users = %d\n\n", (*numUsers));
    // Accept connection from client
    int clientSize = sizeof((*client));
    (*clientSocket) = accept(serverSocket, (struct sockaddr *) &*client, &clientSize);
    // Check if connection is succesful
    if ((*clientSocket) < 0){
        printError("Connection unsuccessful.");
    } else {
        printLog("Connection established.");
        // Pipe to child process, use error handle
        if (pipe(&p1[2*(*numUsers)]) == -1){
            printError("Pipe 1 error.");
        }
        if (pipe(&p2[2*(*numUsers)]) == -1){
            printError("Pipe 2 error.");
        } 
        // Create child process
        (*cpid) = fork();
        users[(*numUsers)] = (*numUsers);
        activeUsers[(*numUsers)++] = (*cpid);
    }
}

// Start game function
void startGame(int* gameStarted, int numUsers, struct sockaddr_in* client, int clientSocket, int serverSocket, pid_t* listenid){
    // If game not started already, start now
    if (!(*gameStarted)){
        printf("\tConnected users = %d\n\n", numUsers);
        printLog("All players connected. Game commencing...");
        (*gameStarted) = 1;
    }
    // Listen for requests whilst game is playing
    (*listenid) = fork();
    if (listenid < 0){
        printError("Listen fork failed.");
    } else if ((*listenid) == 0){
        // Listen id child process
        // Check for another client, then block
        while (1){
            int clientSize = sizeof(int);
            clientSocket = accept(serverSocket, (struct sockaddr *) &*client, &clientSize);
            // Send client info on game already starting
            if (clientSocket >= 0){
                send(clientSocket, "TEXT ERROR. Game already started.", SIZE, 0);
                send(clientSocket, "END", SIZE, 0);
                close(clientSocket);
            }
        }
        // Exit child process
        exit(0);
    }
}

// Called when child process sends GO to its client socket
void runGo(int* p1, int* p2, char clientMsg[SIZE], int clientSocket, int numUsers){
    // Signal handle if client has already disconnected (resulting in broken pipe)
    signalHandle();
    // Receive sum from main process
    int sum = 0;
    if (read(p2[2*(numUsers-1)], &sum, sizeof(int)) == -1){
        printError("Cannot recieve instruction from main process.");
    }
    // Send sum to client
    char sumMsg[SIZE] = "TEXT Your turn! Pick a number between 1-9. Sum is currently ";
    char sumNumber[SIZE];
    sprintf(sumNumber, "%d", sum);
    strcat(sumMsg, sumNumber);
    int retval = send(clientSocket, sumMsg, SIZE, 0);
    if (retval < 0){
        // Client previously disconnected
        clientDisconnect(p1, numUsers, clientSocket);
    }
    // Hold number of failed attempts
    int turns = 1;
    while (1){
        // Send information to client
        int retval = write(clientSocket, "GO", SIZE);
        // Client has already disconnected
        if (retval < 0){
            // Client previously disconnected
            clientDisconnect(p1, numUsers, clientSocket);
        }
        // Recieve information from client
        int readSize = recv(clientSocket, clientMsg, SIZE, 0);
        // Check if recieve error
        if (readSize < 0){
            printError("Recieve data fail.");
        }
        // Protocol error on empty string
        if (strcmp(clientMsg, "") == 0){
            clientDisconnect(p1, numUsers, clientSocket);
        }
        // Tokenise client buffer (by spaces)
        char** msg = 0;
        int size = 0;
        // Get command as array of strings, using token from strtok
        tokenString(&msg, clientMsg, &size);
        // Check information that the client has sent back
        if (strcmp(msg[0], "MOVE") == 0){
            // Client wishes to move
            // Store number as integer
            int number = atoi(msg[1]);
            if (number > 0 && number < 10){
                // Valid move
                if (write(p1[2*(numUsers-1)+1], &number, sizeof(int)) == -1){
                    printError("Cannot write to main process.");
                }
                break;
            } else if (turns < 5) {
                // Tell client the invalid information was provided
                send(clientSocket, "TEXT ERROR. Please enter a value between 1-9.\n", SIZE, 0);
                turns++;
            } else {
                // Entered 5 incorrect responses in a row
                send(clientSocket, "TEXT ERROR. Entered 5 errors. Disconnecting...", SIZE, 0);
                send(clientSocket, "END", SIZE, 0);
                clientDisconnect(p1, numUsers, clientSocket);
            }
        } else if (strcmp(msg[0], "QUIT") == 0){
            // Client wishes to quit
            send(clientSocket, "TEXT Quitting game...", SIZE, 0);
            send(clientSocket, "END", SIZE, 0);
            clientDisconnect(p1, numUsers, clientSocket);
        } else {
            // Faulty client - protocol infringement
            send(clientSocket, "TEXT Protocol infringement.", SIZE, 0);
            send(clientSocket, "END", SIZE, 0);
            clientDisconnect(p1, numUsers, clientSocket);
        }
    }
}

// Signal handle if client has already disconnected (resulting in broken pipe)
// Signal -> software generated interrupt
// Focus: handle signal so that error handling can deal with disconnected client
void signalHandle(){
    struct sigaction newAction, oldAction;
    newAction.sa_handler = SIG_IGN; // Set signal handler
    sigemptyset(&newAction.sa_mask); // Empty signal set
    newAction.sa_flags = 0; // Set 0 flags
    sigaction(SIGPIPE, &newAction, &oldAction); // Action on pipe - ignore send() failing internally
}

// Client has disconnected, pipe back to main process and close child process
void clientDisconnect(int* p1, int numUsers, int clientSocket){
    int disconnect = -1;
    printLog("Client disconnected.");
    if (write(p1[2*(numUsers-1)+1], &disconnect , sizeof(int)) == -1){
        printError("Cannot write to main process.");
    }
    close(clientSocket);
    exit(0);
}