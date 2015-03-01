#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define INPUT_INCORRECT_SIZE 1
#define INPUT_INCORRECT_MODE 2
#define INPUT_INCORRECT_PORT 3
#define INPUT_INCORRECT_FILE 4
#define INPUT_INCORRECT_PACKETSIZE 5
#define INPUT_INCORRECT_PACKETDELAY 6
#define SOCKET_ERROR 7
#define SOCKET_BIND_ERROR 8
#define SOCKET_LISTEN_ERROR 9
#define SOCKET_SEND_ERROR 10
