#include "EPoller.h"

#include <iostream>
#include <assert.h>

EPoller::EPoller(EventLoop* loop)
    : ownerLoop(loop),
      epfd(epoll_create1(0)),
      revents(kInitREventsSize),
      channelMap()
{

}

EPoller::~EPoller()
{
}

void EPoller::poll(int timeoutMs)
{
    int numEvents = epoll_wait(epfd, &*revents.begin(), kInitREventsSize, timeoutMs);
    std::cout << "numEvents=" << numEvents << ", revents.size=" << revents.size() << std::endl;
    if (numEvents > 0)
    {
        for (int i = 0; i < numEvents; i++) {
            int fd = revents[i].data.fd;
            std::cout << "map count: " << channelMap.count(fd) << std::endl;
            Channel* channel = channelMap[fd];
            std::cout << "channel->fd=" << channel->fd << ", fd=" << fd << std::endl;
            assert(channel->fd == fd);
            if (revents[i].events & EPOLLIN) {
                channel->readCallback(channel->data);
            }
            if (revents[i].events & EPOLLOUT) {
                channel->writeCallback(channel->data);
            }
        }
    }

}

void EPoller::addChannel(Channel* channel)
{
    epoll_event event;
    event.data.fd = channel->fd;
    event.events = channel->events;
    
    channelMap[channel->fd] = channel;
    std::cout << "fd mapped: " << channel->fd << ", " << channelMap[channel->fd]->fd << std::endl;
    epoll_ctl(epfd, EPOLL_CTL_ADD, channel->fd, &event);
    

}