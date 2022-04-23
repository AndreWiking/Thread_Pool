
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "BlockingQueue/UnboundedBlockingQueue.h"
#include <cstdio>
#include <functional>
#include <thread>
#include <vector>
#include <iostream>
#include <future>

class ThreadPool {
public:
    explicit ThreadPool(size_t workers_count);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator =(const ThreadPool&) = delete;

    template<typename FuncType, typename... ArgTypes>
    auto AddTask(FuncType &&func, ArgTypes&&... args)->std::future<decltype(func(args...))> {

        auto function_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()>>(
                std::bind(std::forward<FuncType>(func), std::forward<ArgTypes>(args)...));

        task_queue_.Push([function_ptr]() {
            (*function_ptr)();
        });
        return function_ptr->get_future();
    }

    void Join();

    void Stop();

    static ThreadPool *Current();

private:
    void Work();

private:
    bool is_stopped_{false};
    std::vector<std::thread> workers_;
    UnboundedBlockingQueue<std::function<void()>> task_queue_;
    std::condition_variable empty_task_queue_;
    std::mutex mutex_;
};

inline ThreadPool* Current() {
    return ThreadPool::Current();
}

#endif //THREAD_POOL_H


