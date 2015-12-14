/**
 * Really basic SocketServer
 * TODO:
 * add remove(client) function
 * listeners for events (messages/char[], connecting users etc)
 * send message to all users except some
 *
 * code build on top of:
 *  http://www.linuxhowtos.org/C_C++/socket.htm
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <sys/select.h>
#include <netdb.h>
#include "socket_connection/socket_connection_handler.h"

namespace socket_connection{
#define TRUE   1
/*
 * http://linux.die.net/man/3/fd_set
 */
SocketConnectionHandler::SocketConnectionHandler(lms::logging::Logger &logger): logger(logger) {
}


bool SocketConnectionHandler::hasConnections(){
    return connections.size() > 0;
}

void SocketConnectionHandler::openPortForRequests(int port) {
    //set default values
    SocketConnectionHandler::port = port;
	/*
	 *creates a new socket (TCP as SOCK_STREAM is used)
	 */
    setFileDescriptor(socket(AF_INET, SOCK_STREAM, 0));
    if (getFileDescriptor() < 0){
        logger.error("start") <<"failed to open socket " <<strerror(errno);
    }

	int opt = TRUE;
	//set master socket to allow multiple connections , this is just a good habit, it will work without this
    if (setsockopt(getFileDescriptor(), SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
			sizeof(opt)) < 0) {
		perror("setsockopt");
		exit (EXIT_FAILURE);
    }
	/**
	 * set struct values for creating the socket
     */
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
	/*convert port numbers*/
    socket_addr.sin_port = htons(port);
	/*bind port to socket */
    if (bind(getFileDescriptor(), (struct sockaddr *) &socket_addr, adress_length) < 0){
        logger.error("start") <<"failed to bin port " <<strerror(errno);
    }
	printf("Listener on port %d \n", port);

	//try to specify maximum of 5 pending connections for the master socket
    if (listen(getFileDescriptor(), 5) < 0) {
		perror("listen");
		exit (EXIT_FAILURE);
	}

    logger.info("Server started: port:") <<port;
}

void SocketConnectionHandler::cycle() {
    if(listenToFiles()){
        checkNewConnections();
        checkNewMessages();
    }

}

bool SocketConnectionHandler::listenToFiles() {
    int max_fd = getFileDescriptor();
    FD_ZERO(&fds);
	//add server file socket (for adding users)
    FD_SET(getFileDescriptor(), &fds);
    //add clients
    for (std::vector<SocketConnector>::iterator it = connections.begin();
            it != connections.end(); ++it) {
        SocketConnector &connection = *it;
		//if valid socket descriptor then add to read list
        if (connection.getFileDescriptor() > 0) {
            FD_SET(connection.getFileDescriptor(), &fds);
            if(connection.getFileDescriptor() > max_fd){
                max_fd = connection.getFileDescriptor();
			}
		} else {
			//remove invalid client
            logger.warn("invalid client!!!!");
            it = connections.erase(it) - 1;
		}
    }
    return select(max_fd+1, &fds, NULL, NULL, &timeout) > 0;
}

void SocketConnectionHandler::checkNewConnections() {
	//check for new clients
    if (FD_ISSET(getFileDescriptor(), &fds)) {

        SocketConnector newClient;
        //set new file descriptor
        newClient.setFileDescriptor(accept(getFileDescriptor(),
                (struct sockaddr *) &newClient.socket_addr, &newClient.adress_length));
        if (newClient.getFileDescriptor() < 0) {
            logger.error("new Connection") << "failed accept";
		}
        newClient.setConnected(true);
        //add new client
        addConnection(newClient);
	}
}

void SocketConnectionHandler::checkNewMessages(){
	int n = 0;
    for (std::vector<SocketConnector>::iterator it = connections.begin();
            it != connections.end(); ++it) {
        SocketConnector &client = *it;
        if (FD_ISSET(client.getFileDescriptor(), &fds)) {
            n = read(client.getFileDescriptor(), client.getReceiver().getWriteBuffer(), client.getReceiver().getBufferSpace());
			if (n <= 0) {
                //Somebody disconnected, remove client
                logger.info("check messages") << "client disconnected";
                it = connections.erase(it) - 1;
            } else {
                logger.info("check messages")<<"Server bytes"<<n<<"received message:" << client.getReceiver().getReadBuffer();
                client.getReceiver().addedBytes(n);
                if(getSocketListener() != nullptr){
                    while(client.getReceiver().receivedMessage()){
                        getSocketListener()->receivedMessage(client,client.getReceiver().getLastReadPointer(),client.getReceiver().getLastReadCount());
                    }
                }
            }
        }
	}
}

void SocketConnectionHandler::addConnection(SocketConnector &client) {
    connections.push_back(client);
    logger.info("addConnection") << "client connected";
    if(getSocketListener() != nullptr){
        getSocketListener()->connected(client);
    }
}

void SocketConnectionHandler::sendMessageToAllConnections(const void *buffer, int bytesToSend,bool addBytes){
    for(auto &client:connections){
        client.sendMessage(buffer,bytesToSend,addBytes);
    }
}

bool SocketConnectionHandler::connectToSocket(std::string address, int port){
    //setup host
    struct hostent *hostent;
    hostent = gethostbyname(address.c_str());
    if (hostent == NULL) {
        logger.perror("connectToSocket") << "ERROR, no such host";
        return false;
    }
    //setup server
    SocketConnector newConnection;
    //zero it, it should be already zero
    bzero(&newConnection.socket_addr, sizeof(newConnection.socket_addr));
    //data just for the user
    newConnection.address = address;
    newConnection.port = port;
    //create file descriptor
    newConnection.setFileDescriptor(socket(AF_INET, SOCK_STREAM, 0));
    if (newConnection.getFileDescriptor() < 0)
        logger.perror("connectToSocket")<<"ERROR opening socket";
    //set socket_addr
    newConnection.socket_addr.sin_family = AF_INET;
    bcopy((char *) hostent->h_addr, (char *) &newConnection.socket_addr.sin_addr.s_addr,
          hostent->h_length);
    newConnection.socket_addr.sin_port = htons(port);
    //connect to server
    if (connect(newConnection.getFileDescriptor(), (struct sockaddr *) &(newConnection.socket_addr), newConnection.adress_length) < 0){
        logger.perror("ERROR connecting");
        newConnection.close();
        return false;
    }
    newConnection.setConnected(true);
    connections.push_back(newConnection);
    //send debug message to server
    logger.debug("connectToSocket")<<"connected to: "<<address <<":"<<port;
    return true;
}

void SocketConnectionHandler::close(){
    //TODO close own fileDescriptor
    closeConnections();
}

void SocketConnectionHandler::closeConnections(){
    for(auto &client:connections){
        client.close();
    }
}

void SocketConnectionHandler::setSocketListener(SocketListener *listener){
    this->listener = listener;
}

SocketListener* SocketConnectionHandler::getSocketListener(){
    return listener;
}


void SocketConnectionHandler::reset(){
    setSocketListener(nullptr);
    close();
    connections.clear();
}

}
