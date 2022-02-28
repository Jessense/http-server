#ifndef UTILS_H
#define UTILS_H

#include "Buffer.h"

#include <iostream>
#include <thread>

void showBuffer(Buffer *buffer, int connect_fd)
{
    std::string s = "";
    for (int i = buffer->readIndex; i < buffer->writeIndex; i++) {
        s += buffer->data[i];
    }
    std::cout << std::this_thread::get_id() << " : " << connect_fd << " : " << s << std::endl;    
}

#endif