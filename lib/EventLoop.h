#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "Channel.h"
#include "EPoller.h"

#include<thread>
#include<vector>
#include<map>

class EventLoop
{
private:
    std::thread::id tid;
    bool looping;
    bool quit;
    std::unique_ptr<EPoller> epoller;
public:
    static const int kEPollTimeoutMs = 10000;
    EventLoop();
    ~EventLoop();
    void loop();
    void quitLoop();
    void addChannel(Channel* channel);
    void modifyChannel(Channel* channel);
    void removeChannel(Channel* channel); 

    std::thread::id getTid() const {return tid;}
};


#endif