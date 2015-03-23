#include <socket_connection/socket_connector.h>
#include <string>
#include <arpa/inet.h>    //close

SocketConnector::SocketConnector():fileDescriptor(0),connected(false),address(""),port(0){
    bzero(&timeout,sizeof(timeout));
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

bool SocketConnector::sendMessage(const void *buffer, int bytesToSend) {
    if(isConnected()){
        int n = write(fileDescriptor, buffer, bytesToSend);
        if (n < 0) {
            perror("sendMessage\n");
        }
    }else{
        //fucked again
        perror("not connected (sendMessage)\n");
    }
}


void SocketConnector::close(){
    setConnected(false);
    //close(getFileDescriptor());

}
