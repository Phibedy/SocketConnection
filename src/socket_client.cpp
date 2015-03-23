#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <socket_connection/socket_client.h>
#include <socket_connection/socket_connector.h>
#include <string>

SocketClient::SocketClient(lms::logging::Logger *parentLogger):logger("SocketClient",parentLogger){
    SocketClient::listener = nullptr;
}

void SocketClient::error(const char *msg) {
	perror(msg);
	exit(0);
}

void SocketClient::connectToServer(std::string address, int port){
    //setup host
    struct hostent *hostent;
    hostent = gethostbyname(address.c_str());
    if (hostent == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    //setup server
    //TODO why can't I use connector()?
    SocketConnector server;
    //zero it, it should be already zero
    bzero(&server.socket_addr, sizeof(server.socket_addr));
    //data just for the user
    server.address = address;
    server.port = port;
    //create file descriptor
    server.setFileDescriptor(socket(AF_INET, SOCK_STREAM, 0));
    if (server.getFileDescriptor() < 0)
		error("ERROR opening socket\n");
    //set socket_addr
    server.socket_addr.sin_family = AF_INET;
    bcopy((char *) hostent->h_addr, (char *) &server.socket_addr.sin_addr.s_addr,
            hostent->h_length);
    server.socket_addr.sin_port = htons(port);
    //connect to server
    if (connect(server.getFileDescriptor(), (struct sockaddr *) &(server.socket_addr), server.adress_length) < 0){
        error("ERROR connecting\n");
    }
    server.setConnected(true);
    servers.push_back(server);
    //send debug message to server
    logger.info("client connected to server\n");
    char tmp[] = "hi server";
    server.sendMessage(tmp, sizeof(tmp));
}

void SocketClient::cycleClient(){
    if(listenToFiles()){
        checkNewMessages();
    }
}

bool SocketClient::listenToFiles(){
    FD_ZERO(&fds);
    FD_SET(getFileDescriptor(), &fds);

    return select(getFileDescriptor() +1, &fds, NULL, NULL, &timeout) > 0;
}
void SocketClient::checkNewMessages(){
    int n;
    if (FD_ISSET(getFileDescriptor(), &fds)) {
        char tmp[1000];
            n = read(getFileDescriptor(), &tmp,1000);
            if (n <= 0) {
                //No new messages from server
            } else {
                logger.info("got msg");
            }
        }
}
/*
bool SocketClient::listenToFiles() {
    int max_fd = 0;
    FD_ZERO(&fds);
    //listen to all servers
    for (std::vector<SocketConnector>::iterator it = servers.begin();
            it != servers.end(); ++it) {
        SocketConnector &server = *it;
        //if valid socket descriptor then add to read list
        if (server.getFileDescriptor() > 0) {
            //add the filedescriptor for the given server
            FD_SET(server.getFileDescriptor(), &fds);
            if(server.getFileDescriptor() > max_fd){
                max_fd = server.getFileDescriptor();
            }
        } else {
            //remove invalid client
            logger.info("listenToFiles") <<"invalid server file";
            server.setConnected(false);
            //not sure if that method should be called
            if(getSocketListener() != nullptr){
                getSocketListener()->disconnected(server);
            }
        }
    }
    //TODO zero timeout just for debug
    bzero(&timeout,sizeof(timeout));

    return select(max_fd+1, &fds, NULL, NULL, &timeout) > 0;
}
void SocketClient::checkNewMessages(){
    int n = 0;
    for (std::vector<SocketConnector>::iterator it = servers.begin();
            it != servers.end(); ++it) {
        SocketConnector & server = *it;
        if(!server.isConnected()){
            continue;
        }
        if (FD_ISSET(server.getFileDescriptor(), &fds)) {
            n = read(server.getFileDescriptor(), server.getReceiver().getReadBuffer(), 256);
            if (n <= 0) {
                //Server closed connection
                perror("server disconnected \n");
                logger.error("fauled") << n;
                char tmp[] = "Do you still get this server?";
                server.sendMessage(tmp, sizeof(tmp));
                server.setConnected(false);
                //it = servers.erase(it) - 1;
            } else {
                if(getSocketListener() != nullptr){
                    while(server.getReceiver().receivedMessage(n)){
                        getSocketListener()->receivedMessage(server,server.getReceiver().getReadStart(),server.getReceiver().getLastReadCount());
                    }
                }
                logger.debug("check messages") <<"Client received message";
            }
        }
    }
}


void SocketClient::connectToServer(std::string address, int port){
    struct sockaddr_in serv_addr;
    struct hostent *server;

    setFileDescriptor(socket(AF_INET, SOCK_STREAM, 0));
    if (getFileDescriptor() < 0)
        error("ERROR opening socket\n");
    server = gethostbyname(address.c_str());
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(socket_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &socket_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(getFileDescriptor(), (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0)
        error("ERROR connecting\n");
}

*/
