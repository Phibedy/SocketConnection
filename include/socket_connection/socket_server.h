#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <aio.h>
#include <netinet/in.h>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>
class SocketServer:public SocketConnector {

public:
    SocketServer(lms::logging::Logger *parentLogger);
    char buffer[1000*100];

private:

    lms::logging::ChildLogger logger;

    std::vector<SocketConnector> clients;

    /**
     * TODO same as in client
     * @brief listenToFiles
     * @return
     */
    bool listenToFiles();
	void checkNewConnections();
    void checkNewMessages();

public:

    void start(int port);
    void addClient(SocketConnector &client);
    void sendMessageToAllClients(const void *buffer, int bytesToSend);
	void cycle();
    void dispose();
};

#endif /*SOCKET_SERVER_H */
