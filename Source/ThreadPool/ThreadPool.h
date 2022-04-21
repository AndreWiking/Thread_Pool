
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "UnboundedBlockingQueue.h"
#include <cstdio>
#include <functional>
#include <thread>
#include <vector>
#include <iostream>
#include <future>

class ThreadPool {
public:
    explicit ThreadPool(size_t workers_count);
    ~ThreadPool() {
        assert(is_stopped_);
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator =(const ThreadPool&) = delete;

    template<typename T>
    std::future<T> AddTask(std::function<T()> task) {
        std::promise<T> promise;
        std::future<T> future = promise.get_future();
        task_queue_.Push({std::move(task), std::move(promise)});

        return std::move(future);
    }

    void Join() {
        std::unique_lock<std::mutex> ulock(mutex_);

        empty_task_queue_.wait(ulock, [this] {
           return task_queue_.IsEmpty();
        });
    }

    void Stop();

    static ThreadPool *Current();

private:
    void Work() {
        while (true) {
            std::optional<std::pair<std::function<int()>, std::promise<int>>> top_element(task_queue_.Pop());
            std::function<int()> &function = top_element->first;
            std::promise<int> &promise = top_element->second;
            bool set_promise = false;
            if (top_element.has_value()) {
                int val = 0;
                try {
                    val = function();
                }
                catch (const std::exception&) {
                    promise.set_exception(std::current_exception());
                    set_promise = true;
                }
                catch (...) {}
                if (!set_promise) {
                    promise.set_value(val);
                }
                empty_task_queue_.notify_all();
            }
            else {
                break;
            }
        }
    }

private:
    bool is_stopped_{false};
    std::vector<std::thread> workers_;
    UnboundedBlockingQueue<std::pair<std::function<int()>, std::promise<int>>> task_queue_{};
    std::condition_variable empty_task_queue_;
    std::mutex mutex_;
};

inline ThreadPool* Current() {
    return ThreadPool::Current();
}

#endif //THREAD_POOL_H











