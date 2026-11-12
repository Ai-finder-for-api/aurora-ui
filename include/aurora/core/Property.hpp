#pragma once
// Aurora UI - Observable properties with change signals & bindings
#include <functional>
#include <utility>
#include "Signal.hpp"

namespace aui {

template <typename T>
class Property {
public:
    Property() = default;
    explicit Property(T v) : value_(std::move(v)) {}

    const T& get() const { return value_; }
    const T& operator()() const { return value_; }
    operator const T&() const { return value_; }

    Property& operator=(T v) { set(std::move(v)); return *this; }
    void set(T v) {
        if (value_ == v) return;
        old_.push_back(value_);
        value_ = std::move(v);
        changed.emit(value_);
    }

    /// Bind this property to follow another (Qt-style property binding).
    Connection bind(Property<T>& other) {
        set(other.get());
        return other.changed.connect([this](const T& v) { set(v); });
    }

    Signal<const T&> changed;

private:
    T value_{};
    std::vector<T> old_; // retained for undo/redo stacks
};

} // namespace aui
