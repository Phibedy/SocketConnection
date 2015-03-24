#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#include <aio.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>
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
//	void error(const char *msg);

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
    /**
     * @brief sendMessage Die Reihenfolge ist wichtig von pointers und sizeOfObjects pointers[i] <-> sizeOfObjects[i]
     * @param sockfd file descriptor
     * @param pointers pointing to all objects that will be send
     * @param sizeOfObjects size of each object
     * @param numberOfObjects
     */
   // void sendMessage(int sockfd,int* pointers, int*sizeOfObjects, int numberOfObjects);
	void sendMessage(const void *buffer, int bytesToSend);
};

#endif /*SOCKET_CLIENT_H */
