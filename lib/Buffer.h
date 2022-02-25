#ifndef BUFFER_H
#define BUFFER_H

class Buffer
{
public:
    char *data;
    int readIndex;
    int writeIndex;
    int totalSize;
public:
    static const int kInitBufferSize = 65536;
    Buffer();
    ~Buffer();
    char* dataBegin() {return data + readIndex;};
    int writeableSize() {return totalSize - writeIndex;};
    int readableSize() {return writeIndex - readIndex;};
    int frontSpareSize() {return readIndex;};
    int readSocket(int fd);
    int append(char* data, int size);
    int appendChar(char c);
    void makeRoom(int size);
    
};




#endif