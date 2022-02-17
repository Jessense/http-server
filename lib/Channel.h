#ifndef CHANNEL_H
#define CHANNEL_H

#include "EventLoop.h"

#include <functional>

typedef int (*EventReadCallback)(void *data);
typedef int (*EventWriteCallback)(void *data);

class Channel
{
public:
    int fd;
    int events;
    EventLoop* ownerLoop;
    EventReadCallback readCallback;
    EventWriteCallback writeCallback;
    void *data;
public:
    Channel(int _fd, EventLoop* loop);
    ~Channel();
    void handleEvent();
    void setReadCallback(EventReadCallback cb, void* data);
};


#endif
