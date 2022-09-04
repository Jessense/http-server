#include "WorkerThreadPool.h"

WorkerThreadPool::WorkerThreadPool(int thread_num_)
    : thread_num(thread_num_),
      stop(false)
{
    for (int i = 0; i < thread_num; i++)
    {
        threads.emplace_back([this]()
                             {
            while (!this->stop) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->mutex);
                    while (this->task_queue.empty() && !this->stop) //必须是 while，避免假唤醒
                    {
                        this->cond.wait(lock);
                    }
                    // this->cond.wait(lock, [this](){return this->stop || !this->task_queue.empty();}); //这种方法与上面这种方向效果类似
                    if (this->stop && this->task_queue.empty()) {
                        return;
                    }
                    task = std::move(this->task_queue.front()); //为什么要 std::move？
                    this->task_queue.pop();
                }
                task();
                
            } });
    }
}

WorkerThreadPool::~WorkerThreadPool()
{
    stop = true;
    cond.notify_all();
    for (int i = 0; i < thread_num; i++)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
}