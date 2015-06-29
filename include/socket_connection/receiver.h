#ifndef SOCKET_CONNECTION_RECEIVER_H
#define SOCKET_CONNECTION_RECEIVER_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <cstdint>
namespace socket_connection{
class Receiver {

    /**
     * @brief bytesForSize how many bytes are in front of the message that define the size of the sent object
     */
    int bytesForSize;
    /**
     * @brief buffer pointer to the first buffer
     */
    char *buffer;
    /**
     * @brief bufferSize size of the buffer
     */
    int bufferSize;
    /**
     * @brief currentBufferIndex index of the current buffer to read from
     */
    int bufferIndexRead;
    /**
     * @brief bufferIndexWrite index of the current buffer for writting to it
     */
    int bufferIndexWrite;
    /**
     * @brief lastRead number of bytes read in the last duration
     */
    int lastReadByteCount;
    /**
     * @brief lastReadPointer pointing to the last element that was read
     */
    char *lastReadPointer;

public:
    Receiver();

    void reset();

    /**
     * @brief getReadBuffer
     * @return the buffer at the current position for reading
     */
    char* getReadBuffer();

    /**
     * @brief getWriteBuffer
     * @return the buffer at the current position for writing
     */
    char* getWriteBuffer();
    /**
     * @brief receivedMessage same as calling addedBytes() + receivedMessage()
     * @param bytesRead
     */
    bool receivedMessage(int bytesRead);

    /**
     * @brief receivedMessage call this method if new bytes were read into the buffer
     * @param bytesRead
     */
    bool receivedMessage();

    /**
     * @brief addedBytes will increase the bufferIndexWrite
     * @param bytesAdded number of bytes read into the buffer
     */
    void addedBytes(int bytesAdded);

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
    char * getLastReadPointer();
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
    /**
     * @brief getBufferSpace
     * @return space left in the buffer
     */
    int getBufferSpace();


};
}
#endif /*RECEIVER_H */
