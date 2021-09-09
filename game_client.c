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

    initClient(&server, &clientSocket, &res);

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
            printLog("Client timeout.");
            res = send(clientSocket, "quit", SIZE, 0);
            res = recv(clientSocket, serverMsg, SIZE, 0);
            break;
        } else {
            // Scan entire input line
            scanf("%[^\n]%*c", clientMsg);
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