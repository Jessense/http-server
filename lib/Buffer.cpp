#include "Buffer.h"

#include <stdlib.h>
#include <sys/uio.h>
#include <string.h>
Buffer::Buffer()
{
    data = (char*)malloc(sizeof(kInitBufferSize));
    totalSize = kInitBufferSize;
    readIndex = 0;
    writeIndex = 0;
}

Buffer::~Buffer()
{
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
int Buffer::append(char* data, int size)
{
    if (data != NULL) {
        makeRoom(size);
        memcpy(data+writeIndex, data, size);
        writeIndex += size;
    }
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
    char additionalBuffer[kInitBufferSize];
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