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
#include "socket_connection/socket_server.h"

#define TRUE   1
/*
 * http://linux.die.net/man/3/fd_set
 */
SocketServer::SocketServer(lms::logging::Logger *parentLogger):logger("SocketServer",parentLogger) {
}

void SocketServer::start(int port) {
    //set default values
    SocketServer::port = port;
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
}

void SocketServer::cycle() {
    if(listenToFiles()){
        checkNewConnections();
        checkNewMessages();
    }

}

bool SocketServer::listenToFiles() {
    int max_fd = getFileDescriptor();
    FD_ZERO(&fds);
	//add server file socket (for adding users)
    FD_SET(getFileDescriptor(), &fds);
    //add clients
    for (std::vector<SocketConnector>::iterator it = clients.begin();
			it != clients.end(); ++it) {
        SocketConnector &client = *it;
		//if valid socket descriptor then add to read list
        if (client.getFileDescriptor() > 0) {
            FD_SET(client.getFileDescriptor(), &fds);
            if(client.getFileDescriptor() > max_fd){
                max_fd = client.getFileDescriptor();
			}
		} else {
			//remove invalid client
            logger.warn("invalid client!!!!");
			it = clients.erase(it) - 1;
		}
    }
    return select(max_fd+1, &fds, NULL, NULL, &timeout) > 0;
}

void SocketServer::checkNewConnections() {
	//check for new clients
    if (FD_ISSET(getFileDescriptor(), &fds)) {

        SocketConnector newClient;

        newClient.setFileDescriptor(accept(getFileDescriptor(),
                (struct sockaddr *) &newClient.socket_addr, &newClient.adress_length));
        if (newClient.getFileDescriptor() < 0) {
            logger.error("new Connection") << "failed accept";
		}
        newClient.setConnected(true);
		addClient(newClient);
        logger.info("new Connection") <<"client connected";
	}
}

void SocketServer::checkNewMessages(){
	int n = 0;
    for (std::vector<SocketConnector>::iterator it = clients.begin();
			it != clients.end(); ++it) {
        SocketConnector &client = *it;
        if (FD_ISSET(client.getFileDescriptor(), &fds)) {
            n = read(client.getFileDescriptor(), client.getReceiver().getReadBuffer(), client.getReceiver().getBufferSpace());
			if (n <= 0) {
                //Somebody disconnected, remove client
                logger.info("check messages") << "client disconnected";
				it = clients.erase(it) - 1;
			} else {
                logger.info("check messages")<<"Server received message:" << client.getReceiver().getReadStart();
                //TODO listener
                char ans[] = "Got your message";
                client.sendMessage(ans,sizeof(ans),true);
			}
		}
	}
}

void SocketServer::addClient(SocketConnector &client) {
	clients.push_back(client);
    if(getSocketListener() != nullptr){
        getSocketListener()->connected(client);
    }
    char msg[] = "Ene mene muh da hast dich registriert du kuh";
    client.sendMessage(msg,sizeof(msg));
    //Maybe send welcome-message
}

void SocketServer::sendMessageToAllClients(const void *buffer, int bytesToSend){
    for(auto &client:clients){
        client.sendMessage(buffer,bytesToSend);
    }
}

void SocketServer::dispose(){
    //TODO: send bye-message to clients
    close();
    for(auto &client:clients){
        client.close();
    }
}
