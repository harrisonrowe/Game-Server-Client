#include "includes.h"

int main(int argc, char* argv[]){

    // Client and server messages
    char clientMsg[SIZE] = "";
    char serverMsg[SIZE] = "";
    // Client socket structure
    struct sockaddr_in server;
    // Used for bind of client socket
    int res = 0;
    // Hold client socket descriptor entry
    int clientSocket = 0;

    // Define server address, get from cmd
    // if (argc < 2){
    //     printf("Missing IP Address.\n");
    //     exit(1);
    // }

    // Prepare socket structure
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // CREATE SOCKET

    // Create socket descriptor
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Error handle for socket
    if (clientSocket < 0){
        printf("Socket Not Created.\n");
        exit(1);
    } else {
        printf("Socket Created.\n");
    }

    // CONNECT TO SERVER

    res = connect(clientSocket, (struct sockaddr *) &server, sizeof(server));
    // Check if connection successful
    if (res == -1){
        printf("Connection Failed.\n");
        exit(1);
    } else {
        printf("Connection Successful.\n");
    }

    // COMMUNICATE WITH SERVER

    // Client timeout
    struct timeval timeout;
    fd_set readfds;
    int fd = 0, sret;

    // Clear and set file descriptors
    FD_ZERO(&readfds); // Clear file descriptor array of bits
    FD_SET(fd, &readfds); // Add file descriptor to set

    // Communication loop
    while(strcmp(clientMsg, "quit") != 0){
        
        // Clear client and server messages
        memset(clientMsg, '\0', SIZE);
        memset(serverMsg, '\0', SIZE);

        // Prepare console for user input
        printf("> ");
        fflush(stdout); // Enforce data is sent, flush overhead that is sent by OS
        
        // Timeout length
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        // Check for timeout (30 seconds)
        sret = select(8, &readfds, NULL, NULL, &timeout); // Return 

        // Client timeout -> Data not recieved
        if (sret == 0){
            printf("\nClient Timeout.\n");
            res = send(clientSocket, "quit", SIZE, 0);
            res = recv(clientSocket, serverMsg, SIZE, 0);
            break;
        } else {
            // Scan input
            scanf("%s", clientMsg);

            // Send data to server
            res = send(clientSocket, clientMsg, SIZE, 0);
            if (res < 0){
                printf("Send data failed.\n");
                exit(1);
            }
            // Recieve data
            if (strcmp(clientMsg, "time") == 0){
                res = recv(clientSocket, serverMsg, SIZE, 0);
                printf("Reply from client: %s", serverMsg);
            } else if (strcmp(clientMsg, "quit") == 0){
                res = recv(clientSocket, serverMsg, SIZE, 0);
                break;
            }
        }
    }

    // Close descriptor
    close(clientSocket);

    return 0;
}