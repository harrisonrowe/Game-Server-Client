// cd assignment1/milestone2/game-server-client
// ./game_server 80 numbers 2
// ./game_client numbers HARRY-PC2 80
#include "includes.h"

int main(int argc, char* argv[]){
    // Validate command line input
    if (argc != 4){
        printError("Insuffficient command line arguments.");
    }

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
    pid_t cpid, listenid;
    // Port/server variables
    int port = atoi(argv[1]);
    // Command line variables
    char gameType[SIZE] = "";
    strcpy(gameType, argv[2]);
    int maxUsers = atoi(argv[3]);
    int activeUsers[atoi(argv[3])];
    int users[atoi(argv[3])];
    int numUsers = 0;
    int gameStarted = 0;
    int targetScore = 30;
    // Pipes for two way communication between child and parent
    int p1[2*atoi(argv[3])]; // C -> P: Child writes to parent
    int p2[2*atoi(argv[3])]; // P -> C: Parent writes to child

    // Check if max number of users is greater than 1
    if (maxUsers < 2){
        printError("Please enter more than 1 client.");
    }

    // Initiate server connection
    initServer(&client, &server, &serverSocket, &res, port);

    // Server initiated, accept muliple clients
    while(1){
        // ACCEPT CONNECTION - Accept upto max clients and handle clients once game has started
        // If the game is not full and hasn't been started
        if (numUsers < maxUsers && !gameStarted){
            connectClient(users, activeUsers, p1, p2, &numUsers, &client, &clientSocket, serverSocket, &cpid);
        } else {
            // Game full - start game and block incomming connections
            startGame(&gameStarted, numUsers, &client, clientSocket, serverSocket, &listenid);
        }
        // Check error, else check child process, else parent process
        if (cpid < 0){
            printError("Client fork failed.");
        } else if (cpid == 0){
            // In child process
            // Tell client that connection is successful
            send(clientSocket, "TEXT Welcome to the game! Awaiting other players...", SIZE, 0);
            // Close server socket in child process
            close(serverSocket);
            // Close unused pipes
            close(p1[2*(numUsers-1)]);
            close(p2[2*(numUsers-1)+1]);
            // CONNECTION ESTABLISHED, RECIEVE AND RESPOND
            // Wait for client request
            while (1){
                // Reset client message
                memset(clientMsg, '\0', SIZE);
                memset(serverMsg, '\0', SIZE);
                // Read from pipe - Using error handle
                int command;
                if (read(p2[2*(numUsers-1)], &command, sizeof(int)) == -1){
                    printError("Cannot recieve instruction from main process.");
                }
                // Check if game is ready to play
                if (command == 1){
                    send(clientSocket, "TEXT Game commencing. Please await your turn.", SIZE, 0);
                }
                // Check if game is completed - client wins
                if (command == 2){
                    send(clientSocket, "TEXT You win!", SIZE, 0);
                    send(clientSocket, "END", SIZE, 0);
                    break;
                }
                // Check if game is completed - client loses 
                if (command == 3){
                    send(clientSocket, "TEXT You lose!", SIZE, 0);
                    send(clientSocket, "END", SIZE, 0);
                    break;
                }
                // Check if command is currently awaiting input from this process
                if (command == getpid()){
                    runGo(p1, p2, clientMsg, clientSocket, numUsers);
                }
            }
            // Close pipes
            close(p1[2*(numUsers-1)]);
            close(p2[2*(numUsers-1)+1]);
            // Quit process
            close(clientSocket);
            exit(0);
        } else {
            // Parent process
            if (gameStarted){
                // Sum for storing values passed by clients
                int sum = 0;
                // Close unused pipes
                for (int i = 0; i < numUsers; i++){
                    close(p1[2*i+1]);
                    close(p2[2*i]);
                }
                // Write to processes that the game has started
                int startGame = 1;
                for (int i = 0; i < numUsers; i++){
                    if (write(p2[2*i+1], &startGame, sizeof(int)) == -1){
                        printError("Cannot write game start to process.");
                    }
                }
                // Loop through each active user and request input
                while (sum < targetScore){
                    for (int i = 0; i < numUsers; i++){
                        // Select next player availible
                        int player = activeUsers[i], add = 0, ready = 0, win = 2, loss = 3;
                        // Check if player last in game
                        if (numUsers == 1){
                            if (write(p2[2*users[i]+1], &win, sizeof(int)) == -1){
                                printError("Cannot write player to process.");
                            }
                            // Exit out of game loop
                            sum = targetScore;
                            break;
                        }
                        // Send to process
                        if (write(p2[2*users[i]+1], &player, sizeof(int)) == -1){
                            printError("Cannot write player to process.");
                        }
                        // Send sum to process
                        if (write(p2[2*users[i]+1], &sum, sizeof(int)) == -1){
                            printError("Cannot write sum to process.");
                        }
                        // Wait for process to finish
                        if (read(p1[2*users[i]], &add, sizeof(int)) == -1){
                            printError("Cannot read from child process.");
                        }
                        // If valid, add clients score to total score
                        if (add > 0){
                            sum += add;
                            // Check win
                            if (sum >= targetScore){
                                // Let clients know the game is over
                                // Winning client
                                if (write(p2[2*users[i]+1], &win, sizeof(int)) == -1){
                                    printError("Cannot write win to child process.");
                                }
                                // Remaining losing clients
                                for (int j = 0; j < numUsers; j++){
                                    // Don't include this child
                                    if (i == j){
                                        continue;
                                    }
                                    // Write to remaining clients
                                    if (write(p2[2*users[j]+1], &loss, sizeof(int)) == -1){
                                        printError("Cannot write loss to child process.");
                                    }
                                }
                                break;
                            }
                        } else {
                            // Client has been removed from server - remove from users
                            // Remove ID from active processes, push remaining down
                            for (int j = i; j < numUsers; j++){
                                activeUsers[j] = activeUsers[j+1];
                                users[j] = users[j+1];
                            }
                            // Decrease players and i (to check current position again)
                            numUsers--;
                            i--;
                        }
                    }
                }
                // Close pipes
                for (int i = 0; i < numUsers; i++){
                    close(p1[2*users[i]]);
                    close(p2[2*users[i]+1]);
                }
                break;
            }
        }
    }
    // Ensure listen child process is terminated
    kill(listenid, SIGKILL);
    // Close server socket on parent process
    printLog("Server closing - game over.");
    close(serverSocket);
    return 0;
}