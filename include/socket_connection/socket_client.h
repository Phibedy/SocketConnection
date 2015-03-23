#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H
#include <aio.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <lms/logger.h>
#include <socket_connection/socket_connector.h>
class SocketClient:public SocketConnector {

public:
    SocketClient(lms::logging::Logger *parentLogger);
private:

    lms::logging::ChildLogger logger;
    std::vector<SocketConnector> servers;
    SocketListener* listener;

    bool listenToFiles();
	void checkNewMessages();
	void error(const char *msg);

public:

    void connectToServer(std::string address,int port);
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
