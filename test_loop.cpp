#include<iostream>
#include "lib/EventLoop.h"
using namespace std;

void threadFunc()
{
    cout << "thread id: " << this_thread::get_id() << endl;
    EventLoop eventLoop;
    eventLoop.loop();
}

int main()
{
    EventLoop eventLoop;
    thread t(threadFunc);
    threadFunc();
    t.join();
    return 0;
}