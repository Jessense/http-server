#include "Buffer.h"

#include <stdlib.h>
#include <sys/uio.h>
#include <string.h>
#include <iostream>

const char *CRLF = "\r\n";

Buffer::Buffer()
    : data(new char[kInitBufferSize]),
      totalSize(kInitBufferSize),
      readIndex(0),
      writeIndex(0)
{
}

Buffer::~Buffer()
{
    delete data;
}

void Buffer::makeRoom(int size)
{
    int writeable = writeableSize();
    if (writeable >= size)
        return;
    int readable = readableSize();
    int frontSpare = frontSpareSize();
    if (frontSpare + writeable >= size)
    {
        for (int i = 0; i < readable; i++) {
            data[i] = data[i+frontSpare];
        }
        readIndex = 0;
        writeIndex = readable;
    }
    else
    {
        char *temp = (char*)realloc(data, totalSize+size); //realloc可能会把data中的数据复制到新地址
        if (temp == NULL)
            return;
        data = temp;
        totalSize += size;
    }
}

/** 将 data 中的数据（大小size）append到 buffer 中 */
int Buffer::append(char* data_, int size)
{
    if (data_ != NULL) {
        makeRoom(size);
        memcpy(data+writeIndex, data_, size);
        writeIndex += size;
    }
    return 0;
}

int Buffer::appendString(std::string s)
{
    char* temp = new char[s.size()+1];
    strcpy(temp, s.c_str());
    append(temp, s.size());
    return 0;
}

int Buffer::appendChar(char c)
{
    makeRoom(1);
    data[writeIndex++] = c;
    return 0;
}

int Buffer::readSocket(int fd)
{
    char *additionalBuffer = new char[kInitBufferSize];
    iovec vec[2];
    int writeable = writeableSize();
    vec[0].iov_base = data + writeIndex;
    vec[0].iov_len = writeable;
    vec[1].iov_base = additionalBuffer;
    vec[1].iov_len = sizeof(additionalBuffer);
    int wrote = readv(fd, vec, 2);
    if (wrote < 0)
    {
        return -1;
    }
    else if (wrote <= writeable)
    {
        writeIndex += wrote;
    }
    else
    {
        writeIndex = totalSize;
        append(additionalBuffer, wrote-writeable);
    }
    return wrote;
}

char* Buffer::findCRLF()
{
    char *crlf = (char*)memmem(data + readIndex, readableSize(), CRLF, 2);
    return crlf;

}