
#include "ThreadPool.h"

static ThreadPool *pool = nullptr;

ThreadPool* ThreadPool::Current() {
    return pool;
}

ThreadPool::ThreadPool(size_t workers_count) {
    for (size_t i = 0; i < workers_count; ++i) {
        workers_.emplace_back([this](){
            pool = this;
            Work();
        });
        //workers_.emplace_back(&ThreadPool::Work, this);
    }
}

void ThreadPool::Stop() {
    is_stopped_ = true;
    task_queue_.Cancel();
    for (auto &worker : workers_){
        worker.join();
    }
}
