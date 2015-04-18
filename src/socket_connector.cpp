#include <socket_connection/socket_connector.h>
#include <string>
#include <arpa/inet.h>    //close

namespace socket_connection{
SocketConnector::SocketConnector(int bufferSize):connected(false),bufferSize(bufferSize),
    fileDescriptor(0),listener(nullptr),address(""),port(0){
    /*
     * set server_addr to zero
     */
    bzero((char *) &socket_addr, sizeof(socket_addr));

    adress_length = sizeof(socket_addr);
    bzero(&timeout,sizeof(timeout));
    receiver.setBuffer(new char[bufferSize], bufferSize);
}

Receiver SocketConnector::getReceiver(){
    return receiver;
}

bool SocketConnector::isConnected(){
    return connected;
}


int SocketConnector::getFileDescriptor(){
    return fileDescriptor;
}

void SocketConnector::setFileDescriptor(int f){
    fileDescriptor = f;
}


void SocketConnector::setConnected(bool state){
    this->connected = state;
}

bool SocketConnector::sendMessage(const void *buffer, int bytesToSend, bool addSize) {
    if(isConnected()){
        int n = 0;
        if(addSize && receiver.getBytesForSize() > 0){
            std::cout<< "SIZE I WANT TO SEND: " << bytesToSend << std::endl;
            std::uint32_t size = bytesToSend;
            n = write(fileDescriptor, &size, receiver.getBytesForSize());
        }
        n += write(fileDescriptor, buffer, bytesToSend);
        if (n < 0) {
            perror("sendMessage\n");
            return false;
        }else{
            return true;
        }
    }else{
        //fucked again
        perror("not connected (sendMessage)\n");
        return false;
    }

}

void SocketConnector::setSocketListener(SocketListener *listener){
    this->listener = listener;
}

SocketListener* SocketConnector::getSocketListener(){
    return listener;
}


void SocketConnector::close(){
    setConnected(false);
    //close(getFileDescriptor());
}
}
