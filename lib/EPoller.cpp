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
            if (channelMap.find(fd) == channelMap.end()) {
                // std::cout << ownerLoop->getTid() << " : " << fd << " : channel not found" << std::endl;
                continue;
            }
            Channel* channel = channelMap[fd];
            assert(channel->fd == fd);
            if ((revents[i].events & EPOLLERR) || (revents[i].events & EPOLLHUP))
            {
                // std::cout << ownerLoop->getTid() << " : " << channel->fd << " : EPOLLERR | EPOLLHUP" << std::endl;
                if (channel && channel->closeCallback && channel->data)
                channel->closeCallback(channel->data);
            }
            if (revents[i].events & EPOLLIN) {
                // std::cout << ownerLoop->getTid() << " : " << channel->fd << " : EPOLLIN" << std::endl;
                if (channel && channel->readCallback &&  channel->data)
                channel->readCallback(channel->data);
            }
            if (revents[i].events & EPOLLOUT) {
                // std::cout << ownerLoop->getTid() << " : " << channel->fd << " : EPOLLOUT" << std::endl;
                if (channel && channel->writeCallback && channel->data)
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
    int err = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    channelMap.erase(fd);

}