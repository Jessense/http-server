#include<iostream>
#include "lib/EventLoop.h"
using namespace std;

void threadFunc(EventLoop &eventLoop)
{
    cout << "thread id: " << this_thread::get_id() << endl;
    eventLoop.loop();
}

/** call loop outside owner thread will fail */
int main()
{
    EventLoop eventLoop;
    thread t(threadFunc, std::ref(eventLoop));
    t.join();
    return 0;
}