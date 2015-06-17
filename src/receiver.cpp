#include <socket_connection/receiver.h>
#include <iostream>

namespace socket_connection{
Receiver::Receiver():bytesForSize(4),bufferIndexRead(0),bufferIndexWrite(0),lastReadByteCount(0){
}


char* Receiver::getReadBuffer(){
    return &buffer[bufferIndexRead];
}

char* Receiver::getWriteBuffer(){
    return &buffer[bufferIndexWrite];
}

void Receiver::addedBytes(int bytesAdded){
    bufferIndexWrite += bytesAdded;
}


bool Receiver::receivedMessage(){
    std::cout << "######################### bufWrite: " <<bufferIndexWrite << std::endl;
    //number of bytes available atm
    int bytesAvailable = bufferIndexWrite-bufferIndexRead;
    if(bytesForSize == 0){
        if(bytesAvailable> 0){
            lastReadByteCount = bytesAvailable;
            lastReadPointer = &buffer[bufferIndexRead];
            std::cout << "return true 1" << std::endl;
            return true;
        }
    }
    std::cout << "current bufferIndexRead: " <<bufferIndexRead <<std::endl;
    /*
     * check if you try to read more bytes then the size of the buffer
     * Maybe not that usefull as their might be a segfault before
     */
    if(bytesAvailable > bufferSize){
        //TODO error-handling
        return false;
    }
    if (bytesAvailable > bytesForSize){
        //get the size
        std::uint32_t size = 0;
        memcpy(&size,buffer,bytesForSize);
        std::cout << "bytesAvailable: " << bytesAvailable << " bytesNeeded " << size <<std::endl;
        int av = bytesAvailable-bytesForSize;
        if(av < 0 || size > (uint) bytesAvailable-bytesForSize){
            //packet doesn't fit, wait for more data
            std::cout << "packet doesn't fit, wait for more data" << std::endl;
            return false;
        }
        //data package is valid, it could be parsed
        //set values for reading the bufferpart afterwards
        lastReadByteCount = size;
        lastReadPointer = &buffer[bufferIndexRead+bytesForSize];
        //set the new index that will be used for reading the next part
        bufferIndexRead += size + bytesForSize;
        //check if the bufferIndecies can be reset
        if(bufferIndexRead == bufferIndexWrite){
            bufferIndexRead = 0;
            bufferIndexWrite = 0;
            std::cout << "bufferindices are the same"<<std::endl;

        }
        if(bufferIndexRead > bufferSize*0.75){
            //TODO copy buffer to start
            memcpy(buffer,&buffer[bufferIndexRead],bufferIndexWrite - bufferIndexRead);
        }
        std::cout << "set current bufferindex: " << bufferIndexRead <<std::endl;
        return true;
    }else{
        std::cout<<"not enough bytes for size (>): bAvail | bForSize: " << bytesAvailable << " | " << bytesForSize << std::endl;
    }
    return false;
}

bool Receiver::receivedMessage(int bytesRead){
    addedBytes(bytesRead);
    return receivedMessage();
}


char * Receiver::getLastReadPointer(){
    return lastReadPointer;
}

int Receiver::getLastReadCount(){
    return lastReadByteCount;
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
    return bufferSize-bufferIndexWrite;
}
}
