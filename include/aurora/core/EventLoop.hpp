#pragma once
// Aurora UI - Event loop with posted events & timers (QEventLoop/QTimer analogue)
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

namespace aui {

class EventLoop {
public:
    static EventLoop& instance() { static EventLoop l; return l; }

    using Fn = std::function<void()>;
    void post(Fn fn) {
        {
            std::lock_guard lk(mutex_);
            queue_.push(std::move(fn));
        }
        cv_.notify_one();
    }

    int startTimer(int intervalMs, Fn fn) {
        std::lock_guard lk(mutex_);
        timers_.push_back({++nextTimerId_, std::chrono::steady_clock::now() +
                               std::chrono::milliseconds(intervalMs),
                           intervalMs, std::move(fn)});
        cv_.notify_one();
        return nextTimerId_;
    }
    void killTimer(int id) {
        std::lock_guard lk(mutex_);
        std::erase_if(timers_, [id](const Timer& t) { return t.id == id; });
    }

    int exec() {
        running_ = true;
        while (running_) {
            Fn fn;
            std::vector<Timer> due;
            {
                std::unique_lock lk(mutex_);
                cv_.wait(lk, [&] {
                    return !running_ || !queue_.empty() || anyDue();
                });
                if (!running_) break;
                if (!queue_.empty()) { fn = std::move(queue_.front()); queue_.pop(); }
                auto now = std::chrono::steady_clock::now();
                for (auto& t : timers_)
                    if (t.deadline <= now) { due.push_back(t); t.deadline = now + std::chrono::milliseconds(t.intervalMs); }
            }
            if (fn) fn();
            for (auto& t : due) t.fn();
        }
        return exitCode_;
    }
    void exit(int code = 0) {
        {
            std::lock_guard lk(mutex_);
            running_ = false;
            exitCode_ = code;
        }
        cv_.notify_all();
    }
    /// Drain everything currently pending (used by headless tests).
    void processEvents() {
        for (;;) {
            Fn fn;
            {
                std::lock_guard lk(mutex_);
                if (queue_.empty()) break;
                fn = std::move(queue_.front());
                queue_.pop();
            }
            fn();
        }
    }
private:
    struct Timer {
        int id;
        std::chrono::steady_clock::time_point deadline;
        int intervalMs;
        Fn fn;
    };
    bool anyDue() const {
        if (timers_.empty()) return false;
        auto now = std::chrono::steady_clock::now();
        for (auto& t : timers_) if (t.deadline <= now) return true;
        return false;
    }
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<Fn> queue_;
    std::vector<Timer> timers_;
    int nextTimerId_ = 0;
    bool running_ = false;
    int exitCode_ = 0;
};

class Timer {
public:
    Timer() = default;
    explicit Timer(int intervalMs) { start(intervalMs); }
    ~Timer() { stop(); }
    void start(int intervalMs) {
        stop();
        id_ = EventLoop::instance().startTimer(intervalMs, [this] { timeout.emit(); });
    }
    void stop() { if (id_) { EventLoop::instance().killTimer(id_); id_ = 0; } }
    bool active() const { return id_ != 0; }
    Signal<> timeout;
private:
    int id_ = 0;
};

} // namespace aui
