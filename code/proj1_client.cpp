#include "proj1_client.h"
using namespace std;

char *buf, *buf2;
bool verbose=false;
double duration;
timeval startTime, endTime;
void printWelcomeMsg(){
// prints a welcome message at the start of the program

cout<<"###################################"<<endl\
    <<"### EE122 Spring 2015 Project 1 ###"<<endl\
    <<"###      Shuo Sun 23602997      ###"<<endl\
    <<"###         Gabriel Noob        ###"<<endl\
    <<"###        Client Program       ###"<<endl\
    <<"###################################"<<endl<<endl;
}

void printError(int errorno) {
// print an error message for invalid message
switch (errorno)
    {
    case INPUT_INCORRECT_SIZE:
    cerr<<"Invalid number of arguments!"<<endl<<endl\
        <<"Usage ./proj1_server <mode> <address> <port> <filename> <stats_filename>"<<endl\
        <<"<mode>"<<endl<<"Either 0,1, or 2."<<endl\
        <<"This value indicates what type of connection to establish with the server."<<endl\
        <<"0: Connection-oriented sockets"<<endl\
        <<"1: Connectionless sockets"<<endl\
        <<"2: Connectionless sockets without checksum enabled"<<endl<<endl\
        <<"<address>"<<endl\
        <<"The IP address of the server to connect to."<<endl<<endl\
        <<"<port>"<<endl\
        <<"The port number of the server to connect to."<<endl<<endl\
        <<"<filename>"<<endl\
        <<"Where to store the file on the local machine after it has been transferred from the server."<<endl<<endl\
        <<"<stats_filename>"<<endl\
        <<"Where to write the file containing sttistics information about the received packets from the server."<<endl<<endl;
    break;

    case INPUT_INCORRECT_ADDR:
    cerr<<"Error:"<<endl<<"Unable to get host."<<endl;
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

    case SOCKET_ERROR:
    cerr<<"Error:"<<endl<<"Unable to open socket."<<endl;
    break;

    case SOCKET_CONNECT_ERROR:
    cerr<<"Error:"<<endl<<"Unable to connect to host."<<endl;
    break;

    case SOCKET_SEND_ERROR:
    cerr<<"Error:"<<endl<<"Unable to send to host."<<endl;
    break;

    case SOCKET_BIND_ERROR:
    cerr<<"Error:"<<endl<<"Unable to bind client."<<endl;
    break;

    case OUTPUT_FILE_ERROR:
    cerr<<"Error:"<<endl<<"Unable to open file for writing."<<endl;
    break;

    case SET_TIMEOUT_ERROR:
    cerr<<"Error:"<<endl<<"Unable to set server timeout."<<endl;
    }
exit (EXIT_SUCCESS);
}

void readUntilNull(int fd, char* buf){
    int offset = 0;
    while(int n = read(fd, buf+offset, 1)){
       if(buf[offset]=='\0'){
       break;
       }
       else if(n) offset++;
    }
}

int main(int argc, char* argv[]){
ifstream inputFile;
int mode, portno, sockfd;
char*filename, *stats_filename;
struct sockaddr_in server_addr, client_addr;
struct hostent *server;
socklen_t len;
ofstream outputFile, outputStatsFile;

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
cout<<"Server timeout "<<SERVER_TIMEOUT<<" seconds."<<endl;
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
//address
server = gethostbyname(argv[2]);
if(server==NULL) printError(INPUT_INCORRECT_ADDR);
cout<<"Server address: "<<argv[2]<<endl;

//port
portno = atoi(argv[3]);
if(portno > 65535 || portno < 0) {
    printError(INPUT_INCORRECT_PORT);
}
cout<<"Port: "<<portno<<endl;

//filename
filename = argv[4];
cout<<"File name: "<<filename<<endl;

//stats filename
stats_filename = argv[5];
cout<<"stats filename: "<<stats_filename<<endl;

if(argc==7 && strcmp(argv[6],"-v")==0) verbose=true;

gettimeofday(&startTime,NULL);

//open a new socket
if (mode == 0) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
}
else if (mode == 1 ) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}
else {
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); 
}

//check for error
if (sockfd == -1) printError(SOCKET_ERROR);


//bind client to a port
//clear buffer
bzero((char *) &client_addr, sizeof(client_addr));

client_addr.sin_family = AF_INET;
client_addr.sin_port = htons(0);
client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

if (bind (sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) == -1) {
        printError(SOCKET_BIND_ERROR);
}

//clear buffer
bzero((char *) &server_addr, sizeof(server_addr));

//set server address
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(portno);
bcopy((char *)server->h_addr,
      (char *)&server_addr.sin_addr.s_addr,
      server->h_length);

len = sizeof(server_addr);

if(mode==0) {
    //connect to server
    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) printError(SOCKET_CONNECT_ERROR);
    buf = new char[200];
    buf2 = new char[200];
    readUntilNull(sockfd, buf);
    readUntilNull(sockfd, buf2);

    //convert char* to int
    int fileSize = atoi(buf);
    int packetSize = atoi(buf2);

    //clear buffers
    delete[] buf;
    delete[] buf2;

    cout<<endl<<"Receiving a file of "<<fileSize<<" bytes and packet(s) of size: "<<packetSize<<" bytes...";
    //open files for writing
    outputFile.open(filename, ios::out | ios::binary);
    if(!outputFile.is_open()) printError(OUTPUT_FILE_ERROR);

    outputStatsFile.open(stats_filename, ios::out);
    if(!outputStatsFile.is_open()) printError(OUTPUT_FILE_ERROR);

    //loop and read
    buf = new char[packetSize];
    int bytesReceived = 0;

    while(bytesReceived < fileSize) {
        gettimeofday(&endTime,NULL); 
        double tmp1 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);

        int n = read(sockfd, buf, packetSize);
        bytesReceived += n;    
        if(verbose) cout<<"Received "<<bytesReceived<<" out of "<<fileSize<<" bytes..."<<endl;
        outputFile.write(buf, n);

        //update stats file
        gettimeofday(&endTime, NULL);
        double tmp2 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);
        outputStatsFile<<tmp2-tmp1<<endl;
        }
    cout<<"Done"<<endl;
    gettimeofday(&endTime,NULL);

    double tmp1 = startTime.tv_sec + (startTime.tv_usec/ 1000000.0);
    double tmp2 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);
    duration = tmp2 - tmp1;
    cout<<"Connection duration: "<<duration<<" seconds."<<endl;

    cout<<"All data written to "<<filename<<"."<<endl;
    }

else if(mode == 1 || mode == 2) {
    //initialize buffer
    buf = new char[UDP_PACKET_SIZE];

    //set timeout
    struct timeval tv;
    tv.tv_sec = SERVER_TIMEOUT;
    tv.tv_usec = 0;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) printError(SET_TIMEOUT_ERROR);

    cout<<"Sending request to server...";
    if(sendto(sockfd, "get", 4, 0, (struct sockaddr *)&server_addr, len) == -1) { printError(SOCKET_SEND_ERROR);}
    cout<<"Done"<<endl;

     //open files for writing
    outputFile.open(filename, ios::out | ios::binary);
    if(!outputFile.is_open()) printError(OUTPUT_FILE_ERROR);

    outputStatsFile.open(stats_filename, ios::out);
    if(!outputStatsFile.is_open()) printError(OUTPUT_FILE_ERROR);

    //loop and read
    int bytesReceived = 0;
    int packetsReceived = 0;

    while(1) {
        gettimeofday(&endTime,NULL); 
        double tmp1 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);

        int n = recvfrom(sockfd, buf, UDP_PACKET_SIZE, 0, (struct sockaddr *)&server_addr, &len);
        if(n==-1) break;

        //if no packet received, then break
        bytesReceived += n;    
        packetsReceived ++;

        if(verbose) cout<<"Received packet "<<packetsReceived<<" and "<<bytesReceived<<" bytes."<<endl;
        outputFile.write(buf, n);

        //update stats file
        gettimeofday(&endTime, NULL);
        double tmp2 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);
        outputStatsFile<<tmp2-tmp1<<endl;
        }
    cout<<"Done - "<<packetsReceived <<" packets received."<<endl;

    double tmp1 = startTime.tv_sec + (startTime.tv_usec/ 1000000.0);
    double tmp2 = endTime.tv_sec + (endTime.tv_usec/ 1000000.0);
    duration = tmp2 - tmp1;
    cout<<"Connection duration: "<<duration<<" seconds."<<endl;

    cout<<"All data written to "<<filename<<"."<<endl;  
}
    

cout<<"Clearing memory...";
outputFile.close();
delete[] buf;
cout<<"Done"<<endl;
return 0;
}
