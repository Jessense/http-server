#include "Channel.h"
#include <sys/epoll.h>
Channel::Channel(int _fd, EventLoop* loop)
    :fd(_fd), ownerLoop(loop)
{
};

Channel::~Channel()
{
}

void Channel::setReadCallback(const Callback& cb)
{
    readCallback = cb;
    events |= EPOLLIN;
    ownerLoop->addChannel(this);
}
