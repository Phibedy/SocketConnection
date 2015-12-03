#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <aio.h>
#include <netinet/in.h>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>

namespace socket_connection{
class SocketConnectionHandler:public SocketConnector {

public:
    SocketConnectionHandler(lms::logging::Logger &logger);
    //char buffer[1000*100];

private:

    SocketListener *listener;

    lms::logging::Logger &logger;

    std::vector<SocketConnector> connections;

    /**
     * TODO same as in client
     * @brief listenToFiles
     * @return
     */
    bool listenToFiles();
    /**
     * @brief checkNewConnections checks new incoming if listenToFiles returned true
     */
	void checkNewConnections();
    /**
     * @brief checkNewMessages checks if new messages are available if listenToFiles returned true, called after checkNewConnections
     */
    void checkNewMessages();
    /**
     * @brief addClient adds a client
     * @param client
     */
    void addConnection(SocketConnector &client);
public:
    /**
     * @brief connectToSocket
     * @param address
     * @param port
     * @return true if the connecting was successfull
     */
    bool connectToSocket(std::string address,int port);

    bool hasConnections();
    /**
     * @brief start opens the port
     * TODO only one port is available atm
     * @param port
     */
    void openPortForRequests(int port);
    void sendMessageToAllConnections(const void *buffer, int bytesToSend,bool addBytes = true);


    void setSocketListener(SocketListener *listener);

    SocketListener* getSocketListener();

    /**
     * @brief cycle call this method to check for new clients and messages
     */
    void cycle();


    void reset();
    /**
     * @brief close closes the server and all related connections
     */
    void close();
    void closeConnections();
};
}
#endif /*SOCKET_SERVER_H */
