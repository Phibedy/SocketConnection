#ifndef SOCKET_CONNECTOR_H
#define SOCKET_CONNECTOR_H
#include <socket_connection/receiver.h>
#include <string>
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
#include <socket_connection/socket_listener.h>

class SocketConnector{
    bool connected;
    Receiver receiver;

    int fileDescriptor;

protected:
    /**
     * stores the size of the address of the client, used to accept calls
     */
    socklen_t adress_length;

    timeval timeout;
    /**
     * file descriptor set
     */
    fd_set fds;

    SocketListener *listener;

public:
    SocketConnector();
    /** Just for later handling, not required by the system*/
    std::string address;
    /** Just for later handling, not required by the system*/
    int port;
    //TODO not sure if that is really needed for all clients
    struct sockaddr_in socket_addr;

    bool isConnected();
    void setConnected(bool state);
    Receiver getReceiver();

    void setFileDescriptor(int f);

    void close();

    int getFileDescriptor();

    friend bool operator==(const SocketConnector& lhs, const SocketConnector& rhs){
            return lhs.fileDescriptor == rhs.fileDescriptor;
        }

    /**
     * @brief SocketConnector::sendMessage write a message into the given fileDescriptor
     * @param buffer
     * @param bytesToSend
     * @return
     */
    bool sendMessage(const void *buffer, int bytesToSend);
    void setSocketListener(SocketListener *listener);
    SocketListener *getSocketListener();

};

#endif /* SOCKET_CONNECTOR_H */
