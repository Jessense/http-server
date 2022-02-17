#include "EventLoop.h"
#include "EPoller.h"

#include <iostream>
#include <assert.h>



EventLoop::EventLoop()
    : tid(pthread_self()), 
      looping(false),
      quit(false),
      epoller(new EPoller(this))
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::addChannel(Channel* channel)
{
    epoller->addChannel(channel);
}

void EventLoop::loop()
{
    assert(!looping);
    assert(tid == std::this_thread::get_id());
    looping = true;
    
    while (!quit) {
        epoller->poll(kEPollTimeoutMs);
    }

    looping = false;
}

void EventLoop::quitLoop()
{
    quit = true;
}