#pragma once

#include <string.h>
#include <assert.h>
class FixedBuffer
{
private:
    int capacity = 4096;
    int offset;
    char *data;
public:
    FixedBuffer() : offset(0),
                    data(new char[capacity]){}

    ~FixedBuffer()
    {
        delete[] data;
    }

    void append(const char *str, int len)
    {
        assert(available() >= len);
        memcpy(data + offset, str, len);
        offset += len;
    }

    int available() { return capacity - offset; }

    char* get_data() { return data; }

    int get_size() { return offset; }

    void reset()
    {
        offset = 0;
    }
};
