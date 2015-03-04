#include "proj1_server.h"
using namespace std;

char *buf;
bool verbose = false;
int sockfd, newsockfd;
ifstream inputFile;

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
        <<"Usage ./proj1_server <mode> <port> <filename> <packet_size> <packet_delay>"<<endl\
        <<"<mode>"<<endl<<"Either 0,1, or 2."<<endl\
        <<"This value indicates what type of connection to establish with the server."<<endl\
        <<"0: Connection-oriented sockets"<<endl\
        <<"1: Connectionless sockets"<<endl\
        <<"2: Connectionless sockets without checksum enabled"<<endl<<endl\
        <<"<port>"<<endl\
        <<"The port number to use for the server."<<endl<<endl\
        <<"<filename>"<<endl\
        <<"A path to a local file to host. "<<endl<<endl\
        <<"<packet_size>"<<endl\
        <<"The size of each packet to send in bytes."<<endl<<endl\
        <<"<packet_delay>"<<endl\
        <<"Server will sleep for this many seconds between sending packets. Can be expressed as floating point value."<<endl;
    break;

    case INPUT_INCORRECT_MODE:
    cerr<<"Error:"<<endl<<"Mode must be a number between 0-2."<<endl;
    break;

    case INPUT_INCORRECT_PORT:
    cerr<<"Error:"<<endl<<"Port must be a number between 0-65535."<<endl;
    break;

    case INPUT_INCORRECT_FILE:
    cerr<<"Error:"<<endl<<"Unable to open file."<<endl;
    break;

    case INPUT_INCORRECT_PACKETSIZE:
    cerr<<"Error:"<<endl<<"Packet size must be bigger than 0 and smaller than the input file size."<<endl;
    break;

    case INPUT_INCORRECT_PACKETDELAY:
    cerr<<"Error:"<<endl<<"Packet delay must be > 0."<<endl;
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
    break;

    }
exit (EXIT_SUCCESS);
}

void exitHandler(int param){
    cout<<"Exiting server...";
    delete[] buf;
    close(newsockfd);
    close(sockfd);
    inputFile.close();
    cout<<"Done"<<endl;
exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){
int mode, portno, packetSize, fileSize;
float packetDelay ;
char* filename, *header, *header2;
streampos begin,end;
struct sockaddr_in server_addr, client_addr;

signal (SIGINT, exitHandler);
printWelcomeMsg();

if (argc != 6 && argc != 7) {
    printError(INPUT_INCORRECT_SIZE);
}

//parse aguments

//mode
mode = atoi(argv[1]);
if(mode > 2 || mode < 0) {
    printError(INPUT_INCORRECT_MODE);
} 
cout<<"Mode: "<< mode <<" ";
switch(mode) {
    case 0:
        cout<<"(Connection-oriented sockets)"<<endl;
    break;
    case 1:
        cout<<"(Connectionless sockets)"<<endl;
    break;
    case 2:
        cout<<"(Connectionless sockets without checksum enabled)"<<endl;
    break;
}

//port
portno = atoi(argv[2]);
if(portno > 65535 || portno < 0) {
    printError(INPUT_INCORRECT_PORT);
}
cout<<"Port: "<<portno<<endl;

//filename
filename = argv[3];
inputFile.open(filename, ios::in | ios::binary);
if(!inputFile.is_open()) {
    printError(INPUT_INCORRECT_FILE);
}
cout<<"File name: "<<filename<<endl;
begin = inputFile.tellg();
inputFile.seekg(0, ios::end);
end = inputFile.tellg();
fileSize = (int)(end-begin);
inputFile.seekg(0, ios::beg);
cout<<"File size: "<<fileSize<<" bytes"<<endl;

//packet size
packetSize = atoi(argv[4]);
if(packetSize <= 0 || packetSize > fileSize) {
    printError(INPUT_INCORRECT_PACKETSIZE);
}
cout<<"Packet size: "<<packetSize<<" bytes"<<endl;

//packet delay
packetDelay = atof(argv[5]);
if(packetDelay < 0) {
    printError(INPUT_INCORRECT_PACKETDELAY);
    }
cout<<"Packet delay: "<<packetDelay<<" seconds"<<endl;

if(argc==7 && strcmp(argv[6],"-v")==0) verbose = true;

//open a new socket
if (mode == 0) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
}
else if (mode == 1) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}
else {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

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
buf = new char[packetSize];
socklen_t len = sizeof(client_addr);

//prepare headers
stringstream str;
str << fileSize;
string temp = str.str();
header = (char*) temp.c_str();

stringstream str2;
str2 << packetSize;
string temp2 = str2.str();
header2 = (char*) temp2.c_str();

//connection oriented mode
if(mode == 0) {
//start listening
if(listen (sockfd, 100) == -1) printError(SOCKET_LISTEN_ERROR);
cout<<"\nServer started listening..."<<endl;
while(1) {
    //move the flag back to the beginning of the file
    inputFile.seekg(0, ios::beg);
    cout<<"Waiting for connection..."<<endl;
    
    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &len);

    if(newsockfd == -1) {
        cout<<"Not able to accept connection...Retrying..."<<endl;
        }
    cout<<"Client connected..."<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;

    cout<<"Sending header(1/2)...";
    if(send(newsockfd, header, strlen(header)+1, 0) == -1) printError(SOCKET_SEND_ERROR);
    cout<<"Done"<<endl;
    cout<<"Sending header(2/2)...";
    if(send(newsockfd, header2,strlen(header2)+1, 0) == -1) printError(SOCKET_SEND_ERROR);
    cout<<"Done"<<endl;

    cout<<"Sending "<<filename<<"..."<<endl;
    //Send the whole files in packets
    int bytesLeft = fileSize;
    int packetCount = 1;
    int bytesSent = 0;
    while(bytesLeft > 0) {
        int bytesToSend = bytesLeft > packetSize? packetSize:bytesLeft;
        if(verbose){
        cout<<"Sending packet "<<packetCount<<", "<<bytesSent+bytesToSend<<" out of "<<fileSize<<" bytes...";
        }

        //send
        inputFile.read(buf, bytesToSend);    
        if(send(newsockfd, buf, bytesToSend, 0) == -1) printError(SOCKET_SEND_ERROR);

        if(verbose) cout<<"Done"<<endl;
        bytesLeft -= bytesToSend;
        bytesSent += bytesToSend;
        packetCount ++;

        if(packetDelay < 1) {
            usleep((int) (packetDelay * 1000000));
            } 
        else {
            sleep((int) packetDelay);
            }
        }
    cout<<"Successfully sent "<<filename<<"("<<fileSize<<" bytes) to "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl<<endl;
    }
}
else if(mode == 1 || mode == 2){

    if(mode == 2) {
        //turn off checksum
	int opt = 1;
	if(setsockopt(sockfd, SOL_SOCKET,SO_NO_CHECK, (void *)&opt, sizeof(opt)) == -1) perror("Error");
	cout<<"UDP Checksum disabled..."<<endl;
        }
    while(1){
        cout<<"Waiting for connection..."<<endl;
        int n = recvfrom(sockfd, buf, 4, 0, (struct sockaddr *)&client_addr, &len);
        if(n < 0) {cout<<"error"<<endl;}
        if(n > 0) {
            //someone connected
            cout<<"Request received from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;

        cout<<"Sending "<<filename<<"..."<<endl;
        //Send the whole files in packets
        int bytesLeft = fileSize;
        int packetCount = 1;
        int bytesSent = 0;
        while(bytesLeft > 0) {
            int bytesToSend = bytesLeft > packetSize? packetSize:bytesLeft;
            if(verbose){
            cout<<"Sending packet "<<packetCount<<", "<<bytesSent+bytesToSend<<" out of "<<fileSize<<" bytes...";
            }

            //send
            inputFile.read(buf, bytesToSend);    
            if(sendto(sockfd, buf, bytesToSend, 0, (struct sockaddr *)&client_addr, len) == -1) printError(SOCKET_SEND_ERROR);

            if(verbose) cout<<"Done"<<endl;
            bytesLeft -= bytesToSend;
            bytesSent += bytesToSend;
            packetCount ++;

            if(packetDelay < 1) {
		//sleep a fraction of a second
                usleep((int) (packetDelay * 1000000));
                } 
            else {
                sleep((int) packetDelay);
                }
            }
    cout<<"Finish sending "<<filename<<"("<<fileSize<<" bytes) to "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl<<endl;

            }
    }
}

return 0;
}
