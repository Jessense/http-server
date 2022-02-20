#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "EventLoop.h"

#include <vector>

class ThreadPool
{
private:
    EventLoop* mainLoop;
    std::vector<EventLoop*> subLoops;
    EventLoop* tempLoop;
    bool started;
    int threadNum;
    int next; //下次选择的线程
    pthread_mutex_t mutex;
    pthread_cond_t cond;
public:
    ThreadPool(EventLoop* mainLoop_, int threadNum);
    ~ThreadPool();
    void start();
    EventLoop* getNextLoop();
    void eventLoopThreadRun();
};


#endif