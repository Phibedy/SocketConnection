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
    //TODO why can't I use connector()????
    SocketConnector connector;
	struct sockaddr_in serv_addr;
    struct hostent *hostent;
    connector.address = address;
    connector.port = port;
    connector.setFileDescriptor(socket(AF_INET, SOCK_STREAM, 0));
    if (connector.getFileDescriptor() < 0)
		error("ERROR opening socket\n");
    hostent = gethostbyname(address.c_str());
    if (hostent == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    bcopy((char *) hostent->h_addr, (char *) &serv_addr.sin_addr.s_addr,
            hostent->h_length);
	serv_addr.sin_port = htons(port);
    if (connect(connector.getFileDescriptor(), (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting\n");
    printf("client connected to server\n");
    connector.setConnected(true);
    servers.push_back(connector);
}

void SocketClient::cycleClient(){
    if(listenToFiles()){
        checkNewMessages();
    }
}
bool SocketClient::listenToFiles() {
    int max_fd = 0;
    FD_ZERO(&fds);
    //listen to all servers
    for (std::vector<SocketConnector>::iterator it = servers.begin();
            it != servers.end(); ++it) {
        SocketConnector &server = *it;
        //if valid socket descriptor then add to read list
        if (server.getFileDescriptor() > 0) {
            FD_SET(server.getFileDescriptor(), &fds);
            if(server.getFileDescriptor() > max_fd){
                max_fd = server.getFileDescriptor();
            }
        } else {
            //remove invalid client
            printf("invalid server!!!!");
            server.setConnected(false);
            //it = server.erase(it) - 1;
        }
    }
    return select(max_fd+1, &fds, NULL, NULL, &timeout) > 0;
}
void SocketClient::checkNewMessages(){
    int n = 0;
    for (std::vector<SocketConnector>::iterator it = servers.begin();
            it != servers.end(); ++it) {
        SocketConnector & server = *it;
        if (FD_ISSET(server.getFileDescriptor(), &fds)) {
            n = read(server.getFileDescriptor(), server.getReceiver().getReadBuffer(), 256);
            if (n <= 0) {
                //Server closed connection
                printf("server disconnected \n");
                server.setConnected(false);
                //it = servers.erase(it) - 1;
            } else {
                //TODO server.getReceiver().receivedMessage();
                logger.debug("check messages") <<"Client received message";
            }
        }
    }
}
