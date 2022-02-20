#include "ThreadPool.h"
#include <assert.h>
#include <iostream>
ThreadPool::ThreadPool(EventLoop* mainLoop_, int threadNum_)
    : mainLoop(mainLoop_),
      subLoops(),
      tempLoop(NULL),
      threadNum(threadNum_),
      started(false),
      next(0)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::start()
{
    std::cout << "ThreadPool Starting..." << std::endl;
    for (int i = 0; i < threadNum; i++) {
        std::thread t(&ThreadPool::eventLoopThreadRun, this);
        pthread_mutex_lock(&mutex);
        while (tempLoop == NULL) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        
        subLoops.push_back(tempLoop);
        tempLoop = NULL;

         /* 虽然主线程有循环、不会比子线程早退出，但由于 thread 对象要离开作用域了，
         所以要 join 或 detach，但 join 会阻塞，只能 detach*/
        t.detach();
        
    }

    started = true;
    std::cout << "ThreadPool Started" << std::endl;
}

EventLoop* ThreadPool::getNextLoop()
{
    assert(started);
    EventLoop* selected = mainLoop;

    if (threadNum > 0) {
        selected = subLoops[next];
        next++;
        if (next >= threadNum) {
            next = 0;
        }
    }

    return selected;
}

void ThreadPool::eventLoopThreadRun()
{
    EventLoop* eventLoop = new EventLoop();
    pthread_mutex_lock(&mutex);
    tempLoop = eventLoop;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    std::cout << "initialized     thread " << std::this_thread::get_id() << std::endl;
    eventLoop->loop();
    
}