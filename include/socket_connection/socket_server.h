#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <aio.h>
#include <netinet/in.h>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>

namespace socket_connection{
class SocketServer:public SocketConnector {

public:
    SocketServer(lms::logging::Logger &logger);
    //char buffer[1000*100];

private:

    lms::logging::Logger &logger;

    std::vector<SocketConnector> clients;

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
    void addClient(SocketConnector &client);
public:

    bool hasClients();
    /**
     * @brief start opens the port
     * TODO My be moved to the constructor
     * @param port
     */
    void start(int port);
    void sendMessageToAllClients(const void *buffer, int bytesToSend,bool addBytes = true);
    /**
     * @brief cycle call this method to check for new clients and messages
     */
    void cycle();
    /**
     * @brief close closes the server and all related connections
     */
    void close();
};
}
#endif /*SOCKET_SERVER_H */
