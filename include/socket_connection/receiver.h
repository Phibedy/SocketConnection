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

    /**
     * @brief setBuffer set the buffer used to receive messages
     * @param buffer
     * @param size
     */
    void setBuffer(char *buffer, int size);
    /**
     * @brief getReadStart used to read the data that was collected by the buffer
     * @return
     */
    char * getReadStart();
    /**
     * @brief getLastReadCount
     * @return number of bytes that were collected during the last run or of an complete object
     */
    int getLastReadCount();
    int getBytesForSize();
    /**
     * @brief setBytesForSize can't be bigger then 4!
     * @param size
     */
    void setBytesForSize(int size);
    int getBufferSpace();


};
#endif /*RECEIVER_H */
