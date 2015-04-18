#ifndef SOCKET_CLIENT_LISTENER_H
#define SOCKET_CLIENT_LISTENER_H

namespace socket_connection{
class SocketConnector;

class SocketListener {

public:
    SocketListener(){}
    virtual ~SocketListener() {}
    /**
     * @brief receivedMessage called if a message was received
     * @param from
     * @param buff
     * @param bytesRead
     */
    virtual void receivedMessage(const SocketConnector &from, char* buff, int bytesRead)=0;
    /**
     * @brief disconnected called if connection was closed
     * @param disconnected
     */
    virtual void disconnected(const SocketConnector &disconnected) = 0;

    /**
     * @brief connected called if a connection was established
     * @param connected
     */
    virtual void connected(const SocketConnector &connected) = 0;

};
}
#endif /*SOCKET_CLIENT_LISTENER_H */
