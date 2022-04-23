
#include "ThreadPool.h"

static ThreadPool *pool = nullptr;

ThreadPool* ThreadPool::Current() {
    return pool;
}

ThreadPool::~ThreadPool() {
    assert(is_stopped_);
}


ThreadPool::ThreadPool(size_t workers_count) {
    for (size_t i = 0; i < workers_count; ++i) {
        workers_.emplace_back([this](){
            pool = this;
            Work();
        });
    }
}

void ThreadPool::Stop() {
    is_stopped_ = true;
    task_queue_.Cancel();
    for (auto &worker : workers_){
        worker.join();
    }
}

void ThreadPool::Join() {
    std::unique_lock<std::mutex> ulock(mutex_);

    empty_task_queue_.wait(ulock, [this] {
        return task_queue_.IsEmpty();
    });
}

void ThreadPool::Work() {
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
