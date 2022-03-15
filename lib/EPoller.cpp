#include "EventLoop.h"

#include <iostream>
#include <assert.h>
#include <unistd.h>

EPoller::EPoller(EventLoop* loop)
    : ownerLoop(loop),
      epfd(epoll_create1(0)),
      revents(kInitREventsSize),
      channelMap()
{

}

EPoller::~EPoller()
{
    close(epfd);
}

void EPoller::poll(int timeoutMs)
{
    int numEvents = epoll_wait(epfd, &*revents.begin(), kInitREventsSize, timeoutMs);
    if (numEvents > 0)
    {
        for (int i = 0; i < numEvents; i++) {
            int fd = revents[i].data.fd;
            Channel* channel = channelMap[fd];
            assert(channel->fd == fd);
            if ((revents[i].events & EPOLLERR) || (revents[i].events & EPOLLHUP))
            {
                std::cout << "epoll error" << std::endl;
                continue;
            }
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
    epoll_ctl(epfd, EPOLL_CTL_ADD, channel->fd, &event);
}

void EPoller::modifyChannel(Channel* channel)
{
    epoll_event event;
    event.data.fd = channel->fd;
    event.events = channel->events;
    
    channelMap[channel->fd] = channel;
    epoll_ctl(epfd, EPOLL_CTL_MOD, channel->fd, &event);    
}

void EPoller::removeFd(int fd)
{
    channelMap.erase(fd);
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}