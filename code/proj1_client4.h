#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <cstring>
#include <queue>

#define DATA_BLOCK_SIZE 1
#define SERVER_TIMEOUT 5

#define INPUT_INCORRECT_SIZE 1
#define INPUT_INCORRECT_ADDR 2
#define INPUT_INCORRECT_PORT 3
#define SOCKET_ERROR 4
#define SOCKET_CONNECT_ERROR 5
#define SOCKET_SEND_ERROR 6
#define SOCKET_BIND_ERROR 7
#define SET_TIMEOUT_ERROR 8

