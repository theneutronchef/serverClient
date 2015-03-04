#include "proj1_server4.h"
using namespace std;

int *datablock;
int sockfd, newsockfd;

void printWelcomeMsg(){
// prints a welcome message at the start of the program

cout<<"###################################"<<endl\
    <<"### EE122 Spring 2015 Project 1 ###"<<endl\
    <<"###      Shuo Sun 23602997      ###"<<endl\
    <<"###     Gabriel Tan 23845679    ###"<<endl\
    <<"###        Server Program       ###"<<endl\
    <<"###################################"<<endl<<endl;
}

void printError(int errorno) {
// print an error message for invalid message
switch (errorno)
    {
    case INPUT_INCORRECT_SIZE:
    cerr<<"Invalid number of arguments!"<<endl<<endl\
        <<"Usage ./proj1_server <port>"<<endl\
        <<"<port>"<<endl\
        <<"The port number to use for the server."<<endl<<endl;
    break;

    case INPUT_INCORRECT_PORT:
    cerr<<"Error:"<<endl<<"Port must be a number between 0-65535."<<endl;
    break;

    case SOCKET_ERROR:
    cerr<<"Error:"<<endl<<"Unable to open socket."<<endl;
    break;

    case SOCKET_BIND_ERROR:
    cerr<<"Error:"<<endl<<"Unable to bind socket to server address."<<endl;
    break;

    case SOCKET_LISTEN_ERROR:
    cerr<<"Error:"<<endl<<"Unable to start listening."<<endl;
    break;

    case SOCKET_SEND_ERROR:
    cerr<<"Error:"<<endl<<"Unable to send data."<<endl;

    }
exit (EXIT_SUCCESS);
}

void exitHandler(int param){
    cout<<"Exiting server...";
    delete[] datablock;
    close(sockfd);
    cout<<"Done"<<endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){
int portno, dataBlockSize;
float packetDelay ;
struct sockaddr_in server_addr, client_addr;

// random number generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 10);

signal (SIGINT, exitHandler);
sigignore(SIGPIPE);
printWelcomeMsg();

//parse arguments

if (argc != 2) {
    printError(INPUT_INCORRECT_SIZE);
}

//port
portno = atoi(argv[1]);
if(portno > 65535 || portno < 0) {
    printError(INPUT_INCORRECT_PORT);
}
cout<<"Port: "<<portno<<endl;

//open a new socket
sockfd = socket(AF_INET, SOCK_DGRAM, 0);

//check for error
if (sockfd == -1) printError(SOCKET_ERROR);

//clear buffer
bzero((char *) &server_addr, sizeof(server_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(portno);
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

if (bind (sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
    printError(SOCKET_BIND_ERROR);
}

//initialize buffer
dataBlockSize = 1;
datablock = new int[dataBlockSize];
socklen_t len = sizeof(client_addr);

while(1){
    int count = 0;
    cout<<"Waiting for connection..."<<endl;
    int n = recvfrom(sockfd, datablock, 4, 0, (struct sockaddr *)&client_addr, &len);
    if(n < 0) {cout<<"error"<<endl;}
    else {
        //someone connected
        cout<<"Request received from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
        cout<<"Sending..."<<endl;

        while(1) {
            count = count + 1;
            datablock[0] = count;
            packetDelay = dis(gen);
            cout<<datablock[0]<<endl;
            packetDelay = 6;
            cout<<"Delay: "<<packetDelay<<endl;
            sleep(packetDelay);

            //send the datablock
            if(sendto(sockfd, datablock, dataBlockSize, 0, (struct sockaddr *)&client_addr, len) == -1) printError(SOCKET_SEND_ERROR);
        }
    }
    
}
cout<<"Exit"<<endl;
return 0;
}
