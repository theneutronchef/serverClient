#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#define UDP_PACKET_SIZE 10000
#define SERVER_TIMEOUT 5

#define INPUT_INCORRECT_SIZE 1
#define INPUT_INCORRECT_ADDR 2
#define INPUT_INCORRECT_MODE 3
#define INPUT_INCORRECT_PORT 4
#define INPUT_INCORRECT_FILE 5
#define INPUT_INCORRECT_STATSFILE 6
#define SOCKET_ERROR 7
#define SOCKET_CONNECT_ERROR 8
#define SOCKET_SEND_ERROR 9
#define SOCKET_BIND_ERROR 10
#define OUTPUT_FILE_ERROR 11
#define SET_TIMEOUT_ERROR 12
