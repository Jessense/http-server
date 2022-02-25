#include "EventLoop.h"
#include "EPoller.h"

#include <iostream>
#include <assert.h>



EventLoop::EventLoop()
    : tid(std::this_thread::get_id()), 
      looping(false),
      quit(false),
      epoller(new EPoller(this))
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
    std::cout << "started loop in thread " << std::this_thread::get_id() << std::endl;
    while (!quit) {
        epoller->poll(kEPollTimeoutMs);
    }

    looping = false;
}

void EventLoop::quitLoop()
{
    quit = true;
}

void EventLoop::addChannel(Channel* channel)
{
    epoller->addChannel(channel);
}

void EventLoop::modifyChannel(Channel* channel)
{
    epoller->modifyChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    epoller->removeFd(channel->fd);
}


