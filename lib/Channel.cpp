#include "Channel.h"
#include <sys/epoll.h>
#include <iostream>

Channel::Channel(int _fd, EventLoop* loop)
    :fd(_fd), ownerLoop(loop)
{
};

Channel::~Channel()
{
    std::cout << "~Channel()" << std::endl;
}

void Channel::setReadCallback(EventReadCallback cb, void* data_)
{
    data = data_;
    readCallback = cb;
    events |= EPOLLIN;
    ownerLoop->addChannel(this);
}
