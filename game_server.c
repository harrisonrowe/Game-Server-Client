#include "includes.h"

int main(int argc, char* argv[]){

    // Client and server messages
    char clientMsg[SIZE] = "";
    char serverMsg[SIZE] = "";
    // Client and server sockets
    struct sockaddr_in client, server;
    // Used for bind of server socket
    int res = 0, readSize = 0;
    // Hold client and server socket descriptor entry
    int clientSocket = 0, serverSocket = 0;

    // CREATE SOCKET

    // Create TCP socket (using address family and socket stream)
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Check if socket unavailible
    if (serverSocket == -1){
        printf("Socket Failed.\n");
        exit(1);
    } else {
        printf("Socket Created.\n");
    }

    // Prepare sockaddr structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // Current IP address
    server.sin_port = htons(PORT); // Port number specified as macro

    // BIND ADDR TO SOCKET

    // Bind address
    res = bind(serverSocket, (struct sockaddr *) &server, sizeof(server));
    // Error handle for bind error
    if (res < 0){
        printf("Bind Failed.\n");
        exit(1);
    } else {
        printf("Bind Successful.\n");
    }

    // LISTEN FOR CLIENT

    // Listen
    res = listen(serverSocket, BACKLOG);
    // Error handle for waiting for client
    if (res != 0){
        printf("Listen Failed.\n");
        exit(1);
    } else {
        printf("Waiting for incoming connection...\n");
    }

    // ACCEPT CONNECTION - More than 1 client handled on multiple processes

    // Connection from client
    int clientSize = sizeof(client);
    clientSocket = accept(serverSocket, (struct sockaddr *) &client, &clientSize);
    // Check if connection is succesful
    if (clientSocket < 0){
        printf("Connection Unsuccessful.\n");
        exit(1);
    } else {
        printf("Connection Established.\n");
    }

    // CONNECTION ESTABLISHED, RECIEVE AND RESPOND

    // Wait for client request
    while (strcmp(clientMsg, "quit") != 0){
        // Reset client message
        memset(clientMsg, '\0', SIZE);
        memset(serverMsg, '\0', SIZE);
        // Recieve input

        readSize = recv(clientSocket, clientMsg, SIZE, 0);
        // Check if recieve error
        if (readSize < 0){
            printf("Recieve Failed.\n");
            exit(1);
        }
        // Tokenise client buffer (by spaces)

        // Send data back to client
        if (strcmp(clientMsg, "time") == 0){
            // Use command
            time_t ticks = time(NULL);
            strcpy(serverMsg, ctime(&ticks));
            // Send data
            send(clientSocket, serverMsg, sizeof(serverMsg), 0);
        } else if (strcmp(clientMsg, "quit") == 0){
            break;
        }
    }

    // Close client and server socket
    close(clientSocket);
    close(serverSocket);

    return 0;
}