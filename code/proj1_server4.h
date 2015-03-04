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
#include <cstring>
#include <random>

#define INPUT_INCORRECT_SIZE 1
#define INPUT_INCORRECT_PORT 2
#define SOCKET_ERROR 3
#define SOCKET_BIND_ERROR 4
#define SOCKET_LISTEN_ERROR 5
#define SOCKET_SEND_ERROR 6



