#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>

class WorkerThreadPool
{
private:
    int thread_num;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> task_queue;
    std::mutex mutex;
    std::condition_variable cond;
    bool stop;
public:
    WorkerThreadPool(int thread_num_=8);
    ~WorkerThreadPool();
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));
        std::function<return_type()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(func);
        std::function<void()> wrapper_func = [task_ptr]()
        {
            (*task_ptr)();
        };
        {
            std::unique_lock<std::mutex> lock(mutex);
            task_queue.push(wrapper_func);
        }
        cond.notify_one();
        return task_ptr->get_future();
    };
};
