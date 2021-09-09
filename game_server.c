// cd assignment1/milestone2/game-server-client
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
    // Child process ID
    pid_t cpid;
    // Other variables
    int numUsers = 0;
    int maxUsers = 3;
    int activeUsers[3];
    int gameStarted = 0;

    // Initiate server connection
    initServer(&client, &server, &serverSocket, &res);

    // Server initiated, accept muliple clients
    while(1){
        // ACCEPT CONNECTION - Accept upto max clients
        // Connection from client
        if (numUsers < maxUsers && !gameStarted){
            int clientSize = sizeof(client);
            clientSocket = accept(serverSocket, (struct sockaddr *) &client, &clientSize);
            // Check if connection is succesful
            if (clientSocket < 0){
                printError("Connection unsuccessful.");
            } else {
                printLog("Connection established.");
                // Create child process
                cpid = fork();
                activeUsers[numUsers++] = cpid;
            }
        } else {
            // Game full, start game
            gameStarted = 1;
            // If number of users left is 1, exit game
            if (numUsers == 1){
                break;
            }
            // Check each active process to see if client has disconnected
            for (int i = 0; i < numUsers; i++){
                // Check the status of each process
                int status;
                pid_t checkpid = waitpid(activeUsers[i], &status, WNOHANG);
                if (checkpid == -1){
                    printError("Check process ID return error.");
                } else if (checkpid == activeUsers[i]){
                    // If check process id == child id, then process has finished
                    // Remove ID from active processes, push remaining down
                    for (int j = i; j < numUsers; j++){
                        activeUsers[j] = activeUsers[j+1];
                    }
                    // Decrease players and i (to check current position again)
                    numUsers--;
                    i--;
                }
            }
        }
        
        // Check error, else check child process, else parent process
        if (cpid < 0){
            printError("Fork Failed.");
        } else if (cpid == 0){
            // In child process
            // Close server socket in child process
            close(serverSocket);
            // CONNECTION ESTABLISHED, RECIEVE AND RESPOND
            // Wait for client request
            while (1){
                // Reset client message
                memset(clientMsg, '\0', SIZE);
                memset(serverMsg, '\0', SIZE);
                // Recieve input
                readSize = recv(clientSocket, clientMsg, SIZE, 0);
                // Check if recieve error
                if (readSize < 0){
                    printError("Recieve data fail.");
                }

                // Tokenise client buffer (by spaces)
                char** msg = 0;
                int size = 0;
                // Get command as array of strings, using token from strtok
                char* token = strtok(clientMsg, " ");
                // Store each command as a ragged array using array of char pointers
                while (token != NULL){
                    // Reallocate space for each string, then allocate number of characters within the string
                    msg = (char**) realloc(msg, (size+1)*sizeof(*msg));
                    msg[size] = malloc(sizeof(char)*strlen(token));
                    // Assign token string, move to next using space as the delimiter
                    msg[size++] = token;
                    token = strtok(NULL, " ");
                }

                // Send data back to client
                if (strcmp(clientMsg, "time") == 0){
                    // Use command
                    time_t ticks = time(NULL);
                    strcpy(serverMsg, ctime(&ticks));
                    // Send data
                    send(clientSocket, serverMsg, sizeof(serverMsg), 0);
                } else if (strcmp(clientMsg, "quit") == 0){
                    strcpy(serverMsg, "quit");
                    send(clientSocket, serverMsg, sizeof(serverMsg), 0);
                    printLog("Client disconnected.");
                    close(clientSocket);
                    exit(0);
                }
            }

        } else {
            // In parent process
            
        }
 
    }

    // Win message
    printLog("Game Won!");

    // Close server socket on main process
    close(serverSocket);


    return 0;
}