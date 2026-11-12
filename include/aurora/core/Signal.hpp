#pragma once
// Aurora UI - Signal & Slot system (type-safe, Qt-inspired, header-only)
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace aui {

class Object;

/// Handle to a signal-slot connection. Auto-disconnects on destruction.
class Connection {
public:
    Connection() = default;
    explicit Connection(std::function<void()> disconnectFn)
        : fn_(std::make_shared<std::function<void()>>(std::move(disconnectFn))) {}
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection(Connection&&) noexcept = default;
    Connection& operator=(Connection&&) noexcept = default;
    ~Connection() { disconnect(); }
    void disconnect() {
        if (fn_ && *fn_) { (*fn_)(); fn_->get()->operator=(nullptr); }
    }
    bool connected() const { return fn_ && *fn_ != nullptr; }
private:
    std::shared_ptr<std::function<void()>> fn_;
};

/// A type-safe signal. Slots are plain callables; connect() returns a Connection.
template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;

    Connection connect(Slot slot) {
        std::lock_guard lk(mutex_);
        std::uint64_t id = ++nextId_;
        slots_.push_back({id, std::move(slot)});
        auto* self = this;
        return Connection([self, id] { self->disconnectId(id); });
    }

    void disconnectAll() {
        std::lock_guard lk(mutex_);
        slots_.clear();
    }

    std::size_t slotCount() const {
        std::lock_guard lk(mutex_);
        return slots_.size();
    }

    void emit(Args... args) const {
        std::vector<Slot> copy;
        {
            std::lock_guard lk(mutex_);
            copy.reserve(slots_.size());
            for (auto& s : slots_) copy.push_back(s.slot);
        }
        for (auto& s : copy)
            if (s) s(args...);
    }
    void operator()(Args... args) const { emit(std::forward<Args>(args)...); }

private:
    void disconnectId(std::uint64_t id) {
        std::lock_guard lk(mutex_);
        std::erase_if(slots_, [id](const auto& e) { return e.id == id; });
    }
    struct Entry { std::uint64_t id; Slot slot; };
    mutable std::mutex mutex_;
    std::vector<Entry> slots_;
    std::uint64_t nextId_ = 0;
};

/// Convenience: connect a signal to a member function of an Object.
template <typename... Args, typename T, typename M>
Connection connect(Signal<Args...>& sig, T* obj, M method) {
    return sig.connect([obj, method](Args... a) { (obj->*method)(a...); });
}

} // namespace aui
