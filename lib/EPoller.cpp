#include "EPoller.h"

#include <iostream>

EPoller::EPoller(EventLoop* loop)
    : ownerLoop(loop),
      epfd(epoll_create1(0)),
      revents(kInitREventsSize)
{

}

EPoller::~EPoller()
{
}

void EPoller::poll(int timeoutMs)
{
    int numEvents = epoll_wait(epfd, &*revents.begin(), kInitREventsSize, timeoutMs);
    std::cout << "numEvents=" << numEvents << std::endl;
    if (numEvents > 0)
    {
        for (int i = 0; i < numEvents; i++) {
            Channel* channel = channelMap[revents[i].data.fd];
            if (revents[i].events & EPOLLIN) {
                channel->readCallback();
            }
            if (revents[i].events & EPOLLOUT) {
                channel->writeCallback();
            }
        }
    }

}

void EPoller::addChannel(Channel* channel)
{
    epoll_event event;
    event.data.fd = channel->fd;
    event.events = channel->events;
    
    epoll_ctl(epfd, EPOLL_CTL_ADD, channel->fd, &event);
    channelMap[channel->fd] = channel;
}