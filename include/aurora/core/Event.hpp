#pragma once
// Aurora UI - Event hierarchy (QEvent analogue)
#include <string>
#include "Geometry.hpp"

namespace aui {

enum class EventType {
    None, Paint, Resize, Move, Close, Show, Hide, FocusIn, FocusOut,
    MousePress, MouseRelease, MouseMove, MouseDblClick, Wheel,
    KeyPress, KeyRelease, Timer, User = 1000
};

enum class MouseButton { None, Left, Right, Middle };
enum class Key {
    Unknown = 0, Escape = 256, Return, Enter, Tab, Backtab, Backspace, Delete,
    Left, Right, Up, Down, Home, End, PageUp, PageDown, Space = 32
};
enum KeyMod { NoMod = 0, Shift = 1, Ctrl = 2, Alt = 4 };

class Event {
public:
    explicit Event(EventType t) : type_(t) {}
    virtual ~Event() = default;
    EventType type() const { return type_; }
    void accept() { accepted_ = true; }
    void ignore() { accepted_ = false; }
    bool isAccepted() const { return accepted_; }
private:
    EventType type_;
    bool accepted_ = true;
};

class PaintEvent : public Event {
public:
    explicit PaintEvent(Rect region) : Event(EventType::Paint), region_(region) {}
    Rect region() const { return region_; }
private:
    Rect region_;
};

class ResizeEvent : public Event {
public:
    ResizeEvent(Size n, Size o) : Event(EventType::Resize), new_(n), old_(o) {}
    Size size() const { return new_; }
    Size oldSize() const { return old_; }
private:
    Size new_, old_;
};

class MouseEvent : public Event {
public:
    MouseEvent(EventType t, Point pos, MouseButton btn, MouseButton btns, int mods = 0)
        : Event(t), pos_(pos), button_(btn), buttons_(btns), mods_(mods) {}
    Point pos() const { return pos_; }
    MouseButton button() const { return button_; }
    MouseButton buttons() const { return buttons_; }
    int modifiers() const { return mods_; }
private:
    Point pos_;
    MouseButton button_, buttons_;
    int mods_;
};

class WheelEvent : public Event {
public:
    WheelEvent(Point pos, int delta) : Event(EventType::Wheel), pos_(pos), delta_(delta) {}
    Point pos() const { return pos_; }
    int delta() const { return delta_; } // +/-120 per notch, like Qt
private:
    Point pos_;
    int delta_;
};

class KeyEvent : public Event {
public:
    KeyEvent(EventType t, int key, std::string text, int mods = 0, bool autorep = false)
        : Event(t), key_(key), text_(std::move(text)), mods_(mods), rep_(autorep) {}
    int key() const { return key_; }
    const std::string& text() const { return text_; }
    int modifiers() const { return mods_; }
    bool isAutoRepeat() const { return rep_; }
private:
    int key_, mods_;
    std::string text_;
    bool rep_;
};

class TimerEvent : public Event {
public:
    explicit TimerEvent(int id) : Event(EventType::Timer), id_(id) {}
    int timerId() const { return id_; }
private:
    int id_;
};

} // namespace aui
