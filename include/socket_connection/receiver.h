#ifndef RECEIVER_H
#define RECEIVER_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <cstdint>

class Receiver {

    /**
     * @brief bytesForSize how many bytes are in front of the message that define the size of the sent object
     */
    int bytesForSize;
    char *buffer;
    int bufferSize;
    int currentBufferIndex;
    int lastRead;

public:
    Receiver();

    /**
     * @brief getReadBuffer
     * @return the buffer at the current position
     */
    char* getReadBuffer();

    /**
     * @brief receivedMessage call this method if new bytes were read into the buffer
     * @param bytesRead
     */
    bool receivedMessage(int bytesRead);

    void setBuffer(char *buffer, int size){
        this->buffer = buffer;
        this->bufferSize = size;
    }

    char * getReadStart();
    int getLastReadCount();

};
#endif /*RECEIVER_H */
