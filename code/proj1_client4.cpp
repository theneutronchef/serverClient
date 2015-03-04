#include "proj1_client4.h"
using namespace std;

int *buf;
int sockfd;
bool buffer=false;
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
        <<"Usage ./proj1_server <address> <port> [ -b ]"<<endl\
        <<"<address>"<<endl\
        <<"The IP address of the server to connect to."<<endl<<endl\
        <<"<port>"<<endl\
        <<"The port number of the server to connect to."<<endl<<endl\
        <<"[ -b ]"<<endl\
        <<"An optional flag. If present, then the client will buffer the incoming packets, so that the “blinks” appear at a uniform rate of 5 seconds, regardless of whether the incoming packets are spaced more or less than 5 seconds apart."<<endl<<endl;
    break;

    case INPUT_INCORRECT_ADDR:
    cerr<<"Error:"<<endl<<"Unable to get host."<<endl;
    break;

    case INPUT_INCORRECT_PORT:
    cerr<<"Error:"<<endl<<"Port must be a number between 0-65535."<<endl;
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

    case SET_TIMEOUT_ERROR:
    cerr<<"Error:"<<endl<<"Unable to set server timeout."<<endl;

    }
exit (EXIT_SUCCESS);
}

int getTime(struct timeval t) {
    gettimeofday(&t, NULL); 
    return (int) (t.tv_sec + t.tv_usec/1000000.0);
}

int main(int argc, char* argv[]){
    int portno;
    int start, now;

    struct sockaddr_in server_addr, client_addr;
    struct hostent *server;
    socklen_t len;

    printWelcomeMsg();

    if (argc != 3 && argc != 4) {
        printError(INPUT_INCORRECT_SIZE);
    }

    //parse aguments

    //address
    server = gethostbyname(argv[1]);
    if(server==NULL) printError(INPUT_INCORRECT_ADDR);
    cout<<"Server address: "<<argv[2]<<endl;

    //port
    portno = atoi(argv[2]);
    if(portno > 65535 || portno < 0) {
        printError(INPUT_INCORRECT_PORT);
    }
    cout<<"Port: "<<portno<<endl;

    if(argc==4 && strcmp(argv[3],"-b")==0) buffer=true;

    //open a new udp socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

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

    //initialize buffer
    buf = new int[DATA_BLOCK_SIZE];
    std::queue<int> bufferQueue;

    //set timeout
    struct timeval tv;
    tv.tv_sec = SERVER_TIMEOUT;
    tv.tv_usec = 0;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) printError(SET_TIMEOUT_ERROR);

    cout<<"Sending request to server...";
    if(sendto(sockfd, "get", 4, 0, (struct sockaddr *)&server_addr, len) == -1) { printError(SOCKET_SEND_ERROR);}
    cout<<"Done"<<endl;
    cout<<"Receiving..."<<endl;
    struct timeval t, t2, t3; 
    gettimeofday(&t, NULL); 
    start = (int) (t.tv_sec + t.tv_usec/1000000.0);
    cout<<"Start:"<<start<<endl;
    while(1) {
        // blocking call to recv
        int n = recvfrom(sockfd, buf, DATA_BLOCK_SIZE, 0, (struct sockaddr *)&server_addr, &len);


        // if(n == -1) {
        //     cout<<"gg"<<endl;
        //     break;
        // } else {
            if (buffer) {
                // insert into buffer
                if (n > 0) bufferQueue.push(buf[0]);
                // sleep(5);
                gettimeofday(&t2, NULL); 

                now = (int) (t2.tv_sec + t2.tv_usec/1000000.0);
                cout<<now<<endl;

                if (!bufferQueue.empty() && (now - start) >= 5) {
                    int elem = bufferQueue.front();
                    bufferQueue.pop();
                    cout<<"Blink"<<endl;
                    cout<<"Message: "<<elem<<endl;
                    gettimeofday(&t3, NULL); 

                    start = (int) (t3.tv_sec + t3.tv_usec/1000000.0);
                    cout<<"Start:"<<start<<endl;

                }
            } else {
                cout<<"Blink"<<endl;
                cout<<"Message: "<<buf[0]<<endl;
            }
        // }
    }
    return 0;
}
