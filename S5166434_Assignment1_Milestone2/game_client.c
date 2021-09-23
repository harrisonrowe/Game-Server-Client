#include "includes.h"

int main(int argc, char* argv[]){
    // Validate command line input
    if (argc != 4){
        printError("Insuffficient command line arguments.");
    }

    // Variables
    char serverMsg[SIZE] = "";
    // Client socket structure
    struct sockaddr_in server;
    // Used for bind of client socket
    int res = 0;
    // Hold client socket descriptor entry
    int clientSocket = 0;
    // Port/server variables
    int port = atoi(argv[3]);
    char* ip;
    // Command line variables
    char gameType[SIZE];
    char serverName[SIZE];
    // Copy from command line
    strcpy(gameType, argv[1]);
    strcpy(serverName, argv[2]);

    // Initiate client
    initClient(&server, &clientSocket, &res, port, serverName);

    // If communication with server is successful, run a loop, listening for messages
    while(1){
        // Prepare and recieve server message
        // Clear server messages
        memset(serverMsg, '\0', SIZE);
        // Recieve message from server
        res = recv(clientSocket, serverMsg, SIZE, 0);
        // Tokenise client buffer (by spaces)
        char** msg = 0;
        int size = 0;
        tokenString(&msg, serverMsg, &size);

        // Check what msg has been recieved from server
        if (strcmp(msg[0], "TEXT") == 0){
            // Print remaining tokens after TEXT
            printTextMessage(&msg, size);
        } else if (strcmp(msg[0], "GO") == 0){
            // Request input from user
            clientGo(clientSocket, res);
        } else if (strcmp(msg[0], "END") == 0){
            // Close descriptor
            close(clientSocket);
            break;
        } else if (strcmp(msg[0], "ERROR") == 0){
            // Close descriptor
            close(clientSocket);
            printError("Server error.");
        } else {
            // Undefined command from server, terminate client
            close(clientSocket);
            printError("Unknown command from server.");
        }
    }
    return 0;
}