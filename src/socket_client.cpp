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

namespace socket_connection{

void SocketClient::reset(){
    setSocketListener(nullptr);
    setConnected(false);
    getReceiver().reset();
    servers.clear();
}

SocketClient::SocketClient(lms::logging::Logger *parentLogger):logger("SocketClient",parentLogger){
    //SocketClient::listener = nullptr;
}

bool SocketClient::connectToServer(std::string address, int port){
    //setup host
    struct hostent *hostent;
    hostent = gethostbyname(address.c_str());
    if (hostent == NULL) {
        logger.perror("connect to server") << "ERROR, no such host";
        return false;
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
        logger.perror("connect to server")<<"ERROR opening socket";
    //set socket_addr
    server.socket_addr.sin_family = AF_INET;
    bcopy((char *) hostent->h_addr, (char *) &server.socket_addr.sin_addr.s_addr,
          hostent->h_length);
    server.socket_addr.sin_port = htons(port);
    //connect to server
    if (connect(server.getFileDescriptor(), (struct sockaddr *) &(server.socket_addr), server.adress_length) < 0){
        logger.perror("ERROR connecting");
        server.close();
        return false;
    }
    server.setConnected(true);
    servers.push_back(server);
    //send debug message to server
    logger.debug("client connected to server\n");
    return true;
}

void SocketClient::cycleClient(){
    if(listenToFiles()){
        checkNewMessages();
    }
}

void SocketClient::checkNewMessages(){
    int n = 0;
    for (int i = 0;i < (int) servers.size();i++) {
        SocketConnector & server = servers[i];
        if(!server.isConnected()){
            continue;
        }
        if (FD_ISSET(server.getFileDescriptor(), &fds)) {

            n = read(server.getFileDescriptor(), server.getReceiver().getWriteBuffer(), server.getReceiver().getBufferSpace());
            if (n <= 0) {
                //Server closed connection
                logger.perror("checkNewMessages: ") << n;
                getSocketListener()->disconnected(server);
                servers.erase(servers.begin()+i);
                i--;
                //it = servers.erase(it) - 1;
            } else {
                // logger.info()
                server.getReceiver().addedBytes(n);
                if(getSocketListener() != nullptr){
                    while(server.getReceiver().receivedMessage()){
                        getSocketListener()->receivedMessage(server,server.getReceiver().getLastReadPointer(),server.getReceiver().getLastReadCount());
                    }
                }
            }
        }
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
            //add the filedescriptor for the given server
            FD_SET(server.getFileDescriptor(), &fds);
            if(server.getFileDescriptor() > max_fd){
                max_fd = server.getFileDescriptor();
            }
        } else {
            //remove invalid servers
            logger.info("listenToFiles") <<"invalid server file";
            server.setConnected(false);
            //not sure if that method should be called
            if(getSocketListener() != nullptr){
                getSocketListener()->disconnected(server);
                server.close();
            }
        }
    }
    //TODO zero timeout just for debug
    bzero(&timeout,sizeof(timeout));

    return select(max_fd+1, &fds, NULL, NULL, &timeout) > 0;
}

void SocketClient::sendMessageToAllServers(const void *buffer, int bytesToSend, bool addSize){
    logger.info("sendMessageToAllServers") <<"serverCount: " << servers.size();
    for(SocketConnector &sc: servers){
        sc.sendMessage(buffer,bytesToSend,addSize);
    }
}


void SocketClient::setSocketListener(SocketListener *listener){
    SocketConnector::setSocketListener(listener);
}

}
