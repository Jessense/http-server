#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<thread>
#include<vector>
#include<map>

class Channel;
class EPoller;
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
};


#endif