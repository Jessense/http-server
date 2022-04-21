#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

typedef int (*EventReadCallback)(void *data);
typedef int (*EventWriteCallback)(void *data);
typedef int (*EventCloseCallback)(void *data);

class EventLoop;

class Channel
{
public:
    int fd;
    int events;
    EventLoop* ownerLoop;
    EventReadCallback readCallback;
    EventWriteCallback writeCallback;
    EventCloseCallback closeCallback;
    void *data;
public:
    Channel(int fd_, int events_, EventLoop* loop, EventReadCallback readCallback, EventWriteCallback writeCallback, EventCloseCallback closeCallback, void* data);
    ~Channel();
    void handleEvent();
    void enableRead();
    void disableRead();
    void enableWrite();
    void disableWrite();
    bool writeEnabled();
    void update();
    void deregister();
};


#endif
