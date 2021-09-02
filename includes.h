#ifndef INCLUDES_H
#define INCLUDES_H
// Include libraries

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>

// Macros

#define PORT 80 // Default port number for server
#define BACKLOG 5 // Max number of requests recieved by clients
#define SIZE 1024 // Max size for char arrays
#define IP "192.168.1.13" // Default IP for computer

// Include files

#include "prototypes.h"

#endif