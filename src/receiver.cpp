#include <socket_connection/receiver.h>
Receiver::Receiver():bytesForSize(4),lastRead(0){
}


char* Receiver::getReadBuffer(){
    return &buffer[currentBufferIndex];
}

bool Receiver::receivedMessage(int bytesRead){
    //number of bytes available atm
    if(bytesForSize == 0){
        if(bytesRead > 0){
            lastRead = bytesRead;
            return true;
        }
    }
    int totalBytes = currentBufferIndex+bytesRead;
    /*
     * check if you try to read more bytes then the size of the buffer
     * Maybe not that usefull as their might be a segfault before
     */
    if(totalBytes > bufferSize){
        //TODO error-handling
    }
    while (totalBytes > bytesForSize){
        //get the size
        std::uint32_t size = 0;
        memcpy(&size,buffer,bytesForSize);
        if(size > bytesRead-bytesForSize){
            //packet doesn't fit, wait for more data
            currentBufferIndex = totalBytes;
            return false;
        }
        //throw event
        lastRead = size;
        //one complete object was read, decrease the bufferIndex
        currentBufferIndex -= bytesForSize+size;
        return true;
    }
}


char * Receiver::getReadStart(){
    return &buffer[bytesForSize];
}

int Receiver::getLastReadCount(){
    return lastRead;
}

void Receiver::setBuffer(char *buffer, int size){
    this->buffer = buffer;
    this->bufferSize = size;
}

int Receiver::getBytesForSize(){
    return bytesForSize;
}

void Receiver::setBytesForSize(int size){
    this->bytesForSize = size;
}

int Receiver::getBufferSpace(){
    return bufferSize-currentBufferIndex;
}
