#include "EventLoop.h"
#include "EPoller.h"

#include <iostream>
#include <assert.h>

const int kEPollTimeoutMs = 10000;

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
    std::cout << "loop owner id: " << tid << std::endl;
    
    while (!quit) {
        epoller->poll(kEPollTimeoutMs);
    }

    std::cout << "loop end" << std::endl;
    looping = false;
}

void EventLoop::quitLoop()
{
    quit = true;
}