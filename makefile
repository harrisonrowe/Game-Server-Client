all: game_client game_server

game_client: game_client.c functions.c
	gcc -o game_client game_client.c functions.c

game_server: game_server.c functions.c
	gcc -o game_server game_server.c functions.c