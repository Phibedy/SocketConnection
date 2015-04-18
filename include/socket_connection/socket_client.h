#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#include <aio.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>

namespace socket_connection{
/**
 * @brief The SocketClient class basic client for socket-connections
 * TODO Don't like the current structure of the socket-connection-stuff. Even if I wrote it :)
 */
class SocketClient:public SocketConnector {

public:
    SocketClient(lms::logging::Logger *parentLogger);
private:

    lms::logging::ChildLogger logger;
    std::vector<SocketConnector> servers;
    SocketListener* listener;
    /**
     * @brief listenToFiles listens to all files, returns true if data was written into a file
     * @return
     */
    bool listenToFiles();
    /**
     * @brief checkNewMessages checks the files for new messages, called if listenToFiles returned true
     */
    void checkNewMessages();

public:
    /**
     * @brief connectToServer
     * @param address
     * @param port
     * @return true if the connecting was successfull
     */
    bool connectToServer(std::string address,int port);
    /**
     * @brief cycleClient
     */
    void cycleClient();
    void setSocketListener(SocketListener *listener);

    void sendMessageToAllServers(const void *buffer, int bytesToSend, bool addSize);
};
}
#endif /*SOCKET_CLIENT_H */
