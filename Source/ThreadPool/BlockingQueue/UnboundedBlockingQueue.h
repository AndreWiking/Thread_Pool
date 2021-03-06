
#ifndef UNBOUNDED_BLOCKING_QUEUE_H
#define UNBOUNDED_BLOCKING_QUEUE_H

#include <deque>
#include <optional>
#include <mutex>

template <typename T>
class UnboundedBlockingQueue {
public:
    UnboundedBlockingQueue() = default;

    void Push(T &&value) {
        std::lock_guard<std::mutex> lock(mutex_);

        buffer_.push_back(std::forward<T>(value));
        ++buffer_size_;
        not_empty_queue_.notify_one();

    }

    std::optional<T> Pop() {
        std::unique_lock<std::mutex> ulock(mutex_);

        if (is_cancel_ && buffer_.empty()){
            return std::nullopt;
        }

        if (buffer_.empty()) {
            not_empty_queue_.wait(ulock, [this] {
                return !buffer_.empty() || is_cancel_;
            });
        }

        if (is_cancel_ && buffer_.empty()) {
            return std::nullopt;
        }

        T value{std::move(buffer_.front())};
        buffer_.pop_front();
        --buffer_size_;

        return value;
    }

    void Cancel() {
        std::lock_guard<std::mutex> lock(mutex_);
        is_cancel_ = true;
        buffer_.clear();
        buffer_size_ = 0;
        not_empty_queue_.notify_all();
    }

    bool IsEmpty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.empty();
    }
private:
    std::deque<T> buffer_;
    size_t buffer_size_{0};
    std::mutex mutex_;
    std::condition_variable not_empty_queue_;
    bool is_cancel_{false};
};


#endif //UNBOUNDED_BLOCKING_QUEUE_H
