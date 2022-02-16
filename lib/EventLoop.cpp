#include "EventLoop.h"

#include <iostream>
#include <assert.h>
#include <poll.h>
EventLoop::EventLoop(): tid(pthread_self()), looping(false)
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
    assert(!looping);
    assert(tid == std::this_thread::get_id());
    looping = true;
    std::cout << "loop owner id: " << tid << std::endl;
    poll(NULL, 0, 5*1000);
    std::cout << "loop end" << std::endl;
    looping = false;
}