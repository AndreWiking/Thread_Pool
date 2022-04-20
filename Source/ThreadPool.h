
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
    explicit ThreadPool(size_t workers_count) {
        for (size_t i = 0; i < workers_count; ++i) {
            workers_.emplace_back(&ThreadPool::Work, this);
        }
    }
    ~ThreadPool() {
        assert(is_stopped_);
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator =(const ThreadPool&) = delete;

    std::future<int> AddTask(std::function<int()> task) {
        empty_task_queue_.notify_one();
        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        task_queue_.Push({std::move(task), std::move(promise)});

        return std::move(future);
    }

    void Join() {
        std::unique_lock<std::mutex> ulock(mutex_);

        empty_task_queue_.wait(ulock, [this] {
           return task_queue_.IsEmpty();
        });
    }

    void Stop() {
        is_stopped_ = true;
        task_queue_.Cancel();
        for (auto &worker : workers_){
            worker.join();
        }
    }

    void RunTest() {
//        std::promise<int> pr;
//        std::future<int> f1 = pr.get_future();
//        pr.set_value((*task_queue_.Pop())());
//        std::cout << "return " << f1.get() << '\n';
    }

private:
    void Work() {
        while (true) {
            std::optional<std::pair<std::function<int()>, std::promise<int>>> top_element(task_queue_.Pop());
            if (top_element.has_value()) {
                int val = (top_element->first)();
                top_element->second.set_value(val);
                empty_task_queue_.notify_one();
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


#endif //THREAD_POOL_H











