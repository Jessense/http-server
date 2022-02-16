#include<thread>
class EventLoop
{
private:
    std::thread::id tid;
    bool looping;
public:
    EventLoop();
    ~EventLoop();
    void loop();
};


