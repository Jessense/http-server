#include "EventLoop.h"
#include <sys/epoll.h>
#include <iostream>

Channel::Channel(int fd_, int events_, EventLoop* loop, EventReadCallback readCallback_, 
    EventWriteCallback writeCallback_, void* data_)
    : fd(fd_),
      events(events_),
      ownerLoop(loop),
      readCallback(readCallback_),
      writeCallback(writeCallback_),
      data(data_)
{
    ownerLoop->addChannel(this);
} 

Channel::~Channel()
{
    std::cout << "~Channel()" << std::endl;
}



void Channel::enableRead()
{
    events |= EPOLLIN;
    update();
}

void Channel::disableRead()
{
    events &= ~EPOLLOUT;
    update();
}

void Channel::enableWrite()
{
    events |= EPOLLOUT;
    update();
}

void Channel::disableWrite()
{
    events &= ~EPOLLOUT;
    update();
}

bool Channel::writeEnabled()
{ 
    return events & EPOLLOUT; 
}

void Channel::update()
{
    ownerLoop->modifyChannel(this);   
}

void Channel::deregister()
{
    ownerLoop->removeChannel(this);
}
