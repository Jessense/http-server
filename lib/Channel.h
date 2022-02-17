#ifndef CHANNEL_H
#define CHANNEL_H

#include "EventLoop.h"

#include <functional>

typedef std::function<void()> Callback;
class Channel
{
public:
    int fd;
    int events;
    EventLoop* ownerLoop;
    Callback readCallback;
    Callback writeCallback;
public:
    Channel(int _fd, EventLoop* loop);
    ~Channel();
    void handleEvent();
    void setReadCallback(const Callback& cb);
};


#endif
