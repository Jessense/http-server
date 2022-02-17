#include "lib/Channel.h"
#include "lib/EventLoop.h"

#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>

EventLoop* g_loop;

void timeout()
{
    std::cout << "timeout!" << std::endl;
    g_loop->quitLoop();
}

int main(int argc, char const *argv[])
{
    EventLoop loop;
    g_loop = &loop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(timerfd, &loop);
    channel.setReadCallback(timeout);

    struct itimerspec spec =
    {
        { 1, 0 }, // Set to {0, 0} if you need a one-shot timer
        { 1, 0 }
    };
    timerfd_settime(timerfd, 0, &spec, NULL);

    loop.loop();

    close(timerfd);

    return 0;
}
