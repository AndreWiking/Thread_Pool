
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "BlockingQueue/UnboundedBlockingQueue.h"
#include <cstdio>
#include <functional>
#include <thread>
#include <vector>
#include <iostream>
#include <future>

//template <typename T>
//class Task{
//public:
//
//    explicit Task(std::function<T> function, std::promise<T> promise) : function_(std::move(function)), promise_(std::move(promise)) {}
//
//    void operator () () {
//        bool set_promise = false;
//        T val{};
//        try {
//            val = function_();
//        }
//        catch (const std::exception&) {
//            promise_.set_exception(std::current_exception());
//            set_promise = true;
//        }
//        catch (...) {}
//        if (!set_promise) {
//            promise_.set_value(val);
//        }
//    }
//
//    Task(const Task&) = delete;
//
//    Task& operator =(const Task&) = delete;
//
//private:
//    std::function<T> function_;
//    std::promise<T> promise_;
//};

class ThreadPool {
public:
    explicit ThreadPool(size_t workers_count);
    ~ThreadPool() {
        assert(is_stopped_);
    }

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
            std::optional<std::function<void()>> top_element(task_queue_.Pop());
            empty_task_queue_.notify_one();
            if (top_element.has_value()) {
                (*top_element)();
            }
            else {
                break;
            }
        }
    }

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


