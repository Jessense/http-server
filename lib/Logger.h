#include "FixedBuffer.h"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>

class Logger
{
private:
    std::unique_ptr<FixedBuffer> current_buffer_;
    std::queue<std::unique_ptr<FixedBuffer>> empty_buffers_;
    std::queue<std::unique_ptr<FixedBuffer>> full_buffers_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;
    bool stop;
    int fd;

public:
    Logger()
        : current_buffer_(new FixedBuffer()),
          stop(false),
          thread_(std::bind(&Logger::threadFunc, this))
    {
        fd = open("log.txt", O_CREAT | O_APPEND | O_RDWR, 0666);
        if (fd == -1)
        {
            printf("errno=%d\n", errno);
        }
        assert(fd != -1);
    }

    ~Logger()
    {
        thread_.join();
        close(fd);
    }

    void append(const char *str, int len)
    {
        std::time_t result = std::time(nullptr);
        char* timestamp = std::asctime(std::localtime(&result));
        current_buffer_->append(timestamp, strlen(timestamp)-1);
        current_buffer_->append(" ", 1);
        std::lock_guard<std::mutex> lock(mutex_);
        if (current_buffer_->available() >= len)
        {
            current_buffer_->append(str, len);
        }
        else
        {
            full_buffers_.emplace(current_buffer_.release());
            if (empty_buffers_.empty())
            {
                current_buffer_.reset(new FixedBuffer());
            }
            else
            {
                current_buffer_.reset(empty_buffers_.front().release());
                empty_buffers_.pop();
            }
            current_buffer_->append(str, len);
            cond_.notify_all();
        }
    }

    void threadFunc()
    {
        using namespace std::chrono_literals;
        while (!stop)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            std::cv_status status = std::cv_status::no_timeout;
            while (full_buffers_.empty() && status == std::cv_status::no_timeout)
            {
                status = cond_.wait_for(lock, 3s);
            }
            std::unique_ptr<FixedBuffer> to_write;
            if (full_buffers_.empty())
            {
                if (current_buffer_->get_size() == 0)
                {
                    continue;
                }
                else
                {
                    to_write.reset(current_buffer_.release());
                    if (empty_buffers_.empty())
                    {
                        current_buffer_.reset(new FixedBuffer());
                    }
                    else
                    {
                        current_buffer_.reset(empty_buffers_.front().release());
                        empty_buffers_.pop();
                    }
                }
            }
            else
            {
                to_write.reset(full_buffers_.front().release());
                full_buffers_.pop();
            }
            lock.unlock(); // 临界区结束

            write(fd, to_write->get_data(), to_write->get_size());
            to_write->reset();

            lock.lock();
            empty_buffers_.emplace(to_write.release());
            lock.unlock();

        }
    }
};
