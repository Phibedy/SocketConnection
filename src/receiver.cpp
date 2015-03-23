#include <socket_connection/receiver.h>
Receiver::Receiver():bytesForSize(4){
}


char* Receiver::getReadBuffer(){
    return &buffer[currentBufferIndex];
}

void Receiver::receivedMessage(int bytesRead,void (*listener)(char *, int)){
    //number of bytes available atm
    if(bytesForSize == 0){
        listener(buffer,currentBufferIndex+bytesRead);
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
            return;
        }
        //throw event
        listener(&buffer[bytesForSize], size);
        //one complete object was read, decrease the bufferIndex
        currentBufferIndex -= bytesForSize+size;
    }

}
