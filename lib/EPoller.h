#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>
#include <map>
#include <sys/epoll.h>

typedef std::vector<epoll_event> EventList;
typedef std::map<int, Channel*> ChannelMap;

class Channel;
class EventLoop;

class EPoller
{
private:
    EventLoop* ownerLoop;
    int epfd;
    EventList revents; //保存每次 epoll_wait 返回的 events
    ChannelMap channelMap;
public:
    static const int kInitREventsSize = 16;
    EPoller(EventLoop* loop);
    ~EPoller();
    void poll(int timeoutMs);
    void addChannel(Channel* channel);
    void modifyChannel(Channel* channel);

    void removeFd(int fd);
};




#endif