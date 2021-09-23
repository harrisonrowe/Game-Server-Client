# Game-Server-Client

# 2803ICT - Assignment 1 Milestone 2: By Harry Rowe

# Compile instructions:
# Whilst in the folder: S5166434_2803ICT_Assignment1_Milestone2
# Type: 'make' into a terminal environment
# This will compile corresponding .c and .h files into two executables 
# game_server.exe
# game_client.exe

# Server Instructions:
# To run the server executable, it requires three command line arguments
# ./game_server <PortNumber> <GameType> <GameArgs/NumberOfClients> 
# Port number is the address the server will bind to, clients need to enter the same number
# Game type is 'numbers'
# Game args is the number of clients to be connected to the server prior to the game commencing
# This needs to be executed prior to connecting any clients

# Client Instructions:
# Once the server executable is listening for clients, they may connect by running:
# ./game_client <GameType> <ServerName> <PortNumber>
# Game type is 'numbers'
# The server name is used for the hostname or IP address of the hosted server
# To connect to the server, the port number must match

# The game type is called numbers, it can be played with 2 or more people
# Each client takes in turns (in joining order) chosing a number between 1-9
# The sum of previously chosen answers is stored on the server
# The first client to get to 30 or above wins the game