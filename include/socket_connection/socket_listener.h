#ifndef SOCKET_CLIENT_LISTENER_H
#define SOCKET_CLIENT_LISTENER_H

class SocketConnector;

class SocketListener {

public:
    virtual ~SocketListener() {}

    virtual void receivedMessage(SocketConnector &from, char* buff, int bytesRead)=0;

    virtual void disconnected(SocketConnector &disconnected) = 0;

};

#endif /*SOCKET_CLIENT_LISTENER_H */
